# Subsystem 生命周期：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 按 Engine/GameInstance/World/LocalPlayer 生命周期选择现有 Subsystem，优先扩展已承担职责的系统。
2. Initialize/Deinitialize 配对注册与解绑，依赖初始化使用 Collection 而非假设构造顺序。
3. 多 PIE World 和本地玩家分别持有状态，不用全局静态缓存串联不同世界；异步回调验证所属世界。
4. 引擎已有任务机制能满足时避免增加 Tickable Subsystem；需要 Tick 时记录范围和开销。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Public/Subsystems/SubsystemCollection.h:33`：`InitializeDependency`。
- `Source/Runtime/Engine/Public/Subsystems/WorldSubsystem.h:61`：`UTickableWorldSubsystem`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
