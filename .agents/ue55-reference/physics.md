# 碰撞与 Chaos：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 复用项目已有 HitDetection/Trace 通道和动作请求；先确定服务器伤害裁定、客户端预测表现及一次命中的去重范围。
2. 区分 Trace/Sweep/Overlap、对象通道与响应配置，查询忽略自身和持有装备，校验返回的有效目标。
3. ragdoll/物理模拟与运动、动画恢复的状态归属保持单一，取消时恢复对应执行者拥有的参数。
4. 异步物理/网络物理以本机接口为准，不将 5.8 示例复制进当前 CMC/GAS 链路。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Engine/World.h:1984`：`SweepMultiByChannel`。
- `Source/Runtime/Engine/Classes/Components/PrimitiveComponent.h:1527`：`SetSimulatePhysics`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
