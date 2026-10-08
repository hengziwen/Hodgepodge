# 普攻历史工具与当前路径

2026-10-06 已将五段正式 GA、Definition、Timeline、AbilitySet 和 Combo 数据迁入 Main。当前路径与验证见 [资产迁移记录](../../Docs/Validation/main-assets-migration-2026-10-06.md)。

- GA / Definition：`/Game/Main/Character/Hero/Ability/BasicAttack`。
- Timeline：该目录下 `Timeline`；使用 Definition.ExecutionConfig.TimelineTaskConfig.Timeline 的实际引用。
- Combo 定义和表：`/Game/Main/Data/Combo`；AbilitySet：`/Game/Main/Data/AbilitySet/AS_LightCombo`。

`create_assets.py`、`create_definition_assets.py` 保留最初创建实验资产的脚本和输出目录，当前不再用它们配置正式角色；不要直接复跑创建重复资产。其余脚本的正式资源读取路径已更新，但测试语义仍对应各自历史验证阶段，例如旧 montage duration 脚本预设有限连段，而当前末段允许回到第一段。本次迁移未执行这些改写配置的历史脚本。
