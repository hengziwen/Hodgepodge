# UI 基础闭环验证 · 2026-10-08

## 交付范围

保留现有 GameInstance／LocalPlayer／Controller 父类，使用已有 CommonUI／CommonInput／UMG／Slate，自有 UIManager 和四层根布局接通 Experience AddWidgets。未引入 CommonGame、CommonUser、ModularGameplayActors；没有新增角色常驻组件，没有提交代码。

正式 UI 共 9 个资产在 `/Game/Main/UI`，默认 Experience 已引用 EAS_HodgeGameplayUI。血条读取实际 HealthComponent，基础技能栏走原有输入 Tag；菜单阻断自己的玩法输入并清理缓存，菜单拥有的确认页面随菜单退出而撤销。

主要源码为 UI/Subsystem/HodgeUIManagerSubsystem、UI/Foundation/HodgePrimaryGameLayout、UI/HodgeGameplayWidgets、HodgeHUDLayout、HodgeGameViewportClient、UIExtensionPointWidget 和 GameFeatureAction_AddWidgets；生命周期接入现有 GameInstance，输入边界接入现有 Hero／Controller／ASC。制作与 PIE 调度工具仅在 HodgeAbilityEditor。

## 常规构建和原生测试

引擎 UE 5.5.4，Win64 Development。本次未修改引擎版本／Target 设置；既有 IncludeOrder 升级提示保留。Editor、Game 构建退出 0；最终日志 `Saved/UIFoundation/Editor-authoring-final.log`、`Game-modal-final.log`（最后一项改动仅为 Editor 资产制作保护）。单并行／NoUBA 仅是命令参数。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/UIFoundation/AutomationDelivered' '-abslog=E:/Project/Git/Hodgepodge/Saved/UIFoundation/AutomationDelivered.log'
```

最终报告 `AutomationDelivered/index.json`：20 项 Success，包括新增的 2 项 UI 生命周期／作用域测试；命令退出 0。原生测试不是蓝图／PIE／联机通过的替代证据。

## 资产与运行检查

正式 7 个 Blueprint 编译、9 个资产加载与数据校验通过，包含 EAS 的 Client Bundle 与默认 Experience ActionSets 内部引用。配置 InputData 属于 DefaultGame.ini；HodgeExperienceActionSet 扫描为原生 DataAsset 并包含 Main/UI，否则冷启动会遗漏 HUD Bundle。

单人 12 阶段覆盖：初值 150/150、真实伤害 25 后 125/150、等级 2 后 MaxHealth=170 并保持血量比例、Esc 菜单／Modal／返回、HUD 卸载再激活、根布局重建，以及未完成加载的取消与不存在资产的失败。异步失败的 Missing 资产日志是测试主动制造，不计作正式资产缺失。菜单类已加载时 PushAsync 同步完成，不能再取消已完成请求；取消验收使用冷启动。

两人 Listen 和专服世界＋两客户端 PIE 各 11 阶段覆盖：服务器施加伤害只改变目标玩家的血条；一个玩家开菜单时另一个的玩法输入保持允许；HUD 卸载／再激活与根布局重建没有重复 Entry。专服世界不创建 UIManager。PIE 使用同进程，不宣称独立 Server 可执行文件或独立进程通过。

100ms NetEmulation.PktLag 下，真实 EnhancedInput 驱动两轮五段普攻并继续第一段：拥有者和服务器 11 段顺序均为 `01→02→03→05→04→01→02→03→05→04→01`。取消后姿势租约／命中会话归零；客户端提前移动、进入后摇窗口后由服务器授权取消也通过。

生产菜单确认回调、Esc 从 Modal 返回 Menu 再返回 Game，以及打开确认框后卸载 HUD／重新注入均通过。

菜单打开时持续注入攻击与移动，实际能力未激活、位移／移动意图未生效；关闭后没有旧输入重放。血条经历真实死亡归零、Pawn 销毁未就绪、服务器 RestartPlayer 后重绑新 Pawn，再次伤害能正确刷新（客户端等待复制数值稳定后检查）。这里只验证 UI 重绑；自动重生玩法不在本轮实现范围。

CommonUI 日志记录 Menu／Modal 的目标按钮焦点与返回游戏视口。Slate 根截图包含真实 HUD，和仅渲染 3D 的 GameViewport 截图区分；截图放 `Saved/UIFoundation/UI-HUD-final.png`。

## 本轮发现并处理

- 冷启动缺 HUD：修复 HodgeExperienceActionSet 扫描类型／目录，补 Client Bundle。
- 客户端 Esc 不匹配：HUD Escape 绑定改为 Game 输入模式；UE5.5 的绑定 All 不代表匹配所有活动模式。测试调用期间单独绕过编辑器 StopPlaySession，未改用户快捷键。
- PIE 退出时 GameInstance 已解绑：清理记录保存弱 Manager 引用并检查 GI，避免在销毁链查找失效 Subsystem。
- 制作工具限定编辑模式：运行 PIE 时拒绝编译／保存正式 UI 和 Experience；此保护已在 PIE 调用中验证拒绝且不改资产；资产检查与玩法回归分开执行，重编译后重新读取 CDO 并确认 ActionSet 引用。
- 菜单确认框遗留：菜单记录自己创建的确认框，在停用／销毁时精确撤销，避免卸载 HUD 后仍被 Modal 阻断。

## 独立问题与未验证范围

死亡／重生测试观察到额外 +50 的属性过渡值：单人早期等级 2 从 170 到 220；纯客户端重绑先显示 200，随后回到 150。客户端数值稳定 2 秒后再施加伤害 25，血条正确变为 125/150。UI 读取的是实际属性；本轮没有修改属性／装备计算，尚未定位完整时序，不能把 UI 重绑通过说成重生属性流程通过。后续应专项检查旧 Pawn 装备属性 GE 撤销与新生命初始化。

未执行 Cook／打包、独立进程、独立 Server Target、分屏、实体手柄热插拔、完整设置／登录／匹配或美术完成度验收。未测试有实际冷却 GE 的技能图标展示；基础栏实现查询真实 GAS 冷却，当前普攻／跳跃没有演示冷却倒计时。

## 证据与恢复

PIE 脚本在 [Tools/UIFoundation](../../Tools/UIFoundation/README.md)，具体结果位于 Saved/UIFoundation；JSON `error=null` 才算通过。100ms 战斗脚本复用 Tools/AnimNotifyCombat。MCP 执行命令示例：

```powershell
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Tools/UIFoundation/test_single.py ../../Saved/UIFoundation/single-run-final.json
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Tools/UIFoundation/test_network.py ../../Saved/UIFoundation/network-run-final.json
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Tools/UIFoundation/test_dedicated.py ../../Saved/UIFoundation/dedicated-run-final.json
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Tools/UIFoundation/test_owned_modal.py ../../Saved/UIFoundation/modal-run-final.json
```

相关修改前备份 `Saved/UIFoundation/Before`，96 个文件共 676367 字节，manifest 记录 SHA256；没有复制全项目。已有通知迁移、连段修复及用户资产改动均保留，没有暂存、提交或回退。配置方法统一引用 [UI 配置指南](../Guides/ui-foundation-configuration.md)。
