# AI 与导航：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先区分 Controller 决策、BehaviorTree/StateTree 运行、Pawn 执行和服务端权限；AI 共用现有战斗请求接口。
2. 任务开始、Abort、结束与移动取消成对处理；观察者/感知委托在角色销毁和控制器更换时解绑。
3. NavMesh 可达、路径跟随结果和目标有效性分别检查，不用直接 SetActorLocation 绕过运动/复制。
4. LyraBotCreationComponent 是创建入口的参考；不为普通怪物增加 PlayerState，不假定 Lyra 含完整怪物战斗。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/AIModule/Classes/AIController.h:180`：`MoveToActor`。
- `Source/Runtime/AIModule/Classes/BehaviorTree/BTTaskNode.h:48`：`AbortTask`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
