# 角色与默认剑属性配置

2026-10-06 首版实现；四个正式配置已迁入 `/Game/Main/Data/CharacterStats`。当前数值是验证成长与来源分离的示例，不代表最终平衡。

- DA_Pover_Stats：角色成长 Profile。MaxHealth/BaseDamage 的 ScalableFloat 系数为 1，引用 CT_Pover_Growth 对应行；合法等级 1～90。
- CT_Pover_Growth：角色基础曲线。一级 MaxHealth=100、BaseDamage=20；二级 120/22。
- DA_Sword_Stats：武器属性 Profile，引用 CT_Sword_Growth；合法等级 1～90。
- CT_Sword_Growth：武器独立加成。一级 MaxHealthBonus=50、BaseDamageBonus=10；二级 60/12。
- 测试专用 `/Game/CodexText/CharacterStats/DA_Test_Stats` 继续留在 CodexText，未列入正式成长配置。

角色 Profile 的 InitializationEffect 默认指向原生 HodgeCharacterBaseStatEffect（Instant、Override）；装备 Profile 的 AttributeEffect 默认指向 HodgeEquipmentStatEffect（Infinite、Additive、无周期、无堆叠）。参数为 SetByCaller.Stat.MaxHealth 和 SetByCaller.Stat.BaseDamage，不需要为每个等级创建一个 GE。

正式 PawnData `/Game/Main/Data/PawnData/DA_Dafult_PawnData` 的 StatProfile 引用 DA_Pover_Stats。旧 Main/Data 根目录同名路径当前通过重定向解析到这份实际资产。BP_Equipment_Sword.StatProfile 引用 DA_Sword_Stats；Fixture 下 Body/Weapon/HitBox 三份 PawnData 使用 DA_Test_Stats。

修改成长数值：打开相应 CurveTable，按等级横轴调整曲线；Profile 的 Value 是系数，不要把已经含装备的最终数值填到角色基础曲线。修改后保存，重开 PIE 复核。ConfigurationVersion 用于明确配置刷新版本。

运行时入口（需要服务器权限）：

- PlayerState.SetCharacterLevel(Level)：角色升级，保持血量比例，不重授 AbilitySet。
- EquipmentManager.SetEquipmentLevel(Instance, Level)：装备升级，刷新该来源效果，保留绝对 Health。
- PlayerState.InitializeCharacterProgression(CharacterId, Level, SavedHealth)：首次 Avatar 初始化之前填入档案输入；SavedHealth=-1 表示无保存资源。
- PlayerState.RestoreCharacterHealth(Health)：提交合法保存资源并夹取上限，不用于复活或普通治疗；死亡状态恢复仍需生命/死亡系统配合。

这些 API 不是客户端任意升级 RPC。客户端 UI 或 GA 发起请求时，应经过相应的服务器成长/装备业务验证。完整经验、突破、库存、存档后端与角色切换不在本版范围。

角色绑定不自动满血，默认装备属性完成后才提交出生资源。明确 GameMode.RestartPlayer 重生可补满；换 Pawn 重绑定不当成免费回血。战斗依然使用 GAS 聚合后的 BaseDamage，源属性名称不改。

详见 [实现与验证文档](../../../../Docs/Design/character-attribute-growth.md)。
