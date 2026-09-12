# 网络、复制与 Dedicated Server

[返回首页](README.md)

## 当前网络基础

PlayerState 创建可复制的 Mixed ASC，并设置较高 NetUpdateFrequency。PawnExtension 复制 PawnData；ExperienceManager 复制 CurrentExperience；HealthSet 为不同属性选择复制条件。CombatCharacter 有加速度压缩和共享移动结构。

这些是网络基础设施，不是双客户端验证报告。本轮未启动服务器或客户端，不能标记移动、技能、伤害、死亡已联机通过。

## Authority、Owner、Avatar

Authority 决定最终游戏状态的权威执行端；OwnerActor 是 GAS 所属对象；AvatarActor 是执行技能的当前身体；网络拥有者还决定客户端 RPC 等关系。这几个词不能混用。

玩家设置 PawnData 和 AbilitySet 授予已有权威检查。输入绑定属于本地玩家，不应在 Dedicated Server 上索取 LocalPlayer。模拟代理可以需要相机/动画数据，但不应安装本地输入。

## 初始化时序

服务器可先拥有 PlayerState 后生成 Pawn；客户端可能先看到 Pawn，再收到 PlayerState、PawnData 或 Experience。可靠做法是各通知触发幂等状态检查，不是延迟固定秒数后假设所有对象已到齐。

Hero 当前有 OnRep_PlayerState 初始化 ASC 的旧路径。迁移到 Init State 后，复制通知仍有价值，但其职责应改为唤醒协调组件而不是绕过统一入口。

## Dedicated Server 当前阻塞

GameMode::TryDedicatedServerLogin 在专服且当前地图匹配默认地图时返回 true。外部因此提前返回，然而用户登录与 HostMatch 回调被注释，没有继续分配 Experience 的有效路径。

本地验证阶段建议明确选择：未接在线会话时直接进入默认/指定 Experience；若确实启用异步登录，则保证成功和失败都有后续路径。不能只返回“已接管”却不完成处理。

仓库只有 Game 和 Editor Target 文件。独立 Server 构建目标尚需补齐，并验证当前引擎安装是否支持目标平台和 Server 构建。不要把 Editor 的 Dedicated Server PIE 模式与打包的 Server.exe 混为一谈。

## 复制优化不是第一优先级

FastSharedReplication 代码存在，但调度与启用链仍需确认。应先在普通 CharacterMovement 复制下验证位置和动画同步，再决定是否引入优化。否则出现代理不动时难以区分基本复制还是定制路径的问题。

## 重生与销毁

重生前解除旧 Pawn 对 ASC 的关联，清掉输入缓存、临时能力相机覆盖和不应保留的状态；重生后验证 ASC Owner 不变而 Avatar 更新。Health、死亡 Tag、临时 GE 是否恢复由业务规则确定。

CharacterBase Receiver 退出不对称和 Experience 部分加载清理都是生命周期风险，应在多次进入/退出 PIE 与换 Pawn 测试中覆盖。

## 需要的运行记录

每条关键日志至少包含 World/NetMode、Actor 名、LocalRole、是否本地控制、ASC Owner/Avatar、PawnData 名和当前 Init State。仅打印“初始化成功”无法区分服务器、拥有客户端和模拟代理。

源码：[网络相关符号索引](Reference/source-index.md)、[GameMode](../../Source/Hodgepodge/Private/Core/GameMode/HodgeGameModeBase.cpp)、[PawnExtension](../../Source/Hodgepodge/Private/Component/HodgePawnExtensionComponent.cpp)。
