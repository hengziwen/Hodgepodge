# 常见问题

[返回首页](README.md)

## 为什么有很多 Lyra 类，却还不能玩？

类提供能力，但必须有创建、挂载、数据配置和调用。当前 HeroComponent 停用，PawnData 注入和技能输入消费等关键入口缺失，因此存在完整函数体却无法从游戏入口到达。

## 是不是解开 GameMode 的 SetPawnData 就好了？

不是。那只解除 PawnExtension 的一个前置条件。还需正确 PawnClass、有效 Hero 协调、ASC 接入、输入映射绑定、每帧消费和相机模式。状态链末端目前也没有完整业务校验。

## PawnData 现在只有 PawnClass 吗？

不是。2026-09-10 工作区已启用五字段：PawnClass、AbilitySets、TagRelationshipMapping、InputConfig、DefaultCameraMode。旧 README 的描述是旧状态。字段已启用仍不等于资产值已填写或消费者已接通。

## HeroComponent 到底有没有？

有文件和较长草稿，没有有效编译类：头文件和实现整份是行注释。类重定向也不能代替 UCLASS 编译注册。

## 为什么 PlayerState ASC 有效，Character ASC 却为空？

Character 查询走 PawnExtension 保存的指针；Hero 旧初始化路径直接操作 PlayerState ASC，没有通过 PawnExtension 保存关联。

## 为什么配置了 InputTag 还不能触发？

还需要活动 IMC、正确组件类型、Action 绑定、已授予的 Spec、精确相同的动态 InputTag 和 ProcessAbilityInput 调用。完成这些后才检查技能激活条件。

## GameFeature_AddAbilities 能不能替代 PlayerState 的授予？

它可作为能力来源，但要明确目标 ASC 和生命周期。永久基础技能、玩法临时技能和装备技能的回收策略不同。不能给 Pawn 再创建一套 ASC 来回避原有玩家 ASC 接入问题。

## 有 HealthSet 就能自动死亡吗？

不能。属性复制与夹取有实现，但 GE 结算主体、服务器耗尽处理和角色 HealthComponent/死亡绑定仍不完整。

## 有 ABP_Pover_Base 就代表动画接好了吗？

文件存在只证明资源在磁盘。还要在角色蓝图确认 Mesh、Skeleton、AnimClass，并运行检查变量、状态机、Slot 和动画层。

## 是否应该现在删除 ALS 资源、接背包或做大地图？

建议先完成最小玩家和战斗闭环。删除资源需要确认引用；背包、开放世界和后端属于后续系统，不是当前输入断链的解决办法。

## 本知识库有没有联网搜来的结论？

没有。它以本地代码、配置和现有文档为依据。涉及未验证的引擎安装、资产内部值、运行和打包行为均明确留作验证项。

## 怎样查一个类或 Tag？

打开 Reference 索引，或运行 `python Docs/KnowledgeBase/tools/kb.py search "类名或标签"`。索引记录有效定义候选和源文件行号；精确行为仍要打开源码查看。
