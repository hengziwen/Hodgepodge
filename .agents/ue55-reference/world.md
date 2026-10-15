# 世界分区与关卡流送：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先确定 WorldPartition、DataLayer、LevelInstance 或传统子关卡的实际方案，检查默认地图和 WorldSettings 覆盖。
2. 流送加载、可见、Actor 初始化和业务就绪分别确认，异步回调验证当前 World 与加载请求。
3. 硬引用、跨层引用、HLOD 和 Cook 规则分别核实，不能凭编辑器已加载判断打包可用。
4. 关卡切换和卸载清理临时对象/委托，持久玩家状态沿用 PlayerState/GameInstance 所属链路。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Engine/LevelStreaming.h:22`：`ULevelStreaming`。
- `Source/Runtime/Engine/Public/WorldPartition/WorldPartition.h:35`：`UWorldPartition`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
