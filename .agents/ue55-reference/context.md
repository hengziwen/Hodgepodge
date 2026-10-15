# 项目上下文核查：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 读取当前 .uproject、Target/Build.cs、Source、Config 和插件描述，更新 .agents/ue-project-context.md，事实保留核查日期和来源。
2. 工程路径固定到本工作区和 UE5.5.4；移除 LocalUser、D:/Hodgepodge、未启用字段等旧环境假设。
3. 只记录源码/配置可证明的能力，资产内部、PIE、联机和打包结果必须来自对应实际操作。
4. 发现上下文与源码冲突时先相信源码；不让通用 skill 的项目模板覆盖用户现有文档或未提交改动。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Programs/UnrealBuildTool/Configuration/ModuleRules.cs:1213`：`PublicDependencyModuleNames`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
