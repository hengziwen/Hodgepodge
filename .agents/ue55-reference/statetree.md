# StateTree：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 使用本机 StateTreeModule 头文件核实 Link、EnterState、Tick、ExitState 和 InstanceData 类型，签名不从 5.8 复制。
2. 条件/评估器读取状态，任务负责动作；退出、失败、取消均撤销请求和外部回调，避免旧任务恢复角色。
3. 5.8 Scheduled Tick、Considerations 和 Delegate 示例先查本机接口，未找到就使用 5.5 既有任务/事件机制。
4. Init State 是初始化协议，StateTree 是行为执行；不以行为切换倒退 Pawn 初始化状态。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Runtime/StateTree/Source/StateTreeModule/Public/StateTreeTaskBase.h:36`：`EnterState`。
- `Plugins/Runtime/StateTree/Source/StateTreeModule/Public/StateTreeExecutionContext.h:95`：`FStateTreeExecutionContext`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
