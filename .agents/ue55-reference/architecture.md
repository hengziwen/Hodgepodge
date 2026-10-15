# 玩法架构方案：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 讨论具体方案前核查本机 Lyra 的同职责实现、调用者、配置和清理路径，再追踪 Hodgepodge 当前链路。
2. 复用 Experience/PawnData/GameFeature/GAS/Init State 的既有职责；成熟实现做最小适配，不因安装 skill 增加 Runtime 模块或通用框架。
3. 方案说明状态事实来源、修改入口、最终执行者、权限、初始化、取消清理、配置资产和验收用例。
4. Lyra 未实现、占位和与项目不兼容的部分给具体文件/函数证据；库存 AddEntry 的 unimplemented 不能当完整库存实现。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Runtime/ModularGameplay/Source/ModularGameplay/Public/Components/GameFrameworkInitStateInterface.h:54`：`ContinueInitStateChain`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/GameModes/LyraExperienceManagerComponent.cpp`。
- `Source/LyraGame/Character/LyraPawnExtensionComponent.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
