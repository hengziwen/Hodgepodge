# 武器材质恢复

> 2026-10-06 状态同步：原模型/材质修复记录保留；正式剑的显隐使用独立 WeaponPresentation 材质副本，不修改原材质。 当前项目事实见 [本轮更新](../../Docs/KnowledgeBase/26-update-2026-10-06.md)。

2026-09-29，通过 UE MCP 执行 `restore_weapon_materials.py`。

两个 SkeletalMesh 原先都绑定 WorldGridMaterial：
- `/Game/Wuwa/Weapon/Sword_Qiuyuan` → `Materials/MI_R5Sword506Md20001Effect`
- `/Game/Wuwa/Weapon/Sword_Qiuyuan1` → `Materials/MI_R5Sword506Md20001`

父材质为 `Materials/M_Weapon_Restored`。MainTex 使用 JSON 指定的 T_R5Sword506Md20001_D；法线从打包贴图的线性 RG 重建 Z。单独复制为 T_Weapon_PackedLinear，保留源贴图及其导入设置。未确认原自定义 shader 的 B/A 表面参数及 TypeMask 语义，因此不盲接；Roughness=0.45、Metallic=0.65、Specular=0.35 为可调近似值。

原 MSM_ToonCommon、MatCap、流动效果缺少着色器/贴图依赖，未复刻；Effect 实例当前恢复基础材质，不包含原动态特效。其他 mask 文件保留。

已重新编译材质，获得有效 shader statistics；两个网格体槽位引用、保存成功和编辑器预览截图均已核对。未执行 PIE 或 Cook。原网格体磁盘副本在 `Saved/Backups/WeaponMaterials_20260929`，不入库。用户原有动画删除改动未处理。重跑脚本会重建本脚本生成的父材质节点，请先保存自行调整。
