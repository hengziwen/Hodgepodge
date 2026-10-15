# Wing 通知蒙太奇改造与验证

日期：2026-10-14；执行跨 10 月 13～14 日；UE 5.5.4。对应 [设计 v1.3](../Design/attack-wing-presentation.md)和 [配置手册](../Guides/wing-notify-montage-configuration.md)。逐项结果和文件 SHA256 见 [结果 JSON](wing-notify-montage-2026-10-14-results.json)。

## 1. 实际改动

WingAction 通知状态直接保存 WingMontage、PlayRate、StartPosition、FadeOutSeconds，通知条是显示窗口唯一配置入口。Profile 不再要求或执行角色动画映射；旧映射计算移除，旧字段保留序列化兼容。旧程序化时序与姿势在新模式的 Details 面板隐藏。

WingComponent 播放独立翼 AnimInstance 的真实蒙太奇，由角色源时间校正位置，保持暂停、变速、晚加入与跳段同步。通知回调只唤醒统一刷新，避免角色骨骼刷新中重入。正常结束和取消冻结最后姿势淡出；死亡、Avatar 更换和销毁立即清理。缺少匹配骨架、DefaultSlot、蒙太奇或非法参数时明确失败。

新增 ABP_PoverWing_Anzhu 的 Reference Pose → DefaultSlot → Output，新增 AM_Wing_Attack04、AM_Wing_Attack05，分别使用已导入的 B_Attack04、B_Attack05。迁移现有角色 AM_Attack04_Montage、AM_Attack05_Montage 和 WingText 的 AM_Attack04 预览通知；用户当前通知时间、轨道、挂接与 Box 尺寸保持。正式第四号动作原起点为 0，预览原起点为 0.05，迁移起播位置分别保留这些对齐值。

旧 Experience 的程序化条目继续禁用，不新增 Wing GA、表现 RPC 或 Runtime 模块。现有 Definition 执行结束和 WaitHitResults 权威采样入口继续复用。Box 校准改为读取通知蒙太奇，也支持用户当前只有 WingMesh 命中通知的配置，不补回已移除的手部通知。

本机 Lyra Source/LyraGame 与 GameFeature 源码没有独立 Wing 蒙太奇实现；参考 AnimNotify_LyraContextEffects 的通知转交方式，使用 UE 原生 NotifyState、AnimInstance/Montage 和本项目现有生命周期。

## 2. 构建与实际命令

以下最终常规构建退出码均为 0，编辑器在无 PIE、无未保存地图/资产时正常关闭；没有使用 Live Coding。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Wing' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/WingMontage/Automation-final' '-abslog=E:/Project/Git/Hodgepodge/Saved/WingMontage/Automation-final.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/configure_montages.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingAttack/validate_montages.py
$env:WING_SCENARIOS='presentation,damage'
& E:/Python/python.exe Tools/WingAttack/run_montage_batch.py single listen dedicated
& E:/Python/python.exe Tools/WingAttack/export_montage_results.py
```

网关凭据由本机工具私有读取，不写入日志或源码。最终构建和测试日志、备份、逐帧数据位于 Saved/WingMontage；该目录不入库。

## 3. 最终结果

- 原生自动化 **8/8 通过**：NotifyMontage、PresentationSession、AttackPresentationIntegration、FadeState、BoxHitGeometry，以及旧程序化 Envelope、CancellationAndClock、AssetContract 回归。7 项无警告，1 项成功但含已有攻击分支点重叠警告。
- 资产 **16/16 通过**：实际通知资源、独立骨架、时间/轨道保留、挂接、材质与权威采样。
- 3 个蓝图为 BS_UpToDate、hasCompileErrors=false：新翼 AnimBP、既有翼组件 BP、WingText；检查在 PIE 停止后执行。
- 表现真实输入：单人 **16/16**、Listen Server **21/21**、PIE Dedicated **24/24**。验证两份通知蒙太奇、实际翼骨运动、时钟、MID 淡出、正常退出、Dash 取消及空闲不求值。
- Box 伤害专项：单人 **7/7**、Listen Server **8/8**、PIE Dedicated **10/10**。验证翼盒单独扣血、每目标本刀一次、范围外无伤害、服务器技能取消后无伤害、隐藏专服网格和客户端生命复制。
- 合计 **86 项 PIE 断言通过**。这是断言/需求追溯，不是代码行覆盖率百分比。
- 19 个 WingAttack Python 文件语法检查通过；git diff --check 通过。

Box 专项使用运行时缩小的敌人胶囊隔离翼范围，不改变生产怪物资产。其取消项通过原生游戏 Tick 队列请求服务器取消真实攻击 GA；真实 Dash 输入取消由表现专项单独验证，二者不混为一项验收。

## 4. 调试发现与处理

- 预览 API 返回 UAnimationAsset，初版缺少显式类型转换导致两次编译失败；已改为校验并转换 UAnimSequenceBase，最终双目标构建通过。
- 旧映射字段隐藏后不能由 Python 修改；移除了工具对此受保护字段的写入，字段只留兼容、运行时不读取。首次失败创建的本工具资产经重试保存，未保存其他包。
- 旧测试高频输入在客户端不断替换连段请求，无法进入翼动作；改为每段在实际许可窗口点按一次，最终三模式通过，不改产品连段逻辑。
- 用户当前已删除手部命中通知，旧 Box 脚本假设手部存在导致 StopIteration；测试和校准支持单独翼来源，保留用户资产。
- 隔离 Box 场景最初发 Dash 输入没有结束攻击，该失败未计为通过；其 Dash 激活失败原因未单独定位。Box 清理改由服务器原生取消队列隔离验证；另外的真实 Dash 表现专项在三模式均通过。这不等同于已经修复所有 Dash/碰撞组合场景。
- 原角色攻击蒙太奇存在分支点重叠警告；本次持续窗口查询补偿显隐通知漏过，未更改用户既有伤害/恢复时间，不宣称这些资产警告消失。

## 5. 恢复与限制

已恢复原地图 /Game/ThirdPerson/Maps/ThirdPersonMap 及记录的性能选项，PIE 为 0，无未保存地图或资产；打开 AM_Attack04_Montage 作为制作入口。原完整 PIE 设置未记录，恢复单人基线，未宣称完全还原所有选项。没有自动提交或推送，保留既有 Dash、Resonance Skill、开发规则等未提交工作。

未执行独立专服进程、真实多机、Cook/打包和物理鼠标手感评审。翼蒙太奇当前驱动表现与姿势，伤害及技能事件继续配置在角色蒙太奇；不消费翼蒙太奇自身通知作为玩法入口。新蒙太奇默认零动画混入/混出，材质承担淡出；非零动画混入需要合适基础姿势，否则导入动画根位置与参考姿势差异仍可能产生视觉位移。
