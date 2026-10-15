# UDS/UDW 商业资产接入核查：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 这些条目保留 UDS/UDW 的专项知识，但 skill 不包含商业资产；先搜索项目 Content/Plugins 和真实蓝图类、版本、属性。
2. 当前未核实 UDS/UDW 资产可用，禁止按上游 5.8 截图或属性名要求用户配置不存在的字段，也不自动购买/安装。
3. 仅有引擎 SkyAtmosphere/VolumetricCloud 不等于 UDS/UDW；可用引擎原生能力说明环境机制，但不得伪装成插件功能。
4. 确认已有资产后读取时间/天气/材质/粒子/序列对应 API，成对维护状态和事件，按目标平台验证效果与成本。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Components/SkyAtmosphereComponent.h:50`：`USkyAtmosphereComponent`。
- `Source/Runtime/Engine/Classes/Components/VolumetricCloudComponent.h:32`：`UVolumetricCloudComponent`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
