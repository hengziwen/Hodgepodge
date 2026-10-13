# PoverEquipment 装备导入与贴图修复记录

日期：2026-10-11；引擎：UE 5.5.4。

## 结果

- 目标目录：`E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment`。
- 源目录：`D:/下载/EdgeDownloads/漂子装备_by_微波叮桃子_414004e836adf6e38d5275515289704d/PoverEquipment`。
- 生成并保存 40 个 UE 资产：4 个 SkeletalMesh、4 个 Skeleton、20 个 Texture2D、12 个 Material。
- 重建 12 个材质的 34 条贴图绑定，覆盖 10 张实际被 PMX 使用的贴图；4 个网格的 12 个材质槽均已绑定。
- 原始 25 个文件保留原名、原目录和内容；逐文件 SHA-256 检查通过。额外生成 4 个同名 FBX 作为 UE 导入源。
- 独立渲染命令行进程的 247 项检查全部通过；检查包含磁盘加载、资产类型、骨架、材质槽、贴图参数、材质编译及源文件一致性。
- 四件装备已在临时预览场景中查看，未见缺失贴图或默认棋盘格材质。原地图已恢复，检查结束时无未保存地图或资产。
- 本次没有修改 C++、已有装备蓝图或角色装备配置，没有执行 Git 提交。

## 导入与材质处理方式

源模型为 PMX，而项目当前没有 PMX 导入器。使用隔离安装的 Blender Python 4.2.0 与 [MMD Tools 4.5.14](https://github.com/MMD-Blender/blender_mmd_tools) 转换为 FBX，再使用 UE FbxFactory 导入为骨骼网格。没有升级项目引擎或插件。

原始文件夹 `Tex_0/Tex_0`、`Tex_1`、`sph` 均保留。压缩包 `Tex_0.7z` 保留为原文件，已解压的贴图仅导入一次；新增 `Materials` 文件夹容纳保持 PMX 原名的材质，避免网格和同名材质的包名冲突。

源 PMX 中的贴图路径本身均有对应文件。本次“修复”指在 UE 中重建材质贴图引用并绑定网格槽，并非把原本有效的 PMX 路径改名或迁移。

材质使用 Unlit 模型，将基础贴图、按源配置执行加法或乘法的球面贴图、白色 Toon 贴图合成为输出；源文件的双面设置保留，使用 A 后缀透明贴图的材质接入 Alpha/Opacity。这是一套可在 UE 显示的 MMD 材质适配，不等同于原游戏专用 NPR 着色器。

Tex_0 中的 `_N` 和 `_ID` 按线性数据、TC_MASKS 导入以保留打包通道，未当作普通 RGB 法线误接。源 PMX 没有使用这些原始打包贴图，因此它们保留供后续专用着色器使用。另有 `Smaple.jpg` 作者参考图和 `sph/s_02.png` 未被源模型引用；导入保留，不强行连接。

没有创建 PhysicsAsset，没有转换 PMX 物理、约束或绑定玩家插槽。骨架保留原始骨骼，并由 FBX 导入增加一个 Armature 根节点。

## 完整 UE 资产清单

另见 [pover-equipment-import-2026-10-11-assets.csv](pover-equipment-import-2026-10-11-assets.csv)，共 40 行资产。以下均位于 `/Game/Wuwa/PoverEquipment`。

### 骨骼网格（4）

- [暗流之镰](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/暗流之镰.uasset) — 来源：`暗流之镰.pmx`。
- [残象之翼](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/残象之翼.uasset) — 来源：`残象之翼.pmx`。
- [血誓盟约](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/血誓盟约.uasset) — 来源：`血誓盟约.pmx`。
- [裁风之剑](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/裁风之剑.uasset) — 来源：`裁风之剑.pmx`。

### 骨架（4）

- [暗流之镰_Skeleton](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/暗流之镰_Skeleton.uasset) — 来源：`暗流之镰.pmx`。
- [残象之翼_Skeleton](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/残象之翼_Skeleton.uasset) — 来源：`残象之翼.pmx`。
- [血誓盟约_Skeleton](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/血誓盟约_Skeleton.uasset) — 来源：`血誓盟约.pmx`。
- [裁风之剑_Skeleton](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/裁风之剑_Skeleton.uasset) — 来源：`裁风之剑.pmx`。

### 贴图（20）

- [Smaple](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Smaple.uasset) — 来源：`Smaple.jpg`。
- [sph/4391203i](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/sph/4391203i.uasset) — 来源：`sph/4391203i.jpg`。
- [sph/5836636](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/sph/5836636.uasset) — 来源：`sph/5836636.png`。
- [sph/s_02](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/sph/s_02.uasset) — 来源：`sph/s_02.png`。
- [Tex_0/Tex_0/T_EDarkNanzhuWingMd10011_D](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_EDarkNanzhuWingMd10011_D.uasset) — 来源：`Tex_0/Tex_0/T_EDarkNanzhuWingMd10011_D.png`。
- [Tex_0/Tex_0/T_EWindzhujianMd10011_D](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_EWindzhujianMd10011_D.uasset) — 来源：`Tex_0/Tex_0/T_EWindzhujianMd10011_D.png`。
- [Tex_0/Tex_0/T_EWindzhujianMd10011_N](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_EWindzhujianMd10011_N.uasset) — 来源：`Tex_0/Tex_0/T_EWindzhujianMd10011_N.png`。
- [Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_D](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_D.uasset) — 来源：`Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_D.png`。
- [Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_N](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_N.uasset) — 来源：`Tex_0/Tex_0/T_MB1ZhenwuguanzheMd00601_Weapon_N.png`。
- [Tex_0/Tex_0/T_R5Sword504Md20001_D](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_R5Sword504Md20001_D.uasset) — 来源：`Tex_0/Tex_0/T_R5Sword504Md20001_D.png`。
- [Tex_0/Tex_0/T_R5Sword504Md20001_ID](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_R5Sword504Md20001_ID.uasset) — 来源：`Tex_0/Tex_0/T_R5Sword504Md20001_ID.png`。
- [Tex_0/Tex_0/T_R5Sword504Md20001_N](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_0/Tex_0/T_R5Sword504Md20001_N.uasset) — 来源：`Tex_0/Tex_0/T_R5Sword504Md20001_N.png`。
- [Tex_1/暗流之镰](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/暗流之镰.uasset) — 来源：`Tex_1/暗流之镰.png`。
- [Tex_1/暗流之镰A](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/暗流之镰A.uasset) — 来源：`Tex_1/暗流之镰A.png`。
- [Tex_1/残象之翼](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/残象之翼.uasset) — 来源：`Tex_1/残象之翼.png`。
- [Tex_1/血誓盟约](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/血誓盟约.uasset) — 来源：`Tex_1/血誓盟约.png`。
- [Tex_1/血誓盟约A](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/血誓盟约A.uasset) — 来源：`Tex_1/血誓盟约A.png`。
- [Tex_1/裁风之剑](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/裁风之剑.uasset) — 来源：`Tex_1/裁风之剑.png`。
- [Tex_1/裁风之剑A](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Tex_1/裁风之剑A.uasset) — 来源：`Tex_1/裁风之剑A.png`。
- [toon_Defo_White](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/toon_Defo_White.uasset) — 来源：`toon_Defo_White.png`。

### 材质（12）

- [Materials/暗流之镰](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/暗流之镰.uasset) — 来源：`暗流之镰.pmx::暗流之镰`。
- [Materials/暗流之镰AL](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/暗流之镰AL.uasset) — 来源：`暗流之镰.pmx::暗流之镰AL`。
- [Materials/翼](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/翼.uasset) — 来源：`残象之翼.pmx::翼`。
- [Materials/羽](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/羽.uasset) — 来源：`残象之翼.pmx::羽`。
- [Materials/血誓盟约-剑鞘](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑鞘`。
- [Materials/血誓盟约-剑](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑`。
- [Materials/血誓盟约-剑鞘AL](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘AL.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑鞘AL`。
- [Materials/血誓盟约-剑AL](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑AL.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑AL`。
- [Materials/血誓盟约-剑鞘AL1](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘AL1.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑鞘AL1`。
- [Materials/血誓盟约-剑AL1](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/血誓盟约-剑AL1.uasset) — 来源：`血誓盟约.pmx::血誓盟约-剑AL1`。
- [Materials/裁风之剑](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/裁风之剑.uasset) — 来源：`裁风之剑.pmx::裁风之剑`。
- [Materials/裁风之剑AL](E:/Project/Git/Hodgepodge/Content/Wuwa/PoverEquipment/Materials/裁风之剑AL.uasset) — 来源：`裁风之剑.pmx::裁风之剑AL`。

## 已修复材质及完整贴图绑定清单

另见 [pover-equipment-import-2026-10-11-texture-repairs.csv](pover-equipment-import-2026-10-11-texture-repairs.csv)，共 34 行引用。

每一项列出网格、材质槽和材质；参数 BaseTexture/SphereTexture/ToonTexture 分别表示基础、球面和 Toon 贴图。

### 1. 暗流之镰

网格：`/Game/Wuwa/PoverEquipment/暗流之镰.暗流之镰`；槽：`暗流之镰`。

材质：`/Game/Wuwa/PoverEquipment/Materials/暗流之镰.暗流之镰`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/暗流之镰.png` → `/Game/Wuwa/PoverEquipment/Tex_1/暗流之镰.暗流之镰`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 2. 暗流之镰AL

网格：`/Game/Wuwa/PoverEquipment/暗流之镰.暗流之镰`；槽：`暗流之镰AL`。

材质：`/Game/Wuwa/PoverEquipment/Materials/暗流之镰AL.暗流之镰AL`；双面：是；混合：Translucent。

- `BaseTexture`：`Tex_1/暗流之镰A.png` → `/Game/Wuwa/PoverEquipment/Tex_1/暗流之镰A.暗流之镰A`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 3. 翼

网格：`/Game/Wuwa/PoverEquipment/残象之翼.残象之翼`；槽：`翼`。

材质：`/Game/Wuwa/PoverEquipment/Materials/翼.翼`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/残象之翼.png` → `/Game/Wuwa/PoverEquipment/Tex_1/残象之翼.残象之翼`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 4. 羽

网格：`/Game/Wuwa/PoverEquipment/残象之翼.残象之翼`；槽：`羽`。

材质：`/Game/Wuwa/PoverEquipment/Materials/羽.羽`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/残象之翼.png` → `/Game/Wuwa/PoverEquipment/Tex_1/残象之翼.残象之翼`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 5. 血誓盟约-剑鞘

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑鞘`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘.血誓盟约-剑鞘`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/血誓盟约.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约.血誓盟约`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 6. 血誓盟约-剑

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑.血誓盟约-剑`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/血誓盟约.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约.血誓盟约`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 7. 血誓盟约-剑鞘AL

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑鞘AL`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘AL.血誓盟约-剑鞘AL`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/血誓盟约.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约.血誓盟约`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 8. 血誓盟约-剑AL

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑AL`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑AL.血誓盟约-剑AL`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/血誓盟约.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约.血誓盟约`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 9. 血誓盟约-剑鞘AL1

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑鞘AL1`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑鞘AL1.血誓盟约-剑鞘AL1`；双面：是；混合：Translucent。

- `BaseTexture`：`Tex_1/血誓盟约A.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约A.血誓盟约A`。
- `SphereTexture`：`sph/5836636.png` → `/Game/Wuwa/PoverEquipment/sph/5836636.5836636`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 10. 血誓盟约-剑AL1

网格：`/Game/Wuwa/PoverEquipment/血誓盟约.血誓盟约`；槽：`血誓盟约-剑AL1`。

材质：`/Game/Wuwa/PoverEquipment/Materials/血誓盟约-剑AL1.血誓盟约-剑AL1`；双面：是；混合：Translucent。

- `BaseTexture`：`Tex_1/血誓盟约A.png` → `/Game/Wuwa/PoverEquipment/Tex_1/血誓盟约A.血誓盟约A`。
- `SphereTexture`：`sph/5836636.png` → `/Game/Wuwa/PoverEquipment/sph/5836636.5836636`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 11. 裁风之剑

网格：`/Game/Wuwa/PoverEquipment/裁风之剑.裁风之剑`；槽：`裁风之剑`。

材质：`/Game/Wuwa/PoverEquipment/Materials/裁风之剑.裁风之剑`；双面：是；混合：Opaque。

- `BaseTexture`：`Tex_1/裁风之剑.png` → `/Game/Wuwa/PoverEquipment/Tex_1/裁风之剑.裁风之剑`。
- `SphereTexture`：`sph/4391203i.jpg` → `/Game/Wuwa/PoverEquipment/sph/4391203i.4391203i`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

### 12. 裁风之剑AL

网格：`/Game/Wuwa/PoverEquipment/裁风之剑.裁风之剑`；槽：`裁风之剑AL`。

材质：`/Game/Wuwa/PoverEquipment/Materials/裁风之剑AL.裁风之剑AL`；双面：是；混合：Translucent。

- `BaseTexture`：`Tex_1/裁风之剑A.png` → `/Game/Wuwa/PoverEquipment/Tex_1/裁风之剑A.裁风之剑A`。
- `SphereTexture`：`sph/5836636.png` → `/Game/Wuwa/PoverEquipment/sph/5836636.5836636`。
- `ToonTexture`：`toon_Defo_White.png` → `/Game/Wuwa/PoverEquipment/toon_Defo_White.toon_Defo_White`。

## 骨架验证

- `暗流之镰`：源骨骼 4；UE 骨骼 5（增加一个 Armature 根节点）；源三角形 2948；所有源骨骼名称保留。
- `残象之翼`：源骨骼 269；UE 骨骼 270（增加一个 Armature 根节点）；源三角形 5846；所有源骨骼名称保留。
- `血誓盟约`：源骨骼 2；UE 骨骼 3（增加一个 Armature 根节点）；源三角形 22042；所有源骨骼名称保留。
- `裁风之剑`：源骨骼 1；UE 骨骼 2（增加一个 Armature 根节点）；源三角形 13280；所有源骨骼名称保留。

## 实际执行命令与验证

转换和导入通过以下命令执行；隔离转换依赖位于 `Saved/PoverEquipmentImport/Runtime`。

```powershell
& E:/Python/python.exe Tools/PoverEquipment/convert_pmx.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/PoverEquipment/import_and_repair.py
```

最终磁盘及渲染资源验证采用新的 UE 进程，退出码 0。日志记录 0 个错误、28 条警告，警告来自源代码管理插件探测不存在的 Git 安装路径；没有材质 shader 编译错误。

```powershell
$env:HODGE_IMPORT_FRESH = '1'
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -run=pythonscript '-script=E:/Project/Git/Hodgepodge/Tools/PoverEquipment/validate_import.py' -AllowCommandletRendering -unattended -nop4 -nosplash -nosound '-abslog=E:/Project/Git/Hodgepodge/Saved/PoverEquipmentImport/DiskValidationRendering.log'
Remove-Item Env:/HODGE_IMPORT_FRESH
```

首次尝试在当前编辑器重载新建包时，UE 拒绝重载部分新建贴图包；改用新进程从磁盘读取。普通 Python commandlet 未启用渲染时不会创建材质资源，不能据此读取 shader 统计；加入 `-AllowCommandletRendering` 后，12 个材质的纹理依赖和 shader 统计全部通过。

完整结果、40 个资产、34 条绑定及 25 个源文件校验值见 [pover-equipment-import-2026-10-11-results.json](pover-equipment-import-2026-10-11-results.json)。

本次未执行 C++ 构建、PIE、联机、Cook 或打包，也未验证玩家装备挂接和翅膀动画/物理。这些不属于资源导入验证；当前结果证明 UE 资产可加载、材质引用有效、shader 可编译且模型可显示。

## 本机预览证据

- [四件装备临时场景预览](../../Saved/Screenshots/PoverEquipment-all-preview.png)。画面左上：残象之翼；右上：暗流之镰；左下：裁风之剑；右下：血誓盟约。
- [翅膀骨骼网格编辑器预览](../../Saved/Screenshots/PoverEquipment-wing-preview.png)。
- 预览截图及日志保存在 Saved，不纳入源码变更；临时场景仅用于检查，没有保存为项目地图。
