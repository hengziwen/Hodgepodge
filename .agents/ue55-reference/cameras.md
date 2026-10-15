# 相机与 CameraMode：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本项目沿用 HodgeCameraComponent/CameraMode，参考 LyraCameraComponent.GetCameraView 与 CameraModeStack.EvaluateStack 的统一求值入口。
2. 模式提交视图请求，由现有相机栈统一应用；技能、锁定和过场的相机占用在结束/取消时按执行身份释放。
3. 5.5 已有实验性 GameplayCameras 插件不等于项目已启用，更不等于拥有 5.8 的同名接口；不为复用上游示例替换项目相机链路。
4. 相机位置、控制器旋转和 Pawn 朝向区分职责，验证重生、切换目标、联机拥有者与恢复默认模式。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Camera/CameraComponent.h:258`：`GetCameraView`。
- `Source/Runtime/Engine/Classes/Camera/PlayerCameraManager.h:191`：`APlayerCameraManager`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。

## 项目切入与适配差异

- Lyra `Source/LyraGame/Camera/LyraCameraComponent.cpp` 的 GetCameraView→CameraModeStack.EvaluateStack→FMinimalViewInfo 是已核查的统一应用机制。
- 本项目 `Private/Component/HodgeHeroComponent.cpp` 的 DetermineCameraMode 委托与 `Private/Camera/HodgeCameraComponent.cpp` 对接，技能只提交模式请求；不要另加 Tick 直接覆盖最终视图。
