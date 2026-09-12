# 项目定位、目录与文档可信度

[返回首页](README.md)

## 工程是什么

项目描述的长期目标是 Dedicated Server、数据驱动、可扩展的动作 RPG 底层。它现在是单 Runtime 模块 `Hodgepodge`，引擎关联为 5.5。框架大量借鉴 Lyra，角色移动曾依赖 ALS，目前模块依赖已经移除 ALS，但资源和插件参考目录仍在。

“框架有这些类”与“游戏具备这些能力”必须分开。Experience、ASC、AbilitySet、相机模式栈有代码；可移动玩家、完整战斗循环、AI、装备背包、UI、后端不能据此自动认定完成。

## 目录阅读地图

- `Source/Hodgepodge/Public`：反射类型、组件接口、配置字段。理解一个系统先从这里确定契约。
- `Source/Hodgepodge/Private`：函数体和真实执行路径。确认实现必须看这里，尤其要排除注释代码。
- `Source/Hodgepodge/Hodgepodge.Build.cs`：模块依赖及 Iris 支持设置。
- `Source/*.Target.cs`：已有 Game / Editor 目标。独立 Server Target 需要另行补充。
- `Config`：默认地图、类重定向、AssetManager 扫描及输入设置。
- `Content/Main`：自有玩法数据、输入资源与角色动画；详见资产清单。
- `Content/qiuyuan`、`Content/Wuwa`、`Content/Assets`：模型、动画、音效等资源集合。资源存在不证明已经被 Pawn 使用。
- `Plugins/Developer/RiderLink`：开发工具插件，不计入游戏核心逻辑索引。
- `Plugins/ALS-Refactored-4.15`：旧依赖参考目录；存在不代表当前启用。
- `Saved`、`Intermediate`、`Binaries`：运行或构建产物，不作为架构事实来源。

`Core/PlayState` 的拼写是项目现状。知识库保留实际路径，不擅自改成 PlayerState。

## 文档冲突怎么处理

本地源码和配置回答“现在实现了什么”；资产编辑器回答“现在配了什么”；运行日志和验收回答“实际跑起来怎样”。项目 README 是历史状态摘要，Lyra 两篇文档是上游参考，V2 方案是目标设计。

若 README 写“相机完整接入”，但没有有效的 `DetermineCameraModeDelegate` 绑定者，应记录为“相机实现存在，默认模式入口未接通”。若文档说 HeroComponent 不存在，而磁盘已有全部注释的文件，应记录为“草稿存在，类尚未启用”。

## 当前工作区与提交基线

知识库以读取时的磁盘内容为准。生成前已有 PawnData、HeroComponent 和 DefaultEngine.ini 修改，未由本次任务创建或覆盖。精确 Git HEAD、文件数量、SHA-256 和工作区列表见 [扫描快照](Reference/snapshot.md)。

这意味着 checkout 到旧提交后，人工章节可能描述得比代码超前；从磁盘删除或启用草稿后，旧索引也可能失效。维护工具可以提示文件变化，但不能替代语义复核。

## 如何有效阅读代码

1. 先看头文件继承和 UPROPERTY，确定所有权与数据入口。
2. 看构造函数，确认组件是否真正 `CreateDefaultSubobject`。
3. 看生命周期，确认注册、初始化、复制通知和销毁清理。
4. 全局查函数调用，不只看函数定义。
5. 检查调用是否在注释中、条件分支是否可达。
6. 最后查资产配置和实际运行日志。

例如找到 `InitializeAbilitySystem` 定义只能证明有初始化接口；若唯一调用写在全部注释的 HeroComponent 中，仍应判为没有有效调用入口。
