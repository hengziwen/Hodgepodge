# Definition 连击实现与验证

当前状态：第一版运行时、五段资产和默认授予迁移已完成，最终两目标构建、原生自动化及冷启动单人回归通过。以下记录区分实现范围、实际验证与未验证项。

## 实现接口

- `UHodgeAbilityDefinition` 单向引用 AbilityClass；ExecutionConfig 配置 Montage、PlayRate、BlendIn、NaturalBlendOut、StopBlendOut 和 TimelineTaskConfig。
- `FHodgeAbilityBlendSettings` 分开保存 Mode 与 Curve。第一版仅接受 Standard；自然混合在源动画结束后开始，主动停止使用独立配置，不修改共享 Montage。
- AbilitySet 新增 GrantedAbilityDefinitions，原 GrantedGameplayAbilities 保留。ASC 用 SpecHandle 单独关联 Definition，owner-only 复制，原 SourceObject 保持装备来源语义。
- `UHodgeComboDefinition` 引用使用 `FHodgeComboRow` 的 DataTable；Priority 大者优先，同级按数组顺序。
- PawnData 新增 ComboDefinition。协调器作为 PlayerState 默认组件，随 PawnExtension 初始化/解绑；每段由单独的 `UHodgeGameplayAbility_Definition` 子蓝图执行。
- Timeline 保留独立时钟接口，新增 Montage 实例时钟。有效 Duration 来自 Montage，窗口使用源动画秒数，不再再次乘播放倍率。
- Timeline 对事件采用单调消费位置，防止校正重复施加副作用；窗口授权同时检查实际 Montage 位置和本实例账本，不依赖 ASC 聚合标签。
- 输入缓存默认 0.3 秒、单槽、新输入覆盖旧输入；切段前预检查失败保留当前动作，实际切换失败回 Entry。
- 节点标签由拥有端与权威端各维护本地账本，模拟代理接收 skip-owner 标签快照，避免拥有端重复计数。
- 同一 ASC 内重复 AbilityTag 不会任意选择其中一个 Spec；PawnData 校验要求每个节点解析到唯一授予 Definition。
- 激活过程中到达的 Timeline 事件按执行身份暂存，切换完成后消费；单次事件链最多处理 32 次，超过则报错并清理回 Entry，避免零时间循环卡死。

## 网络 A 实现边界

输入切段沿用 GAS LocalPredicted 激活 RPC。请求携带来源节点、来源执行 PredictionKey、Avatar 和 ComboDefinition。服务器按收到时当前执行窗口重新选边，不接受客户端直接指定的目标作为授权，不提供历史窗口补偿。

Timeline 事件派生由权威端触发，使用 GAS server-initiated prediction key 将选定的新段通知拥有端；客户端不能自行声明权威事件已经发生。

移动取消先检查本地执行窗口，再由服务器核验当前执行身份和窗口并确认取消。因此第一版网络移动取消会等待服务器确认，有一个网络往返的反馈成本。

## 资产

新资产目录：`/Game/CodexText/DefinitionCombo`。

- `GA_Attack_1` 至 `GA_Attack_5`。
- `DA_Attack_1` 至 `DA_Attack_5`，及对应 `DA_Attack_N_Timeline`。
- `DT_LightCombo`、`DA_LightCombo`、`AS_LightCombo`。
- 行名为 `Combo.Entry`、`Combo.Light.01` 至 `Combo.Light.05`，分别解析 `Ability.Attack.Light.01` 至 `.05`。
- 原始五个 `/Game/CodexText/Montage/AM_Attack0N_Montage` 未改写。窗口开始暂设为源动画长度的 60%；前四段含接段窗口，五段均有移动取消窗口。
- `/Game/Main/Data/DA_Dafult_PawnData` 已保存 `ComboDefinition=DA_LightCombo`，AbilitySets 包含原 DA_Pover 与新增 AS_LightCombo。
- `/Game/Main/Data/DA_Pover` 已移除旧 GA_BasicAttack 授予；其余效果与属性授予保留。旧 GA 和 Montage 均未删除。
- 默认资产及临时测试前的配置备份在 `/Game/CodexText/Backups/DefinitionCombo_20260928`。事件派生、倍率和失败测试只改内存，已恢复正常五段配置。

## 已执行

- 常规 Editor 与 Game 的 Win64 Development 构建通过，命令分别为 `Build.bat HodgepodgeEditor Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64` 和对应 `Hodgepodge` 目标。最终测试夹具修订后两目标再次构建通过，退出码均为 0；未使用 Live Coding 代替构建。
- 冷启动已核对两份默认资产、五段 Montage/倍率/Timeline 和六行跳转表的保存结果，未残留临时事件、倍率或 Entry 测试配置。
- 五个 GA 经 MCP 编译均返回 `compiled=true, blueprint_status=3`，已保存。
- PawnData、ComboDefinition、五份 AbilityDefinition 经 EditorValidatorSubsystem 检查全部 Valid，无错误或警告。
- 单人 PIE 14 项检查通过，已在冷启动后直接使用磁盘默认配置重跑：单击第一段、正确 Montage、标签计数、自然结束及清理、过期输入不接段、五段连击、移动取消窗口、有效接段优先于移动。
- 双人 Listen Server：拥有客户端 23 项、主机 19 项通过，覆盖服务器/拥有端/观察者的 Montage 播放、停止与节点标签计数。
- 网络模拟 15 项通过：750 ms PktLag 下合法请求接受；权威源执行已结束、客户端仍预测接段时拒绝并清理；100 ms PktLag + 10% 丢包 + 10% 重复包下最终收敛。所有模拟参数恢复为 0。
- 迟到测试先确认服务器已回 Entry，再发送客户端仍处于窗口内的请求；不以客户端墙钟时间推断服务器 Montage 一定结束。本轮没有历史窗口宽限。
- 权威事件派生通过：源动画 0.8 秒事件以及 0 秒起点事件都验证进入第三段，第三段自然结束进入第四段，最后两端回 Entry。
- 2 倍播放、暂停/恢复和外部同名窗口隔离共 11 项通过。Timeline 不因暂停推进，也不使用外部 ASC 标签授权接段。
- 激活预检查失败保留源段、实际执行失败回 Entry、动画打断、重生与保留 PlayerState ASC 共 9 项通过；客户端输入边跳回 Entry 的双端清理另有 3 项通过。
- 原生自动化新增 SessionRules，覆盖稳定优先级、源标签条件、单槽替换、重复初始化和 Avatar 更换清理。冷启动后执行 `Automation RunTests Hodge.`，全部 6 个用例通过：DefinitionGrant、GraphValidation、SessionRules、CleanupAndOrdering、RejectBeforeSideEffects、Validation。
- 脚本在 `Tools/BasicAttack`，机器结果在 `Saved/Tests/definition_combo_*.json`（不纳入源码）。`save_definition_defaults.py` 校验后只保存两个默认数据资产；临时测试脚本不应保存到正式配置。

## 操作与配置入口

打开 `/Game/ThirdPerson/Maps/ThirdPersonMap`，使用默认玩家运行 PIE。鼠标左键单击第一段；在每段后半段再次点击，或在开窗前最多 0.3 秒点击，依次进入后续段。停止输入则本段自然结束；窗口打开后移动可取消后摇。有效接段和移动同时成立时先接段。

动画、倍率与三类混合在 `DA_Attack_N.ExecutionConfig` 修改；源动画秒数窗口在 `DA_Attack_N_Timeline.Events` 修改；段数跳转在 `DT_LightCombo.Transitions` 修改；输入绑定、缓存时长和移动取消标签在 `DA_LightCombo` 修改。Timeline 的有效运行长度由当前 Montage 派生，源时间窗口仍需落在该长度内。GA 蓝图继承共用执行基类，不需要复制五份事件图逻辑。

## 未包含与未验证

本轮没有攻击判定、伤害、组合触发 B、历史窗口网络 B、Section 跳转/循环或图形编辑器。未执行 Cook/打包、独立进程或 Dedicated Server 测试。混合模式限 Standard；曲线字段与播放路径已接通，但未逐条曲线进行画面测量。GameFeature 卸载、战斗成本 GE 的网络拒绝矩阵未做端到端验证；授予撤销映射和会话清理使用原生测试覆盖。

PIE 测试设置最终恢复为 Standalone / 1 玩家；不将本轮 Listen Server 结果描述为所有网络部署模式通过。

当前项目的 CommonUI 在 PIE 输出 `Using CommonUI without a CommonGameViewportClient derived game viewport client`。本轮连击测试通过 Enhanced Input 注入验证玩法链路，没有验证这部分 UI 输入路由，也没有改动 UI 迁移配置。构建还保留已有的 IsFocusable 弃用、目标 IncludeOrderVersion 等警告。

## 工作区恢复说明

2026-09-28 09:47，GitHub Desktop 将此前连击工作保存到 stash `7da06efa0eeac25ee3ef1806820860454010fcc2`。继续任务时已只恢复其中 46 个连击文件；GameplayTags 使用追加合并，42 个其他已修改文件通过哈希核对保持不变。未弹出或删除 stash，未自动提交。


## 追加验证：Timeline 自动使用 Montage 时长

2026-09-28，新增 `Use Montage Duration` 模式，开启后隐藏手动 Duration。Definition 要求此模式并按 Montage 长度检查上界；Timeline 单独校验不再依赖旧 Duration。独立计时资产保持原默认模式。设计细则见设计文档第 16 节。

实际执行与结果：

- `Build.bat HodgepodgeEditor Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64`：退出码 0，UHT、编译、链接通过。构建前检查无脏资产并正常关闭编辑器，未使用 Live Coding 替代。
- 同样参数执行 `Build.bat Hodgepodge Win64 Development`：退出码 0。保留已有 IncludeOrderVersion、IsFocusable 弃用警告。
- MCP 执行 `Tools/BasicAttack/migrate_timeline_montage_duration.py`：沿当前 Definition 引用迁移 BasicAttack 目录的 `DA_Attack01_Timeline` 至 `DA_Attack05_Timeline`；五个 Timeline 与五个 Definition 的数据校验均为 Valid。原资产逐字节备份至 `Saved/Backups/TimelineMontageDuration_20260928/CodexText/BasicAttack`，事件列表和旧 Duration 数值不变。
- 编辑器执行 `Automation RunTests Hodge.`：7/7 成功，包括新增 `Hodge.Combo.MontageDuration` 及扩充的 `RejectBeforeSideEffects`。覆盖忽略旧时长、独立模式兼容、缺失 Montage 上下文拒绝、Definition 动画边界和源时间不受倍率缩放。
- MCP 执行 `Tools/BasicAttack/verify_montage_duration.py`：Standalone PIE 中临时将五个 Timeline 的旧 Duration 设为 0；五段均播放正确的 Montage，按当前跳转表 `1→2→3→5→4` 完成，最终回 Entry，攻击、Recovery、MoveCancel、NextAttack 标签全部清理，共 10 项检查通过。首轮脚本仍假定旧段序而失败，检查实际跳转表后修正测试，未修改跳转表或技能资产引用。
- 测试恢复原 Duration，确认后保存这五个资产，以不丢弃脏包的模式重新加载；五个保存后的模式、原时长及资产校验再次通过。最终无 PIE、无脏内容包；恢复启动时的 `/Game/CodexText/L_MainMenu` 地图和后台 CPU 节流设置。
- `git diff --check`、三个相关 Python 脚本的 AST 语法检查通过。结果在 `Saved/Tests/timeline_montage_duration_migration.json` 和 `timeline_montage_duration_pie.json`，不纳入源码。

本次没有重新进行联机、Cook/打包、Dedicated Server 或蓝图批量重新编译；不将单机播放与常规构建结果描述成这些检查已通过。第 15 节三项审查问题继续暂缓，未修复。


### 多客户端后台掉帧排查

用户随后反馈多客户端掉到个位数。上一轮曾临时关闭 `EditorPerformanceSettings.bThrottleCPUWhenNotForeground`，结束时恢复测试前读到的 True，但未验证后台多客户端性能。本次使用当前单进程、2 客户端设置，在同一 PIE 会话依次采样开启/关闭此选项（各 7 秒，忽略起始 1 秒）：开启约 3.00 帧/秒、333.13 ms；关闭约 79.33 帧/秒、12.61 ms。数值来自 Slate post-tick 墙钟帧间隔，不是 GPU 用时或网络 RTT。结果确认后台节流可复现此次个位数现象，未证明所有场景都已恢复 120 FPS。

当前编辑器会话已关闭后台 CPU 节流，诊断 PIE 已请求停止；没有修改客户端数量、分辨率、画质、Timeline 或网络模拟参数。采样文件：`Saved/Tests/multiclient_throttle_comparison.json`。该设置本次通过内存对象修改，未额外写入用户配置文件。


### 2026-09-29：临时设置审计与启动采样范围修正

- 当前后台 CPU 节流为 False；t.MaxFPS、t.IdleWhenNotForeground、VSync 均为 0，ClientFixedFPS 为空、ServerFixedFPS 为 0；没有 Benchmarking / FixedTimeStep。五段 PlayRate 均为 1，手动 Duration 已恢复，未残留测试用 0。
- NetworkEmulationSettings 明确禁用，进出包延迟和丢包配置均为 0；另通过 native console 执行 `NetEmulation.Off` 清空进程内持久模拟。NetEmulation.PktLag 等是控制台命令，不应把 get_console_variable_int_value 返回 0 作为其状态证据。
- Win32 只读检查发现编辑器主窗口最小化。已恢复为可见正常窗口。UE 5.5 EditorEngine.cpp 的 ShouldThrottleCPUUsage 在后台选项关闭时仍会检查 AreAllWindowsHidden；这解释了关闭选项后空闲编辑器仍可能为 3 FPS，但尚不能证明用户联网启动掉帧的完整原因。
- 从 PIE 启动前采样 35 秒：首秒平均约 49 FPS、单帧最大 294 ms；之后各段约 80 FPS。核实日志后发现此次是两个 Standalone 世界，并非联网双客户端，因此此结果只作为双窗口对照，不能当成联网问题已解决。采样文件为 Saved/Tests/multiclient_startup_samples.json。
- 本轮采样回调按时注销；检查未发现此前 BasicAttack 测试 tick / 节流比较 measure 回调函数残留。当前无诊断 PIE。联网模式枚举无法通过现有 Python 反射设置，控制台 Set 在编辑器禁用，native 属性工具不接受该设置对象；已请用户通过运行菜单设置网络模式后再继续联网采样。未修改画质、分辨率、技能代码或资产。
