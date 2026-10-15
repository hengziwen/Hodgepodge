# 核心类型、日志和容器：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 按标识、显示文本与普通字符串选择 FName/FText/FString；本地化文本不得通过 FString 往返丢失语义。
2. TArray/TMap/TSet 遍历时考虑扩容和迭代器失效；引用与指针的生命周期随容器变化重新评估。
3. 日志包含对象、资产、请求和失败阶段；check 只表达程序不变量，坏配置和运行失败提供可诊断返回路径。
4. 遵循项目中文注释和相邻风格，不按上游格式化建议重排无关代码。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Core/Public/Containers/Array.h:393`：`TArray`。
- `Source/Runtime/Core/Public/UObject/NameTypes.h:870`：`FName`。
- `Source/Runtime/Core/Public/Misc/AssertionMacros.h:425`：`ensure`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
