# 动画与 Montage：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先追踪 UHodgeAnimInstance 与 LyraAnimInstance 的 ASC/GameplayTagPropertyMap 初始化，核实 Pawn 更换与 AnimInstance 重建。
2. Montage 播放由现有能力和战斗链路发起；结束、中断、取消及混出回调验证 Montage/动作身份，避免多个系统重复驱动。
3. 线程安全动画更新只使用允许的数据快照；不能在工作线程随意读写 Actor、ASC 或组件。
4. 根运动、MotionWarping、角色朝向与相机经过项目现有仲裁链路；不为照搬示例重构 ALS 或改无关动画资产。
5. 上游 5.8 新动画插件的能力不视为 5.5 可用；核实本机 uplugin、依赖和蓝图节点后再给配置步骤。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Animation/AnimInstance.h:1326`：`NativeThreadSafeUpdateAnimation`。
- `Source/Runtime/Engine/Classes/Animation/AnimInstance.h:734`：`Montage_SetEndDelegate`。
- `Source/Runtime/Engine/Classes/Animation/AnimMontage.h:22`：`UAnimMontage`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/Animation/LyraAnimInstance.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。

## 项目切入与适配差异

- `Source/Hodgepodge/Private/Animation/HodgeAnimInstance.cpp`：InitializeWithAbilitySystem/NativeInitializeAnimation/NativeUpdateAnimation；先核实 ASC 与 AnimInstance 的实际绑定。
- LyraAnimInstance 的 GameplayTagPropertyMap.Initialize 是接入参考；项目已有运动、根运动和战斗动作约束继续沿用，不把整个 Lyra 动画图当作已迁入。
