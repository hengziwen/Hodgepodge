# R2T1PlayerMaleMd10011 材质修复（2026-09-14）

交付资产：/Game/CodexText/Model/R2T1PlayerMaleMd10011。
6 个材质槽指向同文件夹材质；5 张 D 贴图接 Base Color，上下装 N 以 BC5 法线压缩重建 RG 法线。头发双面、不将未知 alpha 当透明度。六个材质支持骨骼及 Morph Target。MI_3XingStar 未提供贴图，使用金色常量替代。粗糙度为估计值，未还原原游戏卡通阴影、描边、ID/HM 遮罩驱动效果。

已执行 repair.py、fix_slots.py、final_check.py，经 UE 5.5.4 原生 MCP 调用；材质保存、6 槽与贴图本地引用检查通过，骨骼网格预览实际显示颜色与衣服细节。未执行打包测试，无 C++ 变更。

备份：D:/CodexUE55Lab/ModelRepairBackup/Model。
源目录哈希：D:/CodexUE55Lab/ModelRepairBackup/source-hashes.json。
过程中验证脚本误保存源材质的 Morph 使用标记，已从与修复前哈希完全一致的 Git LFS 对象恢复六个源文件，并重载编辑器中的源材质内存。最终源目录所有已记录文件 SHA-256 一致。verify.py 已改为只读并增加路径断言。
