# 编辑器工具与 Python：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 优先沿用 HodgeAbilityEditor 和已有 AuthoringLibrary；UE 编辑器 Python 通过 unreal 运行，普通系统 Python 没有编辑器对象。
2. 编辑器模块依赖受目标限制，Runtime 不直接引用 UnrealEd；读取 5.5 实际子系统接口，不复制 5.8 ToolsetRegistry。
3. 按目标资产开启事务、标记修改并保存，先读取状态，异步操作失败后读取结果再决定是否重试。
4. UI/动画编辑使用现有工具真实支持的范围；不得通过静默保存全部资产或升级插件补齐缺失能力。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Editor/UnrealEd/Public/Subsystems/EditorAssetSubsystem.h:277`：`SaveLoadedAsset`。
- `Source/Editor/Blutility/Public/EditorUtilitySubsystem.h:48`：`UEditorUtilitySubsystem`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
