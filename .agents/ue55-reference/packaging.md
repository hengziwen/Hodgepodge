# Cook、打包与部署：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先核实已有 Target、平台工具链、地图和资源扫描规则；本项目无 Server Target，不生成臆造专服构建命令。
2. Editor/Game 编译与 Cook、Stage、Package 分别验收，UAT BuildCookRun 参数以本机脚本和目标需求为准。
3. 软引用资源、PrimaryAssetLabel、插件内容和平台条件需要检查 Cook 可达性，不能用编辑器加载成功替代。
4. 本次技能安装不触发打包或发布；后续按任务明确输出目录，保护用户资产和已有构建产物。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Developer/DeveloperToolSettings/Classes/Settings/ProjectPackagingSettings.h:177`：`UProjectPackagingSettings`。
- `Source/Programs/UnrealBuildTool/Configuration/TargetRules.cs:3150`：`TargetType`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
