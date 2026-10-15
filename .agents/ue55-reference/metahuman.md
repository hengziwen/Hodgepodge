# MetaHuman 可用性与旧版流程：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本机未找到 5.8 的 MetaHumanCharacter/Identity 头文件；当前 skill 提供 5.5 前置核查，不宣称安装了 Creator/Animator。
2. 5.5 角色使用已有导入资产/Bridge 工作流；需要云端 Creator 或 Animator 时先核实账号、插件版本和本地资产，不自动安装或升级。
3. LiveLink 可复用本机 LiveLinkComponentController，但源、Subject、面部 AnimBP、骨架和录制工具必须逐项确认。
4. 不调用上游 UMetaHumanCharacterEditorSubsystem 或假定 webcam/audio 一键生成功能在本机存在；缺少依赖时给出具体缺口。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Animation/LiveLink/Source/LiveLinkComponents/Public/LiveLinkComponentController.h:25`：`ULiveLinkComponentController`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
