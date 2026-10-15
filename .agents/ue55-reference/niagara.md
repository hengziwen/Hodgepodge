# Niagara 特效：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先检查现有 System/Emitter、User 参数、材质、渲染器和激活方式；特效生命周期归属到当前组件/动作。
2. SpawnSystemAttached/AtLocation 的池化和自动销毁参数以本机签名为准，结束与取消时处理组件和回调。
3. DataChannel、SimCache、LightweightEmitter 等功能按本机 5.5 类和插件逐项查验，不能套用 5.8 示例。
4. 表现复制与伤害裁定分离；自定义模块、火焰、溶解、序列特效只修改任务关联资源，并验证性能和视觉结果。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h:45`：`SpawnSystemAttached`。
- `Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h:517`：`SetVariableFloat`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
