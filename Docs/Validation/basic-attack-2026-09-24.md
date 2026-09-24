# Timeline 普通攻击验证（2026-09-24）

实现为数据配置型 `GA_BasicAttack` 蓝图，父类 `UHodgeGameplayAbility_BasicAttack` 执行可复用的 GAS Task 流程。蓝图 Class Defaults 的 `AttackSteps` 持有五组 Montage / Timeline；EventGraph 不重复实现原生流程。

## 资产与授予

- GA：`/Game/CodexText/BasicAttack/GA_BasicAttack`
- 五条 Timeline：`/Game/CodexText/BasicAttack/DA_Attack01_Timeline` 至 `DA_Attack05_Timeline`
- Montage：用户指定的 `/Game/CodexText/Montage/AM_Attack01_Montage` 至 `AM_Attack05_Montage`，没有修改原动画资产。
- 现有 `/Game/Main/Data/DA_Dafult_PawnData` → `AbilitySets` → `/Game/Main/Data/DA_Pover` → `GrantedGameplayAbilities[0]` 已替换为新 GA，输入仍为 `InputTag.Ability.Melee`。
- 旧能力集备份：`/Game/CodexText/Backups/BasicAttack_20260924/DA_Pover`。旧 `/Game/Main/Character/Hero/GA_Attack` 未修改。
- `IMC_Default` 的 `IA_Attack` 绑定鼠标左键，已有 Pressed Trigger；按住不会自动重复点击。GameFeature 负责添加 IMC。

## 行为

点击激活第一段，每段最多缓存一次后续点击。在 Timeline 的 `Status.Attack.Cancel.NextAttack` 窗口内转到下一段；第五段没有下一段窗口。无后续点击则当前段播放完结束。

每条 Timeline 总时长精确使用对应 Montage 的长度。后摇、接段和移动取消窗口从时长 60% 开始，到总时长结束，区间为左闭右开。五段开始时间约为 1.90、2.06、2.04、2.80、2.44 秒。窗口由资产管理，可以调整 `Events` 的 `StartTime`；最终手感仍需按动画动作微调。

同一时刻先处理已缓存的接段，再处理移动取消。原始移动意图始终记录；攻击中不提交普通行走输入。提前按住移动会等窗口打开才取消；窗口内按下移动立即取消，停止自身 Montage，清除窗口标签并重置连击。

`WaitMoveCancel` 仅在本地控制端运行；输入/标签事件立即检查，Tick 补充自定义模拟输入阈值检查。客户端通过 GAS EndAbility 同步取消，服务端不读取远端玩家的本地输入。换段和结束均移除委托、结束任务，输入等待提前结束时也解绑复制事件。

未添加命中检测、伤害、攻击 GameplayEffect、目标搜索或武器碰撞。

## 执行与结果

常规构建（编辑器关闭时执行 Editor 构建，未使用 Live Coding）：

```powershell
& D:/UE_5.5/Engine/Build/BatchFiles/Build.bat HodgepodgeEditor Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64
& D:/UE_5.5/Engine/Build/BatchFiles/Build.bat Hodgepodge Win64 Development -Project=D:/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64
```

两目标均退出 0；最后一轮增量构建分别用时 12.81 / 12.29 秒。原有 IncludeOrder、插件依赖声明及 HUD 弃用警告未扩大处理。

MCP `manage_blueprint.compile`：`GA_BasicAttack` 为 UpToDate，保存成功；重新打开编辑器后使用落盘资产测试。

通过 MCP 执行 `Tools/BasicAttack/verify_pie.py`，在 `/Game/ThirdPerson/Maps/ThirdPersonMap` 使用实际 PawnData 授予的能力，通过 Enhanced Input 注入攻击和移动，不临时授予测试 GA。

- 单人 PIE：12 项通过，覆盖单击自然结束、五段按序接续、前半段移动不取消、提前按住移动在窗口处取消、取消后从第一段重启。
- 双人 Listen Server 的远端客户端：36 项通过；额外核对服务端段数及正在播放的 Montage，以及结束后本地攻击标签清零。网络结束检查等待 0.2 秒传播。
- 双人 Listen Server 主机玩家：16 项通过，包含结束后的窗口标签清理。
- 详细采样位于 `Saved/Tests/basic_attack_pie.json`（首轮单人）和 `Saved/Tests/basic_attack_client.json`、`Saved/Tests/basic_attack_server.json`；这些是生成结果，不纳入源码。

初次资产生成将窗口末端序列化为六位小数，导致部分条目的 EndTime 略超 Duration，运行时正确拒绝。已改为足够精度并重新保存验证。测试脚本的集合序列化和同帧联机结束断言问题也已修正；不将这些失败轮次算作通过。

MCP 再次运行 `automation RunTests Hodge.Timeline`：CleanupAndOrdering、RejectBeforeSideEffects、Validation 全部成功（日志 UTC 08:25:42）。

测试后恢复 PIE_Standalone / 1 玩家 / RunUnderOneProcess=True；保留默认测试地图，未提交代码，未回退原有工作区改动。

## 手动体验

1. 打开 `/Game/ThirdPerson/Maps/ThirdPersonMap`，启动 PIE 并点击视口获取焦点。
2. 鼠标左键单击：播放第一段后结束。各段过程中再单击一次，后半段窗口打开时接下一段，最多五段。
3. 攻击前半段按住 WASD：不应立刻取消；到后半段窗口时停止攻击并恢复移动。
4. 停止移动再次单击：应从第一段重新开始。
5. 调手感时编辑对应 Timeline 的窗口时间，以及 GA Class Defaults 的 `PlayRate`、`CancelBlendOutTime`；不要把窗口 EndTime 写到 Duration 之后。

未执行 Cook/打包、独立进程客户端、专用服务器、人工网络延迟/丢包、重生/Pawn 切换和逐帧视觉质量验收。
