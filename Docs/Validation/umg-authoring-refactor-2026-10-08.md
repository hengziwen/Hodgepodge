# UMG 可视化制作重构验证 · 2026-10-08

## 本轮交付

按用户批准的设计实施。血条、技能栏、菜单、确认框和 HUD 的布局已保存到正式 WBP 的 WidgetTree；按钮、显示更新、定时器、焦点与确认结果在 Graph 中实现。八个界面 WBP 使用引擎或通用框架父类，六个业务原生控件类型移到 Archive/UI/NativePrototype，不参与 UBT/UHT。

一个 HodgeGameplayUIDataSource 按 LocalPlayer 共享，负责当前 Pawn/ASC、属性就绪、生命事件、技能状态及 InputTag 提交。UIManager 持有它，更换 Controller/移除玩家使旧源失效；现有 PlayerState 增加就绪通知，不引用 WBP。没有新增角色常驻组件，没有引入 CommonGame/CommonUser/ModularGameplayActors。

Root/HUD/血条/技能栏/菜单/确认框保持原路径，新增 WBP_AbilitySlot 和 WBP_UIButton，Main/UI 共 11 个正式资产。默认 Experience 和 InputData 入口保持原路径。

## 构建与原生测试

UE 5.5.4，Win64 Development。Editor 和 Game 常规构建通过，关闭编辑器后执行；最新日志在 Saved/UMGAuthoringRefactor/Editor-lifecycle-final.log、Game-lifecycle-final.log。既有 IncludeOrder 提示和 CodexText/HodgeSurvivorHUD 的弃用警告保留，未升级引擎或构建设置。

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

新增/移走 C++ 文件时使用过 -NoUBTMakefiles 以刷新 UBT 源文件清单，参数未写入项目设置。21 项原生测试通过，新增 DataSourceInactive 检查旧引用/无有效玩家不能提交输入；最终 AutomationFinal/index.json 的 21 项均为 Success，命令退出 0。

## Designer 与资产

八个界面实际检查均有控件树：根布局 5、HUD 4、血条 5、技能栏 2、技能项 3、按钮 2、菜单 8、确认框 7 个控件。血条、技能栏、技能项、按钮、菜单和确认框均有保存的 Graph，所有 Blueprint 编译通过，11 个资产校验无错误。

已打开 WBP_PlayerVitals 的实际 Designer，观察到控件树和 HP 75/100 的设计期显示。内容组件设置 Desired 预览尺寸，框架根布局/HUD仍按屏幕预览。HUD 显示扩展点占位，子内容在对应 WBP 编辑。截图位于 Saved/Screenshots/HodgeUMG-Designer.png。

运行时 Slate 截图显示 HP 150/150 与普攻/跳跃栏，实际显示不依靠已归档的业务 C++ ConstructWidget。MigrateDesignerAssets 复跑返回 alreadyMigrated，未增加脏包、不重建现有控件树和 Graph。

## PIE 与网络检查

- 单人 12 阶段：真实出生属性、伤害、MaxHealth/等级变化、屏幕文字与进度条、Esc/Modal、HUD卸载再注入、根布局重建、异步取消/失败。
- 生产菜单按实际按钮 HandleButtonClicked 进入 Blueprint 绑定，覆盖打开确认框、接受/取消、Esc返回、HUD卸载与再激活、控件池复用。
- Listen Server 与两客户端 PIE：各自属性与 UI 输入隔离，HUD再注入和根布局重建；专服世界不创建UIManager。
- 旧数据源引用在重建后返回未就绪且 SubmitInput=false，新源正确就绪；不会继续操作旧玩家。
- 100ms Packet Lag 下正式连段 11 次，两端顺序一致并清理命中会话/姿势租约；客户端移动取消回归通过。
- 死亡归零、Pawn销毁未就绪、新Pawn重绑与稳定后再次扣血，界面正确刷新。

最终结果以 Saved/UMGAuthoringRefactor 的通过 JSON 和 acceptance-summary 为准，不能将工具调用成功替代测试断言。

## 本轮清理时机修正

默认 0.4 秒容器切换在后台 PIE 窗口中可能延迟完成，使移除/HUD Destruct延后。正式四个根容器改为 TransitionDuration=0；页面样式和自身 UMG 动画仍可编辑。通用 HUD 在 NativeOnDeactivated 立即撤掉自己拥有的菜单，不等 Slate 析构。

确认框的 Finish 是 Blueprint 函数，保留独立参数栈，先标记完成；页面仍激活时才主动 Pop，已因 Back/外部移除而停用时只完成一次结果，避免在容器退出回调内重复移除。菜单保存、解绑并关闭自己的确认框，不清空其他页面的 Modal。

## 边界与恢复

此前普攻首次偶发拒绝及拒绝恢复缺口仅保留调查记录，本轮未修改 ASC/Combat 网络授权逻辑。重生属性曾出现 +50 过渡值，本轮UI只读取就绪后的实际值，未修复完整装备/重生时序。

未执行 Cook/打包、独立进程、独立 Server Target、实体手柄热插拔或美术完成度验收。当前基础普攻/跳跃未配置冷却 GE，本轮不宣称完整冷却技能的视觉验收。

相关文件/资产备份为 Saved/UMGAuthoringRefactor/Before，107 个文件共 607225 字节，manifest含SHA256；没有复制整个项目。旧业务原型两份源文件归档在 Archive/UI/NativePrototype。回滚需恢复对应代码和WBP父类并常规构建；不能只恢复一个旧 uasset。

本轮未暂存、提交或覆盖用户已有改动。[制作与配置说明](../Guides/ui-foundation-configuration.md) 已更新，实际操作入口为 Main/UI 内的 Designer。
