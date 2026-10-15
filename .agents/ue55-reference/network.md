# 复制与服务器权限：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 分别识别服务器、拥有者客户端和模拟代理，明确谁修改事实、谁发请求、谁接收 OnRep；拥有 Actor 不等于拥有服务器权限。
2. 属性复制核实 GetLifetimeReplicatedProps/DOREPLIFETIME；FastArray 的增删改核实标记脏与子对象注册/撤销。
3. Push Model 必须同时检查注册参数、运行配置和写入时的脏标记；SetupIrisSupport 只提供构建支持，不能证明运行时启用了 Iris。
4. RPC 验证发送者、当前 Pawn、目标和参数；不用 Reliable 每帧发送表现数据。GAS 预测保留 ASC 自有协议。
5. 分别测试 Listen Server、拥有者/其他客户端、重生和重连。本项目没有独立 Server Target，PIE Dedicated 不证明独立专服构建成功。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Public/Net/UnrealNetwork.h:261`：`DOREPLIFETIME`。
- `Source/Runtime/Net/Core/Classes/Net/Serialization/FastArraySerializer.h:443`：`MarkItemDirty`。
- `Source/Runtime/Engine/Classes/GameFramework/Actor.h:1866`：`HasAuthority`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
