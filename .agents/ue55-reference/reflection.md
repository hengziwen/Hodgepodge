# C++ 反射与蓝图接口：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 沿用 UE 类型前缀、Hodge 命名、反射宏、中文注释和 Public/Private 结构；先明确调用方和可见性，再暴露最小蓝图接口。
2. UCLASS/USTRUCT/UPROPERTY/UFUNCTION 的组合以 5.5 UHT 和相邻代码为依据；generated.h 最后 include。
3. 修改反射字段、继承或默认子对象前检查资产引用和 CoreRedirects，保存并关闭编辑器常规构建，再冷启动编译受影响蓝图。
4. 不要用 AddToRoot 掩盖持有关系，也不要通过编辑尚不存在的蓝图字段弥补代码缺口。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/CoreUObject/Public/UObject/ObjectMacros.h:740`：`UCLASS`。
- `Source/Runtime/CoreUObject/Public/UObject/Object.h:99`：`UObject`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
