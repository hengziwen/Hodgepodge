# PCG 与程序化几何：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先核实项目是否启用 PCG/GeometryScript/ProceduralMeshComponent，不因安装 skill 自动启用插件。
2. 按本机 PCGSettings/PCGContext 创建 CPU 节点，读取实际输入类型、种子、取消和输出契约。
3. 5.8 PointData/PointArrayData 与 GPU 执行 API 不能当作 5.5 既有能力；缺失时使用本机 CPU PCG 路径。
4. 区分编辑器生成、运行时生成、WorldPartition 和 Cook，输出对象由正确世界持有并支持清理/重复生成。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/PCG/Source/PCG/Public/PCGSettings.h:23`：`UPCGSettings`。
- `Plugins/PCG/Source/PCG/Public/PCGContext.h:72`：`FPCGContext`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
