# 数据资产与 AssetManager

> 最近源码核对：2026-09-19。源码接入状态与运行验收分开记录。
[返回首页](README.md)

## 三层配置的区别

GameData 是全局通用资源入口；ExperienceDefinition 是一套玩法的组合；PawnData 是某类 Pawn 的配置。AbilitySet 和 InputConfig 是可复用的数据块。不要把所有东西塞进 GameData，也不要每个角色复制整份 Experience。

## PawnData 当前字段

当前工作区已启用以下五个 UPROPERTY，早期文档的“仅一个字段”已过时：

- PawnClass：生成类。建议指向 AHodgeHeroCharacter 派生蓝图；构造默认空。
- AbilitySets：配置要授予的能力集合。PlayerState::SetPawnData 现已在权威端遍历并授予（未记录 GrantedHandles）。
- TagRelationshipMapping：能力阻断、取消和附加激活条件关系；组件初始化时已调用 ASC::SetTagRelationshipMapping。
- InputConfig：InputAction 与 GameplayTag 的映射，不等价于 IMC；构造默认空。
- DefaultCameraMode：默认相机模式类；构造默认空，已由有效 HeroComponent::DetermineCameraMode 消费。

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

DynamicTag GE 由 ASC 的动态标签辅助方法读取；HealthSet 已实现 Damage/Healing 元属性到 Health 的转换与耗尽广播；完整攻击仍需要技能授予、服务器命中、目标 ASC 和死亡衔接。

## AssetManager 配置

DefaultGame.ini 指向 `/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData` 与 `/Game/Main/Data/DA_Dafult_PawnData.DA_Dafult_PawnData`，拼写 Dafult 是现有资源名，不应只改配置的一边。

PrimaryAssetTypesToScan 包含 GameData、PawnData、ExperienceDefinition、ExperienceActionSet、GameFeatureData 及地图/标签。检查三件事：目录或 SpecificAssets 对不对；AssetBaseClass 是否正确；bHasBlueprintClasses 是否符合资源形态。

Cook 规则与 PIE 是两回事。PIE 能加载，不说明打包一定包含资源。Map 扫描目录 `/Game/Maps` 与默认 ThirdPerson 地图位置不同，应在打包验证中明确地图收集来源，不能仅看扫描项判断成功或必然失败。

### 注册原则：只登记需要"点名加载"的资产

硬引用链 ExperienceDefinition → PawnData → AbilitySets / InputConfig 在烘焙时会作为依赖一起被 cook，所以它们不需要在扫描表登记；登记反而会给它们生成不必要的 PrimaryAssetId，并影响 Chunk 划分。

一句话记法：会被「按类型 / 按 ID」点名加载的，必须注册；只被别的资产引用带出来的，不注册。

### ~~新增：AbilityTimeline 与 ComboSet 的按名登记~~（⚠️ 未落地）

> **本节仍未生效。** `Config/DefaultGame.ini` **没有** `HodgeAbilityTimeline` / `HodgeComboSet` 两个 PrimaryAssetTypesToScan（仍是 7 项：Map、PrimaryAssetLabel、HodgeGameData、GameFeatureData、HodgeExperienceDefinition、HodgePawnData、HodgeExperienceActionSet）。时间轴的测试件 `DA_TimelineTest` / `GA_TimelineTest` 走实验区直接引用，不依赖按名加载。

~~`Config/DefaultGame.ini` 已把 `HodgeAbilityTimeline` 与 `HodgeComboSet` 加入 PrimaryAssetTypesToScan（目录 `/Game/Main`）。它们会被"点名预加载"，符合上面的注册原则。~~

> 具体缺失的是 `UHodgeAssetManager::PreloadPrimaryAssetBundles`、`UHodgeGameplayAbility::PreloadPrimaryAssetsOnGrant` 以及 Timeline / ComboSet 的 Bundle 收集（源码全量搜索 0 命中）。⚠️ **注意**：`UHodgeAbilityTimeline` 类本身已随 HEAD `77b7dba` 存在（见 [GAS 章节](07-gas.md)），但它**没有 Montage 字段、也不做 Bundle 收集**——所以"按名登记 + 预加载"这套设计仍然完全没有落地。**`FHodgeBundles::Equipped` 本身仍存在**，并且被 `UHodgeExperienceManagerComponent::StartExperienceLoad()` 用于按端加载 Experience Bundle——这一点不受影响。
>
> 上面的"注册原则"本身仍然成立，但本节引用的预加载实现从未落地。历史依据见 [2026-09-17 记录](21-update-2026-09-17.md)。

## 资产编辑检查单

打开 Experience 查看 DefaultPawnData；打开 PawnData 查看五个字段；打开 Hero 蓝图查看父类、Mesh、AnimClass 和组件；打开 InputConfig 与 IMC 核对两套映射；打开 AbilitySet 核对技能输入 Tag；最后运行观察实际类型和引用。

源码：[PawnData](../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[ExperienceDefinition](../../Source/Hodgepodge/Public/Data/HodgeExperienceDefinition.h)、[AbilitySet](../../Source/Hodgepodge/Private/Data/HodgeAbilitySet.cpp)、[AssetManager](../../Source/Hodgepodge/Private/Data/HodgeAssetManager.cpp)。
