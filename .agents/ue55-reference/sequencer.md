# Sequencer 与渲染队列：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先读取序列资产、帧率、绑定对象、相机切换与播放世界，区分 Possessable/Spawnable 的生命周期。
2. SequencePlayer 的播放、结束和中断与角色输入/相机占用配对；旧序列回调不得清理新动作。
3. 运行时 LevelSequence/MovieScene 与编辑器 MovieRenderPipeline 依赖分离，MRQ/MovieRenderGraph 按本机插件核实。
4. 渲染前明确帧范围、输出目录、格式和成本；启动渲染队列不等于产出已验证视频。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/LevelSequence/Public/LevelSequencePlayer.h:106`：`CreateLevelSequencePlayer`。
- `Plugins/MovieScene/MovieRenderPipeline/Source/MovieRenderPipelineCore/Public/MoviePipelineQueueEngineSubsystem.h:28`：`UMoviePipelineQueueEngineSubsystem`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
