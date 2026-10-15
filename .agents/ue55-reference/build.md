# 模块与 UBT 构建：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 保持 Hodgepodge 单一业务 Runtime 模块和已有 HodgeAbilityEditor 工具模块，Public/Private 对应，不为整理目录增加模块。
2. 本机保持 BuildSettingsVersion.V5 与 IncludeOrderVersion.Unreal5_5；不使用上游 5.8 枚举或默认构建参数。
3. 公开头文件暴露的模块类型需要 Public 依赖；Editor 依赖受 Target.bBuildEditor/WITH_EDITOR 限制，Game 构建验证不能省略。
4. 按 Docs/AI_DEVELOPMENT.md 常规构建 Editor；Runtime/依赖改变再构建 Game。保存后关闭编辑器，不强杀未保存工作，Live Coding 不代替常规构建。
5. 同类非内联实现集中主 cpp；generated.h 位于头文件 include 末尾，反射实现沿用 UE_INLINE_GENERATED_CPP_BY_NAME。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Programs/UnrealBuildTool/Configuration/ModuleRules.cs:1213`：`PublicDependencyModuleNames`。
- `Source/Programs/UnrealBuildTool/Configuration/TargetRules.cs:196`：`Unreal5_5`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
