# 数据资产与 AssetManager

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 配置所有权

2026-10-06 属性首版：PawnData.StatProfile 引用角色成长配置，EquipmentDefinition.StatProfile 引用装备加成配置；正式角色用 DA_Pover_Stats，默认剑用 DA_Sword_Stats。曲线在 Main/Data/CharacterStats，角色基础为 Instant GE、装备为独立 Infinite GE。详见 [实现记录](../Design/character-attribute-growth.md)。

Experience 声明 GameFeature、Actions/ActionSets 与 DefaultPawnData。PawnData 提供 PawnClass、AbilitySets、ComboDefinition、DefaultWeaponDefinition、InputConfig、DefaultCameraMode、TagRelationshipMapping；字段均已存在，不能再说只有 PawnClass。

当前 Experience 指向 `/Game/Main/Data/PawnData/DA_Dafult_PawnData`，输入 DA_HodgeInputConfig、相机 CM_ThirdPerson、默认装备 BP_Equipment_Sword、连段 DA_LightCombo。AbilitySets 为 Main/Data/AbilitySet/DA_Pover 与 AS_LightCombo，关系映射为空。DefaultGame.ini 的 `/Game/Main/Data/DA_Dafult_PawnData` 只是回退入口。

## 技能与装备数据

Definition.ExecutionConfig 持有 Montage、PlayRate、Blend 与 Timeline；HitWindows 绑定命中窗口和配置，WeaponUseWindowTag 关联手持窗口。Timeline 单一 Events 数组，Kind=Window/Point，时间使用蒙太奇源时间；不是 GA、不是连段状态容器。

ComboDefinition 指向跳转 DataTable，输入缓存 0.3 秒、连段保留默认 1 秒；bAllowAfterExecutionEnded 控制动作结束后的续段许可。正式顺序 1→2→3→5→4，末段窗口跳回第一段。

EquipmentDefinition 指定实例、Actor 与 AbilitySets；WeaponPresentationProfile 保存模型/材质、HandSocket/BackSocket、偏移、曲线与时间。BackSocket 默认 WeaponOnBack，BackTransform 为插槽内偏移，当前单位变换。

## 加载与扫描

AssetManager 管 GameData 与资源入口，Experience 使用 Equipped Bundle 按端加载。当前未实现授予期 PreloadPrimaryAssetBundles / PreloadPrimaryAssetsOnGrant，不能沿用旧方案宣称已预加载所有动画。

扫描表有 Map、PrimaryAssetLabel、GameData、GameFeatureData、Experience、PawnData、ActionSet。Definition/Timeline/Combo 配置经已有硬引用链可达，当前不靠额外按名扫描；Cook 仍需单独确认地图与依赖收集。

GameData 位于 `/Game/Main/Data/DA_Dafult_GameData`；具体 GE 需核对源资产及测试。修改资产名要同步引用、配置、CoreRedirects，不能只改文档拼写。

源码入口：[PawnData](../../Source/Hodgepodge/Public/Data/HodgePawnData.h)、[Definition](../../Source/Hodgepodge/Public/Data/HodgeAbilityDefinition.h)、[ComboDefinition](../../Source/Hodgepodge/Public/Data/HodgeComboDefinition.h)、[Profile](../../Source/Hodgepodge/Public/Equipment/HodgeWeaponPresentationProfile.h)。
