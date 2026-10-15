# 自动化、调试和验证：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 文档/skill 修改只验证文档、链接和差异；项目 C++ 修改按现有 Editor/Game 常规构建流程执行。
2. 新增自动化用例覆盖真实风险：坏配置、重复初始化、取消、解绑、旧 Pawn 回调和受影响网络边界，不复制实现凑测试。
3. 命令行自动化使用 -ExecCmds='Automation RunTests <真实过滤器>' 与 -TestExit='Automation Test Queue Empty'；不要立即 quit 导致测试提前退出。
4. 仅在本机确认插件/模块时使用 CQTest；测试启动、进程退出码和实际断言结果分别检查。
5. 报告 Editor/Game、蓝图、PIE、联机、Cook/打包的实际范围；不把日志历史通过计为本次验证。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Core/Public/Misc/AutomationTest.h:4153`：`IMPLEMENT_SIMPLE_AUTOMATION_TEST`。
- `Source/Runtime/Core/Public/Misc/AutomationTest.h:4202`：`BEGIN_DEFINE_SPEC`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
