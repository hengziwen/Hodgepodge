# Timer、异步与线程：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 按一次性延迟、后台纯计算、并行批处理选择 Timer/UE::Tasks/Async；不为等待依赖添加无限轮询。
2. TimerHandle、任务和委托由创建者负责清理；EndPlay、取消和 Pawn 更换验证执行身份。
3. 后台线程处理拷贝的数据，访问 UObject 与提交游戏状态回到允许的线程；不要同步等待造成游戏线程死锁。
4. 以本机头文件核实 5.5 Tasks API；上游 FTaskConcurrencyLimiter 等版本敏感类型先检查存在性和签名。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Public/TimerManager.h:276`：`ClearTimer`。
- `Source/Runtime/Core/Public/Tasks/Task.h:141`：`Launch`。
- `Source/Runtime/Core/Public/Async/Async.h:449`：`AsyncTask`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
