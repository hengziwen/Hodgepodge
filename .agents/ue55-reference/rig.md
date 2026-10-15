# Control Rig 与 IK：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 读取源/目标骨架、参考姿势、比例和现有 IK Rig/Retargeter，先定位本机 5.5 控制器和节点能力。
2. 上游 5.8 IK Retargeter 操作栈和控制器 API 不作为 5.5 前提，使用本机 IKRetargeter 声明和现有资产流程。
3. ControlRig 与 AnimGraph 明确执行阶段和坐标空间，避免动画工作线程访问不允许的 UObject 状态。
4. 只保存关联骨架/动画/Rig 资产；验证关节、足部接触、根运动和实际角色表现，不批量重存 ALS。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Animation/IKRig/Source/IKRig/Public/Retargeter/IKRetargeter.h:218`：`UIKRetargeter`。
- `Plugins/Animation/ControlRig/Source/ControlRig/Public/ControlRig.h:50`：`UControlRig`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
