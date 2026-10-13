# 软锁定镜头修正与验收 · 2026-10-13

依据[锁定设计 v0.4](../Design/lock-on-system.md)和[网络调研／修正方向](../Design/soft-lock-camera-research-2026-10-13.md)。用户确认：保留中键选目标，先修镜头，不加入攻击自动索敌。当前使用 UE 5.5.4，没有升级插件或构建设置。

## 1. 根因与实际 CameraMode 链路

此前目标保持与网络检查通过，不能说明镜头操作正确；旧 E2E 没有充分检查真实视角与模式混合。

- 锁定组件截取鼠标，横向缓存 0.12 秒后只允许 ±8° 偏移；大幅输入还会被切换手势丢弃。因此玩家不能自由环绕。
- 镜头持续追双方中点，PlayerController 又把控制角度改写为目标方向，临时技能镜头仍可能丢失输入或被角色朝向抢写。
- CameraMode 本身确实经项目自带的组件和栈运行，但 GetBlendInfo 读了 Last() 栈底而不是索引零的栈顶，造成模式诊断错误。
- 首次进入才初始化视角不覆盖“仍在混合栈中再次晋升”的情况；动态 Profile 混合时间也需要在 Push 判断前准备。
- 在混合期间，仅给锁定层应用输入会使实际位移被权重削弱；如果为过渡层观察输入，又必须防止完全隐藏期间的旧输入在返回时再次应用。
- GA.SetCameraMode 的“系统被注释掉”注释已经过时，调用实际存在；本次修正此注释。

实际链路仍是：**GA.SetCameraMode／ClearCameraMode → Hero.SetAbilityCameraMode／ClearAbilityCameraMode → DetermineCameraModeDelegate → HodgeCameraComponent → CameraModeStack → PlayerCameraManager POV**。没有新增 SpringArm、独立相机 Actor 或绕开模式栈的 SetWorldRotation。

## 2. 已完成的修正

1. **自由环绕：完成。** 原始输入继续走 Controller；锁定层观察同一帧等效角度，混合输出不重复加位移。取消横向八度限制／缓存，Pitch 保留可配置安全范围。
2. **柔和构图：完成。** 停手宽限 0.75 秒；Yaw／Pitch 死区 20°／12°；指数锐度 3，自动角速度最多 120°／秒和 60°／秒，最短弧处理 ±180°。可以独立关闭辅助。
3. **切换目标：完成。** 保留中键与甩动规则，甩动不吞相机输入；可关闭甩动检测。获取要求屏幕内，左右切换允许前方获取视锥中的屏幕外邻居。
4. **角色面向分离：完成。** ControlRotation 表达视角；Hero.FaceRotation 阻止提前按视角旋转锁定角色，CMC 从确认目标解析角色面向。Dash／Sprint 原有覆盖与恢复保留。
5. **CameraMode 仲裁：完成。** 技能优先于锁定，锁定优先于默认。临时模式期间禁止自动构图／目标切换，但保留鼠标输入；结束覆盖／解除锁定继承实际画面。
6. **混合连续性：完成。** OnBecomeTop 在计算混合前准备动态 Profile 并继承上一帧实际输出；首次进入与栈内晋升都适用。返回时丢弃历史输入，保留同帧尚未求值的输入。
7. **诊断与配置：完成。** GetBlendInfo 返回真正栈顶；GetDesiredCameraMode 与 GetActiveCameraModeClass 区分期望／实际。正式 DA_HeroLockOn 保存新参数，旧 FramingYawLimit 仅保留序列化兼容。

主要文件：

- [软镜头状态与角度算法](../../Source/Hodgepodge/Private/Targeting/HodgeLockOnTypes.cpp)、[锁定组件](../../Source/Hodgepodge/Private/Component/HodgeLockOnComponent.cpp)、[配置](../../Source/Hodgepodge/Public/Data/HodgeLockOnProfile.h)。
- [相机模式](../../Source/Hodgepodge/Private/Camera/HodgeCameraMode_LockOn.cpp)、[组件](../../Source/Hodgepodge/Private/Camera/HodgeCameraComponent.cpp)、[模式栈](../../Source/Hodgepodge/Private/Camera/HodgeCameraMode.cpp)。
- [Hero 输入](../../Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp)、[角色 FaceRotation](../../Source/Hodgepodge/Private/Character/HodgeHeroCharacter.cpp)、PlayerController 移除目标旋转覆盖。
- [原生测试](../../Source/Hodgepodge/Private/Tests/HodgeLockOnTests.cpp)、[真实镜头测试](../../Tools/LockOn/run_camera_e2e.py)、[批次运行工具](../../Tools/LockOn/run_validation_batch.py)。

资产：DA_HeroLockOn 修改配置；BP_Hero_Pover 和 `/Game/Main/Camera/CM_ThirdPerson` 执行编译／保存。没有修改 Guardian 插槽、行走、攻击通知、翅膀或 HUD 布局。任务前快照在 Saved/LockOnCamera/Before，完整源文件／资产 SHA256 见[结果 JSON](soft-lock-camera-2026-10-13-results.json)。

## 3. 实际结果与需求追溯

- **Editor／Game Win64 Development 常规构建：通过，退出 0。** 关闭已保存编辑器后执行，反射／虚函数变化后冷启动，没有以 Live Coding 替代。
- **31/31 原生回归 Success。** LockOn 11、Facing 8、Rotation 1、Movement 8、UI 3。新增 SoftCameraManualAndAssist 和 CameraModePriorityAndContinuity；校验无效参数、自由环绕／Pitch 边界、停手优先、最短弧／死区、关闭辅助、30／120fps 算法一致性、历史输入与同帧输入、真实栈权重／动态混合／技能优先／返回和角色抢写。
- **镜头专项：四种模式各 24/24，总计 96 项。** single、listen、dedicated PIE、listen-lag（各网络驱动 Net PktLag=100）。
- **原锁定／Dash／Sprint 回归：四种模式各 20/20，总计 80 项。** 包含真实中键映射、左右切换、标签／目标复制、动作暂让／恢复、死亡清除。
- **正式资产核对：10/10。** Profile／授予／中键／HUD／真实胸口锚点均有效；新软镜头参数另外记录在配置 JSON。
- **蓝图：** 正式 Hero 与默认 CameraMode 已执行 Compile／Save，新增编译日志无 Blueprint Error。Blueprint.Status 是受保护字段，Python 无法直接读取，本次不把该字段伪造为成功状态。
- **恢复：** PIE=0，无未保存地图／资产，恢复本轮原地图 L_EnemyAIValidation 与性能配置；原完整播放选项未记录，明确恢复单人 Standalone 基线。测试网络驱动随 PIE 销毁，截图原件已还原。

镜头需求断言：

- **CAM-01**：实际 Hero 使用 HodgeCameraComponent，自由状态取 PawnData 默认模式。
- **CAM-02**：中键进入实际 LockOn 模式栈，获取不突然跳到角色朝向。
- **CAM-03**：实际画面越过旧八度限制；慢速累计环绕超过 240°；停手宽限保留视角，不误切目标。
- **CAM-04**：环绕时角色依然面向目标，不随相机抢写。
- **CAM-05**：停手后修正实际 POV，角度变化平滑且有界。
- **CAM-06**：临时模式实际成为栈顶、接管输入、目标保持、结束连续恢复。
- **CAM-07**：解除连续返回默认镜头，自由输入继续工作。
- **CAM-08**：获取与技能覆盖的混合过程中，本帧 6° 输入实际输出仍为 6°，没有被权重削弱。

中键经所属 PC.InputKey 排入游戏帧，视角／移动等经 EnhancedInput 注入；记录的是实际 PlayerCameraManager POV、ControlYaw、ActorYaw、模式类型和待确认状态。CameraOverride 使用真实 Hero 相机所有权接口与已授予 Spec；它验证接口链，不代表制作并测试了全部正式技能演出 GA。

[所属玩家截图](LockOn/soft-camera-single.png)用于检查布局／构图；逐帧角度与断言用于检查运动，单张截图不能证明过渡流畅。未执行行／分支覆盖率插桩，没有编造覆盖率百分比。

## 4. 执行中发现并闭环的问题

- 测试绑定受保护 DetermineCameraMode 导致构建失败：增加有实际诊断价值的公开 GetDesiredCameraMode，不直接暴露内部字段。
- 直接调用非导出的 FGameplayAbilitySpecHandle.GenerateNewHandle 导致链接失败：使用 ASC 已授予能力的有效句柄；没有硬编码伪造句柄值。
- 模式单元夹具实例化 Abstract 的 ThirdPerson 基类触发 ensure：使用具体测试派生类，不取消生产 Abstract 限制。
- 旧回归在退出混合未结束时强设 ControlYaw=0，随后被画面写回，导致再次选到另一个敌人：等待默认模式稳定后摆放测试视角，没有让生产解除逻辑强行归零。
- 混合期角位移只送一层会减弱输入：两层共享同一帧采样；丢弃隐藏期旧采样，补 CAM-08。
- Python 不允许读取受保护 Blueprint.Status：改为记录真实 Compile／Save 调用并核对新增 Blueprint 编译日志，没有绕过字段保护。

首次失败报告／日志保留于 Saved/LockOnCamera，最后结果分别逐条核对，没有以 gateway 接受脚本或程序退出 0 代替玩法通过。已有弃用 API／测试夹具 PlayerState 调试警告保留，不作为本轮新修复。

## 5. 参数、临时实现与未验证项

参数编辑入口：`/Game/Main/Character/Hero/Targeting/DA_HeroLockOn`，具体字段见[配置手册](../Guides/lock-on-configuration.md)。如果只想手动观察，关闭 bEnableSoftCameraAssist；如果不想转镜头同时切目标，关闭 bEnableMouseFlickSwitch。两者不清中键选择。

**P1 仍需调优／验证：** 0.75 秒宽限、20°／12° 死区、120／60 自动速度是项目原型值，不是《鬼泣》参数；物理鼠标／DPI／主观体验、复杂墙角／高差／完整八向美术与真正技能演出组合未全面验收。

**P2 可扩展：** 群体包围构图、动态距离／FOV、Boss 多锚点、独立相机设置菜单。当前沿用默认 CM_ThirdPerson 的距离曲线和防穿透，不宣称这些扩展已制作。

原生菱形标记仍为占位美术；测试派生类、坐标、角位移、CameraOverride 和 CLI 固定日期／计数仅用于验收，不是生产攻击逻辑。没有新增相机硬编码追敌／伪造服务器授权。手柄、攻击自动索敌／吸附继续不在本次范围。

独立进程／Server Target、丢包、换 Pawn／热卸载、Cook／打包仍未执行；专服 PIE 不等于独立专服通过。原有用户未提交改动保留，没有自动提交 Git。

## 6. 实际命令

```powershell
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.LockOn+Hodge.Facing+Hodge.Rotation+Hodge.Movement+Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/LockOnCamera/Automation' '-abslog=E:/Project/Git/Hodgepodge/Saved/LockOnCamera/automation.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/LockOn/configure_soft_camera.py
& E:/Python/python.exe Tools/LockOn/run_validation_batch.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnCamera/compile_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnCamera/validate_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/LockOnCamera/restore.py
& E:/Python/python.exe Tools/LockOn/export_soft_camera_results.py
git diff --check -- Source Docs Tools
```

原始数据在 Saved/LockOnCamera；最新结果 JSON 固化源文件与报告 SHA256。最终修正 GA 过时注释和文件尾空行后补做常规双构建，没有因此重复行为测试；31 项与八组 PIE 对应相同可执行行为。Python 工具语法、任务文档本地链接、新增注释块与最终 git diff --check 均通过。
