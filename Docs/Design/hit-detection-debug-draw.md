# 命中检测调试绘制

2026-10-06：为现有原生检测策略补充调试绘制，没有修改检测结果、伤害规则或资产配置。实现集中于 HodgeHitDetection.cpp，无新增组件、反射字段或模块依赖。

Weapon / Body 使用每个采样点从上一帧到当前帧的球体 SweepMultiByObjectType。两个 Socket 沿刀身生成 SegmentSamples 个点；无位移绘制球体，有位移绘制球体扫掠形成的胶囊和中心轨迹线。HitBox 使用实际 UBoxComponent 的缩放后尺寸，按 RotationSubsteps 插值查询盒体；每个子步绘制两端盒体和八个顶点的扫掠连线。

三类默认全部开启，每次绘制保留 5 个游戏秒。Weapon 青色、Body 黄色、HitBox 紫色；产生几何命中的扫掠及命中点为绿色。绿色只代表策略几何查询命中，后续还要通过视线、角度、目标 ASC、阵营和 GA 伤害资格校验，不能据此认定已扣血。

## 控制台开关

```text
Hodge.Combat.DebugDraw.Weapon 1
Hodge.Combat.DebugDraw.Body 1
Hodge.Combat.DebugDraw.HitBox 1
Hodge.Combat.DebugDraw.Duration 5
```

对应开关设为 0 可单独关闭；Duration 为每次采样产生的绘制的保留时间，不是命中窗口持续时间。只在命中窗口开启、来源解析成功并实际执行检测后绘制，不会因开启开关就启动攻击。检测在服务器执行，单人或 Listen Server 的服务器视窗能观察绘制；没有为远端客户端复制调试形状。Dedicated Server 不绘制，代码受 ENABLE_DRAW_DEBUG 保护。

要显示第一段正式武器检测，仍需 DA_Attack_1.HitWindows 与 Timeline 的 WindowTag 完全匹配，并配置有效的 SourceTag、Profile 和 DamageEffect。检查时第一段 HitWindows 仍为空，默认剑已配置 WeaponStart / WeaponEnd 来源；本次不覆盖用户正在编辑的这些资产。

## 本次验证

- 按用户回复保存并关闭编辑器后执行 Editor、Game Win64 Development 常规构建，均退出 0。
- Hodge.Combat 五项现有自动化回归均 Success、无错误。
- 重新打开编辑器读取三个 CVar，均为 1；Duration 为 5.0，资产/地图无未保存修改。
- 在 PIE 请求正式攻击，并通过 URL Experience=Exp_MeleeValidation 试用既有 Body 验证配置。截图没有确认可见检测形状，因此不计作三类绘制视觉验收通过；不修改夹具或正式命中配置来扩大本次范围。保留截图与回执。
- 没有修改用户已有蓝图、武器 Socket、模型、检测 Profile 或战斗规则，没有提交。

命令与原始证据保存在 Saved/HitDetectionDebug：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combat' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HitDetectionDebug/CombatTests'
```

未执行 Cook/打包、独立进程/专服或三类实际攻击的完整视觉与伤害回归。已有 UI GameViewportClient 报错仍在，未扩大到 UI 修复。
