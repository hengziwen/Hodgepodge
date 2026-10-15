# 蓝图图表与编译：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先读取目标蓝图父类、默认值和目标图类型，核实 C++ 暴露接口与资产字段，不猜引脚名。
2. 现有普通 EventGraph 桥不能证明支持 AnimGraph、函数图或 UMG 树；按实际工具 schema 选择编辑方式。
3. 编辑现有资产前保留恢复点，串行修改同一资产，读取结果、编译诊断并保存明确目标，不批量重存无关资产。
4. 蓝图编译、保存持久化、PIE 玩法和联机分别记录；工具返回成功不代替读取最终资产。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Engine/Blueprint.h:31`：`UBlueprint`。
- `Source/Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h:169`：`CompileBlueprint`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
