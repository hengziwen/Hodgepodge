# Hodgepodge UE 5.5.4 skill 契约

三套上游完整内容与提交号保存在 `skill-sources/` 和 `ue55-skills.lock.json`。`skills/` 下的入口已改写为本项目的 UE 5.5.4 工作流；上游原始文档保留用于查阅和版本比较，其中的 5.8 示例没有被整体移植或编译认证。

## 使用次序

1. 遵守仓库 `AGENTS.md`、`Docs/Development/development-standards.md`、`Docs/AI_DEVELOPMENT.md`，再读取当前 skill 的领域参考。
2. 当前工程事实由 `.uproject`、Source、Config、插件描述和实际资产读取结果决定；`.agents/ue-project-context.md` 是核查起点，不能替代源码。
3. 具体方案前查本机 Lyra 的声明、实现、调用者、初始化/清理、配置和模块依赖；成熟机制直接复用并做最小适配。记录参考文件/函数、依赖、适配差异及占位缺口。
4. 引擎入口以 `E:/UE/UE_5.5/Engine` 为准，先读取 `Build/Build.version`；本次核实为 5.5.4、CL 40574608。Lyra 为 `E:/Project/UProject/study/Epic/LyraStarterGame`。换机器先重新定位，不照抄旧路径。

## 原始文档的使用边界

- `skill-sources/` 位于 Codex 的 `.agents/skills/` 发现目录之外，保留全部上游知识、脚本、引用和许可；不将其中的 `SKILL.md` 当作本地执行入口。
- 先使用当前入口及领域参考。需要更细的主题背景时，才按入口链接读取对应上游章节，不能整套载入，也不执行上游附带脚本。
- 准备采用上游代码时，在本机 5.5 头文件查精确签名，阅读实现、调用者、模块、弃用标记和启用状态，再形成项目实现。路径存在只说明定位有效，不证明全部行为适用。
- 没有对应符号/插件时，说明具体缺口，选择本机已有机制或继续不依赖它的工作；不通过升级 UE、启用新插件或删除用户配置来适配文档。

## 项目实现与验证

- 业务保持单一 Hodgepodge Runtime 模块及已有 HodgeAbilityEditor 工具模块；Hodge 命名、Public/Private 对应、同类主 cpp、中文注释按开发准则执行。
- 玩家 ASC 的 PlayerState Owner 与 Pawn Avatar 分开处理，怪物不套用玩家所有权。授予/撤销、委托、Timer、资源和动作请求配对；旧 Pawn/旧动作的异步回调不能恢复旧状态或清理新资源。
- 初始化可重复进入，依赖等待、配置错误和运行失败分别诊断；属性、GameplayTag、路径和默认子对象变更先检查资产与重定向。
- C++ 改动执行常规 Editor 构建；Runtime/依赖变更补 Game 构建。反射/继承/默认子对象变更保存并关闭编辑器构建，再冷启动核对蓝图。Live Coding 不代替该流程。
- skill/文档变更只验证 frontmatter、命名、引用、来源完整性和差异。本次不会因为安装 skill 自动构建、操作资产或打包。
- 交付区分 C++、蓝图、PIE、联机和 Cook/打包的实际结果，列出命令与未验证项；不自动提交或推送。
