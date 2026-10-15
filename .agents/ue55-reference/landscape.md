# Landscape、RVT 与植被：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先读取地形尺寸、EditLayers、材质层和 RVT/Grass 配置，明确改动资产及可恢复范围。
2. 自动地形、斜率混合和纹理平铺参数沿用既有材质实例，WorldPosition/UV 单位保持一致。
3. 核实 5.5 Landscape/RVT 实际节点及编辑器支持，不能直接套用 5.8 Nanite/PCG 组合能力。
4. 验证编辑器表现、碰撞、流送和目标平台开销；安装知识 skill 不表示 UDS/UDW 商业资产已存在。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Landscape/Classes/Landscape.h:256`：`ALandscape`。
- `Source/Runtime/Engine/Classes/VT/RuntimeVirtualTexture.h:19`：`URuntimeVirtualTexture`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
