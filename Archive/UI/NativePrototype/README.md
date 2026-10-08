# 运行时 UI 原型源码归档

六个业务原生控件类型已迁移为 Main/UI 的 Designer 控件树和 Blueprint Graph。此目录位于 Source 外，不参与 UBT／UHT；不得通过 include 把这里的 cpp 接回 Runtime。

当前框架与共享数据源仍位于 Source/Hodgepodge 的 UI 目录。迁移前的相关源文件和资产备份见 Saved/UMGAuthoringRefactor/Before/manifest.json；恢复旧原型需要恢复整套相关代码和 WBP 父类，再关闭编辑器常规构建，不能只恢复一个旧 uasset。

配置与制作入口见 [UI 指南](../../../Docs/Guides/ui-foundation-configuration.md)。
