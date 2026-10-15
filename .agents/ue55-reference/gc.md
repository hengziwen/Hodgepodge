# 对象生命周期与 GC：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先区分 UObject 与普通 C++ 对象。需要保活的 UObject 成员使用反射可见的 UPROPERTY/TObjectPtr，不能用 TSharedPtr 管理 UObject。
2. TWeakObjectPtr 不保活，回调执行前重新检查；TSoftObjectPtr 记录资源路径，加载后的保活关系要单独安排。
3. 普通 C++ 持有者需要保活 UObject 时核实 FGCObject 或 TStrongObjectPtr 的契约；Outer 关系不能直接当作保活证明。
4. 沿用项目 GC 设置，不因 5.8 示例开启增量 GC。Pawn/动作切换时还要验证执行身份，弱引用有效不代表回调仍属于当前动作。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/CoreUObject/Public/UObject/ObjectPtr.h:173`：`TObjectPtr`。
- `Source/Runtime/Core/Public/UObject/WeakObjectPtrTemplates.h:32`：`TWeakObjectPtr`。
- `Source/Runtime/Core/Public/UObject/StrongObjectPtrTemplates.h:30`：`TStrongObjectPtr`。
- `Source/Runtime/CoreUObject/Public/UObject/GCObject.h:71`：`AddReferencedObjects`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
