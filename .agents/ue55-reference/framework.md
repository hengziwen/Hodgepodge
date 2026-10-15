# Gameplay Framework：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 沿 Experience→GameMode/PawnData→PlayerState/PawnExtension→Hero 追踪，不套用模板角色 BeginPlay 全初始化。
2. 区分仅服务器存在的 GameMode、复制 GameState/PlayerState、本地 LocalPlayer 和各端 Controller/Pawn 的生命周期。
3. 玩家 ASC Owner 保持 PlayerState；重生/Travel 的持久状态与 Pawn 临时表现分离，服务器与客户端入口同时核实。
4. 找到 Lyra 同职责实现后复用机制并记录差异；不把空覆盖、旧注释或同名类当作有效调用链。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/GameFramework/GameModeBase.h:440`：`RestartPlayer`。
- `Source/Runtime/Engine/Classes/GameFramework/PlayerState.h:34`：`APlayerState`。
- `Source/Runtime/Engine/Classes/GameFramework/Pawn.h:262`：`OnRep_PlayerState`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/Player/LyraPlayerState.cpp`。
- `Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
