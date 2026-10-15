# 材质与着色器：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 区分运行时 MID/参数集合与编辑器图表修改；运行时不依赖 MaterialEditingLibrary。
2. 先检查现有材质、参数名、使用对象和实例层级；不批量覆盖第三方材质，不凭工具目录推断图表编辑能力。
3. 上游 Substrate/着色器示例需核实 5.5 项目渲染配置与平台，禁止为复用示例切换渲染管线。
4. 改图后检查编译、目标平台和实际视觉结果；动态参数生命周期跟随实际组件/动作。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Public/Materials/MaterialInstanceDynamic.h:22`：`SetScalarParameterValue`。
- `Source/Editor/MaterialEditor/Public/MaterialEditingLibrary.h:112`：`CreateMaterialExpression`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
