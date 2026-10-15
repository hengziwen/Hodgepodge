# 渲染、Lumen 与性能：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先核实项目平台/RHI、DeviceProfile 和渲染设置，再解释 Lumen/Nanite/阴影/后处理的实际开关。
2. 本机 5.5 的 MegaLights 等实验功能以实际插件/CVar/项目配置为准，不套用 5.8 的成熟度和默认值。
3. 性能结论基于目标场景与硬件的 Unreal Insights/GPU 数据，分别定位游戏线程、渲染线程、GPU 和显存。
4. 任何画质或管线变更先说明影响；不为遵循上游 skill 自动开启 Substrate、升级引擎或更换插件。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Components/SkyAtmosphereComponent.h:50`：`USkyAtmosphereComponent`。
- `Source/Runtime/Engine/Classes/Components/VolumetricCloudComponent.h:32`：`UVolumetricCloudComponent`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
