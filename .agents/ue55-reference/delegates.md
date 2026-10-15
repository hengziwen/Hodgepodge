# 委托与事件：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 按是否需要蓝图和反射选择普通或 Dynamic 委托；Dynamic 回调核实 UFUNCTION 和精确签名。
2. 普通多播保存 FDelegateHandle 并 Remove；Dynamic 使用成对 AddDynamic/RemoveDynamic，重复初始化先处理已有绑定。
3. AddUObject/AddWeakLambda 只能处理对象有效性，不能替代 Pawn/动作身份检查；跨线程回调应在合适线程提交结果。
4. 本机 5.5.4 已有 DECLARE_TS_MULTICAST_DELEGATE；它不使被调用 UObject 或业务逻辑自动线程安全。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Core/Public/Delegates/DelegateCombinations.h:26`：`DECLARE_TS_MULTICAST_DELEGATE`。
- `Source/Runtime/Core/Public/Delegates/DelegateSignatureImpl.inl:831`：`AddWeakLambda`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
