# 音频与 MetaSound：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 区分一次性 PlaySound 与可控制 AudioComponent，明确附着、空间化、并发和停止/释放的所有者。
2. MetaSound 参数名和类型从现有资源读取，运行时调用核实 5.5 AudioComponent 接口与 MetasoundEngine 依赖。
3. 音乐/Quartz、混音和衰减按需要单独配置，委托和异步结果在世界/Pawn 结束时清理。
4. 服务器事件与各客户端音效播放分开，避免 Listen Server 重复播放；验证实际听感而非仅工具返回成功。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Components/AudioComponent.h:541`：`SetFloatParameter`。
- `Plugins/Runtime/Metasound/Source/MetasoundEngine/Public/MetasoundSource.h:128`：`UMetaSoundSource`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
