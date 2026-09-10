# Hodgepodge AI 开发约定

适用于整个项目。开始任务先读本文及 `Docs/AI_DEVELOPMENT.md`，再读相关代码和配置。

- 以当前 `.uproject`、`Source`、`Config`、插件描述为事实依据；README、Lyra 学习文档和架构方案是背景，不代表功能已经实现。
- 保持 UE 5.5 兼容；本机核实为 5.5.4。不要自行升级引擎、插件或构建设置。
- 业务代码属于单一 `Hodgepodge` Runtime 模块，维持 `Public` / `Private` 对应目录和 `Hodge` 命名。不要为了整理目录引入额外模块。
- 优先沿用 Experience、PawnData、GameFeature、GAS 和 Init State 设计；修改前追踪实际调用链，不把 Lyra 示例当成本项目现状。
- 玩家 ASC 的 PlayerState 所有权、服务器权限与客户端复制入口必须同时考虑；新增初始化需处理重复调用、解绑和 Pawn 更换。
- 遵循相邻代码风格、UE 类型前缀和反射宏；`.generated.h` 放在头文件 include 末尾，反射实现沿用 `UE_INLINE_GENERATED_CPP_BY_NAME`。
- UObject 持有关系考虑 GC；公开头文件暴露的模块类型需核实 Build.cs 依赖可见性；Runtime 模块不得无条件依赖 Editor 模块。
- 修改类名、属性、GameplayTag、资产路径前核对蓝图引用、配置和 CoreRedirects。不要顺手修正 `Dafult`、`PlayState` 等已有拼写。
- 第三方 ALS、RiderLink 和二进制资产不做顺手重构或清理；需要修改时明确关联当前任务及验证办法。
- 开始与结束检查工作区状态，保留用户已有改动；不回退、不覆盖、不自动提交。生成文件、缓存和日志不纳入源码变更。
- C++ 修改按 `Docs/AI_DEVELOPMENT.md` 执行 Editor 构建；涉及 Runtime/依赖再执行 Game 构建。文档修改只验证文档和差异。
- Live Coding 成功不替代常规构建；反射布局、继承、默认子对象或构建配置修改后应关闭编辑器编译，再重新打开。
- 每次交付说明改动、实际执行的命令及结果、未验证项；需要用户操作时给出具体资产路径、属性、操作步骤和预期结果。
- 不把“C++ 编译通过”描述成“蓝图、PIE、联机或打包通过”；不要要求用户通过编辑器配置尚不存在的字段来弥补代码缺口。
