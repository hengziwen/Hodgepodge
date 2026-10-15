# Enhanced Input：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 沿用 HodgeInputConfig/HodgeInputComponent 与 Hero 初始化；定位 IA/IMC、InputTag、绑定入口和本地玩家子系统。
2. MappingContext 的添加与移除要配对，Pawn 重建或重复初始化不得重复绑定；输入完成与取消均需释放持续动作。
3. 按 Triggered/Started/Completed/Canceled 的实际语义选择回调，不把 IA 触发直接当作能力在服务器执行成功。
4. 5.8 的 Input Mode Filtering 不作为本机配置前提；重绑定先核实 EnhancedInputUserSettings 的 5.5 签名与项目设置。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h:22`：`UInputMappingContext`。
- `Plugins/EnhancedInput/Source/EnhancedInput/Public/UserSettings/EnhancedInputUserSettings.h:565`：`MapPlayerKey`。
- `Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h:430`：`BindAction`。

## Lyra 参考

参考根目录：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/Character/LyraHeroComponent.cpp`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
