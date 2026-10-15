# GameplayTag 与消息：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先查项目 Native Tags、DefaultGameplayTags.ini 和实际资产引用；保留既有拼写与重定向。
2. 区分精确匹配和层级匹配，用容器或 TagQuery 表达组合条件；状态事实仍由所属系统维护。
3. 注册/解绑标签变化委托要成对，确认客户端收到的标签源；松散标签和 GameplayEffect 授予标签的复制行为分别检查。
4. Lyra GameplayMessageRuntime 是样例插件能力；不假定 Engine 或项目已经启用它，也不为通知创建第二套状态源。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/GameplayTags/Classes/GameplayTagContainer.h:112`：`MatchesTagExact`。
- `Source/Runtime/GameplayTags/Public/NativeGameplayTags.h:41`：`UE_DEFINE_GAMEPLAY_TAG`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
