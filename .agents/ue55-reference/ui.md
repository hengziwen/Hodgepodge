# UMG、Slate 与 CommonUI：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本项目已经声明 UMG/Slate/CommonUI/CommonInput 依赖；仍要追踪实际 UIManager、层栈和宿主资产，不能沿旧快照认定未实现。
2. 控件展示从现有事件/属性委托更新，Construct/Destruct 重复调用的绑定必须幂等；PlayerState/Pawn 更换重新绑定。
3. CommonActivatableWidget 管理激活/输入/焦点时保持现有路由；不用 Widget Tick 轮询 ASC，也不引入 CommonGame/CommonUser。
4. BindWidget 字段、控件树与蓝图父类必须实际存在；业务数据为真实来源或明确标注样例。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/UMG/Public/Blueprint/UserWidget.h:1514`：`NativeDestruct`。
- `Plugins/Runtime/CommonUI/Source/CommonUI/Public/CommonActivatableWidget.h:151`：`NativeOnActivated`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
