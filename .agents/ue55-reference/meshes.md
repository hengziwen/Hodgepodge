# 网格与骨骼资源：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先检查 Skeleton、Socket、LOD、材质槽和物理资产，确认现有蓝图与装备引用。
2. 区分资源编辑和组件运行时设置；骨架/默认子对象变更后冷启动编译关联动画蓝图。
3. 导入/重导入核实单位、坐标轴、骨骼映射和原文件设置，只写任务关联资源。
4. 性能与视觉结果在目标场景验证，资源存在不证明骨架和引用正确。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Components/SkeletalMeshComponent.h:330`：`SetSkeletalMeshAsset`。
- `Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h:414`：`SetStaticMesh`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
