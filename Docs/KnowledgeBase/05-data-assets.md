# 数据资产与 AssetManager

[返回首页](README.md)

## 三层配置的区别

GameData 是全局通用资源入口；ExperienceDefinition 是一套玩法的组合；PawnData 是某类 Pawn 的配置。AbilitySet 和 InputConfig 是可复用的数据块。不要把所有东西塞进 GameData，也不要每个角色复制整份 Experience。

## PawnData 当前字段

当前工作区已启用以下五个 UPROPERTY，旧 README 的“仅一个字段”已过时：

- PawnClass：生成类。建议指向 AHodgeHeroCharacter 派生蓝图；构造默认空。
- AbilitySets：配置要授予的能力集合。字段已存在，但 PlayerState 的授予循环仍注释。
- TagRelationshipMapping：能力阻断、取消和附加激活条件关系；组件应用入口仍注释。
- InputConfig：InputAction 与 GameplayTag 的映射，不等价于 IMC；构造默认空。
- DefaultCameraMode：默认相机模式类；构造默认空，实际消费依赖 HeroComponent。

UPROPERTY 表示编辑器能保存此字段，不表示现有 DA_Dafult_PawnData 已填写。二进制数据内容必须在编辑器查看。

## ExperienceDefinition

GameFeaturesToEnable 保存插件名；DefaultPawnData 为硬引用；Actions 为 Instanced Action 对象数组；ActionSets 组合复用动作集合。当前 AssetManager 扫描配置将 ExperienceDefinition 作为蓝图类扫描，加载代码取对应类默认对象。

新增 Experience 时要保持资产类型、目录扫描和加载代码预期一致。不要把同名普通数据实例放进目录后就假设一定会被作为 Experience 类解析。

## AbilitySet

集合有 GrantedAttributes、GrantedGameplayAbilities 和 GrantedGameplayEffects。每个技能项包含 Ability 类、等级、InputTag；授予时 InputTag 写入 Spec 的动态源标签。ASC 输入匹配使用 HasTagExact，父标签相同不等于精确相同。

GiveToAbilitySystem 在权威端执行，顺序为属性集、技能、效果。可传 GrantedHandles 记录创建结果，便于 TakeFromAbilitySystem 撤销。永久玩家能力与临时装备能力应采用不同的句柄所有权策略，避免重生重复授予或卸装误删永久能力。

PlayerState 构造函数已经创建 HealthSet。若 AbilitySet 再添加同类型 HealthSet，需要明确是否造成重复属性实例；不要在配置里无意识叠加。

## GameData

目前字段是 DamageGameplayEffect_SetByCaller、HealGameplayEffect_SetByCaller、DynamicTagGameplayEffect，均为软类引用。路径存在不代表引用已填，更不代表对应 GE 的 Modifier、SetByCaller 标签正确。

DynamicTag GE 由 ASC 的动态标签辅助方法读取；伤害和治疗的实际结算还依赖技能和 HealthSet 管线。当前不能仅配置 Damage GE 就宣布伤害系统完成。

## AssetManager 配置

DefaultGame.ini 指向 `/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData` 与 `/Game/Main/Data/DA_Dafult_PawnData.DA_Dafult_PawnData`，拼写 Dafult 是现有资源名，不应只改配置的一边。

PrimaryAssetTypesToScan 包含 GameData、PawnData、ExperienceDefinition、ExperienceActionSet、GameFeatureData 及地图/标签。检查三件事：目录或 SpecificAssets 对不对；AssetBaseClass 是否正确；bHasBlueprintClasses 是否符合资源形态。

Cook 规则与 PIE 是两回事。PIE 能加载，不说明打包一定包含资源。Map 扫描目录 `/Game/Maps` 与默认 ThirdPerson 地图位置不同，应在打包验证中明确地图收集来源，不能仅看扫描项判断成功或必然失败。

## 资产编辑检查单

打开 Experience 查看 DefaultPawnData；打开 PawnData 查看五个字段；打开 Hero 蓝图查看父类、Mesh、AnimClass 和组件；打开 InputConfig 与 IMC 核对两套映射；打开 AbilitySet 核对技能输入 Tag；最后运行观察实际类型和引用。

源码：[PawnData](../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[ExperienceDefinition](../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[AbilitySet](../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)、[AssetManager](../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)。
