# 旋转模式实现与验证报告

实施跨越 2026-10-09～10。适用 UE 5.5.4，Win64 Development。设计依据：[角色旋转与移动动画模式调整](../Design/character-facing-modes.md)；实际 API：[配置与接口指南](../Guides/character-facing-configuration.md)。

## 1. 实现范围

- 新增方向类型、原始输入／世界方向快照和带初始化代的请求句柄。
- 扩展现有 RotationComponent：Movement／Controller 基础模式、定向动作请求、约束优先、限速恢复、结果事件、配对释放、失效来源清理及必要复制。
- CMC 自定义移动数据只上传请求序号，服务器查已授权状态；跨模式不合并 SavedMove，回放后恢复最新正常开关。未授权序号不授予旋转权限。
- 基础 GA 结束释放自己拥有的旋转请求；不清其他来源的控制或 Tag。
- AnimInstance 缓存同版模式结果；Free 关闭方向补偿，ReservedStrafe 保留方向图，权重归零后重置模型偏移。
- Main 主图和固定层接入 9 个 Free／ReservedStrafe 方向资源入口、4 个 Warping 权重入口。原方向计算、资源、FullBody、轻反馈 Group 和 IK 保留。
- 编辑器作者入口及真实 PIE 测试入口沿用现有 Editor 模块。未新增 Runtime 模块或改引擎／插件版本。

不包含完整锁定目标服务、锁定 CameraMode、Dash／Sprint 执行、体力或无敌系统；ReservedStrafe 用明确的测试上下文验证，不宣称已有产品锁敌功能。

## 2. 实现假设与文档歧义处理

1. 基础切换统一平滑，动作起手默认 Pending→Applied；只有显式 bInstantAtStart=True 才允许瞬时准备，且不突破旋转冻结。
2. 同种请求以合法接受顺序决胜，动作优先于基础请求；冻结独立叠加。32 个活动请求上限，已结束结果／服务器历史有限保存。
3. Controller 模式只跟随有效控制 Yaw。组件默认允许此模式能力，但不搜索目标；服务器配置、来源和状态仍可拒绝请求。
4. Definition 的 ExecutionId 是各端生成的本地 GUID。跨端权限使用 SpecHandle／PredictionKey 验证，服务器映射自己的执行 GUID，不要求两端随机 GUID 相等。
5. Source 为 Pawn／其拥有对象可申请基础模式；客户端动作来源必须是当前活动 GA。Source 弱持有，结束、垃圾标记和失去 Avatar 都会清理。
6. 八向“隔离”首版采用固定层资源入口与 Warping 权重分支，共享移动数据和主实例；不复制整套主 AnimInstance。Sequence 切换保留原惯性混合。
7. Pivot 保持既有 EnablePivot=False；本轮不自动启用素材尚待验收的掉头功能。空中延续与最终艺术观感仍按后续角色规则评审。
8. 保留 GetMoveIntent 的二维语义；世界方向是新增快照。输入布尔事件不改为每帧方向事件。

## 3. 构建与蓝图

实际执行以下常规命令，Editor／Game 均退出 0；未使用 Live Coding：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
```

Editor 最终工具构建日志 EditorBuild11.log；Runtime／Game 验证日志 GameBuild3.log、GameBuild5.log，后者确认目标已更新。新 Editor 验证函数不引入 Game 依赖。

两份正式 AnimBP 原生编译：ERRORS=0、WARNINGS=0；编译信息中保留 FootPlacement 实验性提示及未用引脚信息。接线重复执行返回 DirectionEntries=9、WarpingEntries=4，不重复生成方向分支。

原有 Build.cs／插件依赖及 IncludeOrder 提示、既有 UI 弃用警告保留；不通过改引擎或清理无关代码使本次构建通过。

## 4. 单元与原生回归

新增 Hodge.Facing 六组：InputSnapshots、ModesAndPriority、ActionsAndConstraints、LifecycleAndExceptions、ReplayAndNetworkData、AnimationSnapshot。

覆盖输入空间、模拟量、零／非有限方向；模式优先和双释放；动作准备与显式瞬时定向；冻结来源组合；失效来源、死亡、无 Controller、重复绑定与旧句柄；容量上限；历史模式回放与退出开关恢复；真实网络移动数据序列化；动画快照与权重退出。

一起执行 Hodge.Rotation、Hodge.Combo、Hodge.Combat、Hodge.HitReaction，总计 **24 项：15 无警告成功、9 带日志警告成功、失败 0、未运行 0**。警告包含原有 GameplayCueNotifyPaths、属性／组件诊断，以及测试 Pawn 无 PlayerState 的现有 PossessedBy 日志。

实际自动化参数：

```text
UnrealEditor-Cmd.exe Hodgepodge.uproject -unattended -NullRHI -nosplash
-ExecCmds="Automation RunTests Hodge.Facing+Hodge.Rotation+Hodge.Combo+Hodge.Combat+Hodge.HitReaction"
-TestExit="Automation Test Queue Empty"
-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/FacingImplementation/Native2
```

Native2/index.json 是通过依据。仅退出码 0 或 Test Queue Empty 不算通过，脚本同时检查 failed=0、notRun=0。

## 5. 端到端与集成结果

真实单人 PIE、Play As Client 两客户端＋PIE 服务器的 0ms／100ms 三组，各 **8 个场景通过，共 24 个场景**：

- 自由模式静止转镜头不转角色。
- 自由侧向输入使角色转向并选择 A_Pover_Jog_F。
- ReservedStrafe 侧移选择 Jog_R，后退选择 Jog_B，角色保持控制朝向。
- 释放模式恢复 Free。
- 在真实 Definition GA 中申请动作覆盖，角色转向 90°；释放后恢复仍有效的 Controller 模式。
- 服务器权限拒绝后拥有者撤销预测并恢复 Free。

拥有者／服务器／观察者均检查 Driver、Style、模型快照和运动结果；三个视角在 0ms 组最终动作朝向分别一致为 0°或90°。100ms 是每个参与 NetDriver 的模拟包延迟，不声明真实 RTT=100ms。

另外复用真实攻击 InputAction、蒙太奇通知和 GE：0ms／100ms 各完成移动取消、正式五段连段和默认短受击，**6 个集成回归组通过**。检查连段顺序 01→02→03→05→04，移动取消后双方攻击能力结束，真实受击期间拥有者进入 Controlled。

以上全部使用正式 Main Hero、主 AnimBP、固定层和目标 Profile；隔离的 CodexText 探针只提供测试能力。没有覆盖正式攻击定义／Impact，没有保存临时探针 Reaction 修改。

重现入口见 [Tools/CharacterFacing](../../Tools/CharacterFacing/README.md)。原始结果与逐帧采样在 Saved/FacingImplementation，战斗回归在 Saved/AnimationNetworkReview/facing-regression-0ms/100ms.json。

## 6. 覆盖率报告与限制

使用 OpenCppCoverage 0.9.9.0，对 MSVC PDB 可测编译行采集。工具安装到忽略的 Saved/FacingImplementation/OpenCppCoverage，没有改变引擎、构建设置或项目依赖。

统计对象是六个 Runtime 相关 cpp 与两个 Editor 工具 cpp。整个文件包含大量本次未改的旧代码，因此同时报告整个文件和 git 差异行的覆盖率；不可将整个文件比例当作新功能完成比例。

可重现的计算脚本：[summarize_results.py](../../Tools/CharacterFacing/summarize_results.py)。提交的结果摘要见[测试与覆盖率 JSON](character-facing-results-2026-10-10.json)，实际最新数字以该 JSON 为准。

最终仅合并 Native2 和干净的覆盖率会话：Runtime 差异可测行 **282/307，91.86%**；包含 Editor 工具的差异行 **325/429，75.76%**；八个完整 cpp 的可测行 **869/1636，53.12%**。首次接线构图发生在较早会话，最终干净会话没有重新删除已工作的节点来提高覆盖率，因此作者构图路径并未全部计入最终数据。

覆盖范围明确限定为 PDB 可测行。优化／内联可能合并或移除源行；无分支覆盖率，不能把原始 XML 的 branches-valid=0 解读成 0% 或100% 分支覆盖。没有蓝图节点行覆盖率百分比，蓝图使用编译、幂等作者操作、实际资源选择和端到端结果证明。

原始 XML、二进制合并数据、HTML 报告在 Saved/FacingImplementation。未覆盖的差异行列入 JSON，包括配置异常、有限历史回收、过时结果及部分首次作者建图路径；不宣称 100% 全路径覆盖。

## 7. 发现的问题与处理

### 已处理

- UE 5.5 不允许 BlueprintReadOnly uint32：表现版本改为 int32，请求序号保持内部 UPROPERTY uint32，并明确序列化。
- 部分引擎 API 名称／访问级别与假设不同：修正 GetAllGraphs、const LinkedAnimInstances 读取、测试访问及必要头文件，保持5.5.4兼容。
- 初次单元夹具创建抽象 UObject 触发 ensure：改为具体组件；保留 Native1 失败报告，Native2 全部通过。
- K2 Select 索引类型带 index 子类别造成连接不兼容：清理类型标记并使用明确枚举值类型，正式图随后编译通过。
- 隐藏编辑器没有主窗口句柄，CloseMainWindow 未实际退出，导致一次 DLL 链接占用：改用无脏资产时的正常 QUIT_EDITOR，并核实退出后构建。
- 默认编辑器地图为主菜单，没有角色：验证入口显式加载 ThirdPersonMap，不改变默认地图配置。
- 测试脚本直接修改 EditDefaultsOnly 实例字段失败：添加仅 Editor PIE Authority 的验证函数，Runtime 游戏权限接口未放宽；清理失败也写 JSON。
- 无窗口测试创建第二本地玩家触发 CommonUI SlateUser ensure：单人夹具改为一个真实玩家，普通编辑器干净复跑 8 项通过；保留旧覆盖会话与异常退出记录。
- 覆盖率进程旧会话记录 0x80000003 退出：弃用该异常会话的最终合并数据；单玩家夹具和正常窗口环境重新采集三组 24 场景，清理后退出码 0，日志无 ensure／fatal，最终报告采用这次干净数据。
- 战斗回归修改探针 Reaction 导致测试资产脏包：结束后仅重载明确的 CodexText 探针包，不保存或重载用户正式资产。

### 剩余范围与风险

- 完整锁定服务／锁定 CameraMode、空中朝向策略、Pivot 启用、素材角度与艺术品质仍未实施／验收，属于设计范围外或待决事项。
- 未执行跨机器、独立进程、长时间丢包／抖动压力、Cook 和打包；不把同进程 PIE 服务器描述为独立服务器可执行文件。
- 未达到全分支或全部异常组合覆盖。保护性回退、历史容量边界与消息极端乱序仍需持续扩展。
- 旧带日期文档与部分知识库基线保留历史值，最新模式与实际操作以本设计／配置指南为准。

## 8. 工作区与交付边界

仅提交本轮源码、两份正式动画图、测试工具及相关设计／配置／验证文档。保留用户已有 GE_MeleeDamage_Instant、五份攻击蒙太奇和 BP_Hero_Pover 改动；BP_Hero_Pover 与本轮开始备份哈希一致。

生成文件、缓存、覆盖率工具安装、原始日志与备份不提交。PIE 结束、监听解绑、网络模拟 Off、脏测试包重载后再结束编辑器。提交本地 Git，不自动推送远程。
