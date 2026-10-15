# Mass Entity：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本机 5.5.4 MassEntity 模块位于 Engine/Source/Runtime/MassEntity，不能凭上游版本迁移叙述推断其位置。
2. 任务确需 Mass 时才评估项目依赖；先明确 Fragment/Tag、Query 的读写权限、Processor 阶段和游戏线程要求。
3. 以本机 ConfigureQueries 签名和 FMassEntityQuery API 为准，不套用 5.8 构造/注册变更；结构修改走 Deferred 命令。
4. 实体句柄失效、World 切换与 UObject 桥接单独处理；没有需求不把现有 Actor/GAS 框架迁移成 ECS。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/MassEntity/Public/MassProcessor.h:119`：`ConfigureQueries`。
- `Source/Runtime/MassEntity/Public/MassEntityQuery.h:45`：`ForEachEntityChunk`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
