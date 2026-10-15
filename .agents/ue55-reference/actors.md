# Actor 与组件生命周期：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 区分构造/CDO、OnRegister、InitializeComponent、BeginPlay 与 EndPlay，默认子对象只在构造期创建。
2. 动态组件核实 Outer、AddInstanceComponent/注册、复制和卸载；附着规则明确局部/世界变换与 Socket。
3. 组件依赖通过现有 Init State 协调，重复初始化和 Pawn 更换不得重复绑定或创建资源。
4. 委托、Timer、异步加载和能力句柄在对应 EndPlay/停用路径释放，不能只依赖析构。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/GameFramework/Actor.h:2050`：`BeginPlay`。
- `Source/Runtime/Engine/Classes/Components/ActorComponent.h:686`：`OnRegister`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
