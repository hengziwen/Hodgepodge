# 攻击限定翅膀制作与验证

设计和需求编号见 [设计文档](../../Docs/Design/attack-wing-presentation.md)。本工具仅启用新 Anzhu 动画模式，旧程序化翅膀仍关闭，不增加翼网格伤害。

1. 常规构建 HodgepodgeEditor / Hodgepodge；构建前保存并关闭编辑器。
2. 通过 `Tools/HitReaction/mcp_execute.py` 执行 `configure_assets.py`。它保留 WingText 的挂接，新增 Profile、组件和角色通知窗口；会改写本功能自己的映射及窗口，用户后续调参前应保留备份，勿盲目重跑。
3. `validate_assets.py` 只读校验资产与注入唯一性。
4. `start_test.py` 启动单人真实输入测试并自动隔离敌人；全局 `WING_MODE='listen'/'dedicated'` 可以启动对应双客户端模式。启动回调自动执行 `run_e2e.py`。
5. 每次结束执行 `stop_test.py`，全部结束后执行 `restore_editor.py`，恢复地图、网络及性能选项。
6. `write_report.py` 将实际测试结果与需求追溯写入 Docs/Validation。

自动化命令使用 `Automation RunTests Hodge.Wing`；原始报告、端到端逐帧数据、构建日志及备份均在 `Saved/WingAttack`。

配置入口：`/Game/Main/Character/Hero/Wing/DA_PoverWing_Anzhu`。显示窗口位于 `AM_Attack04_Montage` 和预览用 `AM_Attack04` 的 `AttackWing` NotifyState。窗口采用源动画秒；映射倍率使用 PlayRateRatio，不是壁钟延迟。WingText 身体仍循环播放攻击，所以预览中翅膀在每次循环的窗口内显现；正式 Hero 空闲时没有匹配源，保持隐藏。

隐藏淡出：`configure_fade.py` 创建独立材质副本并配置 `FadeOutSeconds=0.35`、`VisibilityParameter=WingVisibility` 和三个材质覆盖。它不改原模型与原材质；先完成常规构建再执行。重复执行复用自己的材质副本，不重置动画窗口和挂接。后续仅在 Profile 调整 FadeOutSeconds，零表示立即隐藏。E2E 会同时验证各槽位的实际 MID 参数，`WING_WORK` 可指定独立报告目录。

翼 Box：`configure_box.py` 沿现有伤害窗口采样蒙皮顶点，将该条 WingMesh 通知改为 Bone_Wing000 锚点的跟随盒体，并开启权威采样；原资产备份与尺寸报告位于 Saved/WingBox。它只排除特效槽，保留实体与遮罩翼面，按 0.8 网格缩放测量世界厘米尺寸，不新增 Socket、不修改材质或其他通知。动作、缩放或窗口改变后才重新校准；手动调参可直接改通知的 BoxHalfExtent 与 LocalTransform。

`validate_box.py` 检查几何、去重和正式开关；`run_box_damage.py` 使用真实玩家输入、隔离的运行时小胶囊目标验证翼盒单独伤害、范围外拒绝、多帧去重和取消后无伤害，不修改生产怪物配置。start_test.py 可通过 WING_TEST_SCRIPT 选择该测试脚本，WING_WORK 指向 Saved/WingBox；原表现 E2E 会按 Profile 开关区分专服没有翼网格与专服隐藏采样网格。
