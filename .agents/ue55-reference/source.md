# 引擎源码定位：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 从 E:/UE/UE_5.5/Engine/Build/Build.version 确认 5.5.4，再用 rg --files 与 rg 查声明、定义、调用者和模块 Build.cs。
2. Public/Classes 决定 include 路径；公开头文件使用的模块类型要检查 PublicDependencyModuleNames，链接导出还要检查 API 宏。
3. 删除上游默认 5.8/5.7 及 E:/Program Files 引擎路径假设。找到同名符号之后还要阅读实现、默认值和版本弃用标记。
4. 报告真实文件、函数和适配差异；引擎或 Lyra 源码未覆盖的部分单独说明，不以历史示例证明当前实现。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/CoreUObject/Public/UObject/Object.h:99`：`UObject`。
- `Source/Programs/UnrealBuildTool/Configuration/ModuleRules.cs:1213`：`PublicDependencyModuleNames`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
