# 实验性 Mover：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 本机 Mover 位于 Plugins/Experimental/Mover；项目当前使用 CharacterMovement，不将 skill 安装解释为启用或迁移 Mover。
2. 只有任务明确需要 Mover 时才检查插件版本、模块、输入生产、同步状态和后端；用本机 MoverComponent 声明重建调用链。
3. 5.8 的 movement modifiers、后端和同步 API 不能直接视为 5.5 可用；缺失功能沿用现有 CMC/RootMotion 路径或说明缺口。
4. 改变网络运动框架前需要完整评估 GAS、根运动、预测和取消，不能通过改引擎版本来完成适配。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Plugins/Experimental/Mover/Source/Mover/Public/MoverComponent.h:62`：`UMoverComponent`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
