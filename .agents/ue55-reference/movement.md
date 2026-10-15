# CharacterMovement：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 沿用现有 ACharacter/CharacterMovement 和动作请求入口，先追踪输入、AI、能力到实际运动执行者。
2. 自定义运动需同时检查服务器模拟、客户端预测、SavedMove、纠正和模拟代理表现；本地移动正常不足以证明联网正确。
3. RootMotion/MotionWarping 与角色旋转不可各自 Tick 争写；临时移动参数在动作取消和 Pawn 销毁时恢复且验证执行身份。
4. 保持 UE5.5 的虚函数签名和项目既有配置，不把 5.8 Mover 例子换入现有 CMC。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h:1951`：`PhysCustom`。
- `Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h:2898`：`FSavedMove_Character`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
