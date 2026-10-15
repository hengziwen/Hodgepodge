# 存档与序列化：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 明确持久数据 schema、稳定标识、版本号与迁移入口，不直接序列化临时 UObject 指针或 Pawn 地址。
2. SaveGame 标记不表示自动保存 Actor；采集/恢复流程、FArchive 标志和资源加载需明确实现。
3. 异步保存和读取在回调中检查槽位/用户/请求身份，失败保留可诊断结果，禁止迟到读取覆盖当前游戏。
4. 多人存档由服务器维护事实，客户端显示和平台存储权限分开；保存成功与加载后玩法正确分别验证。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/GameFramework/SaveGame.h:23`：`USaveGame`。
- `Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h:1156`：`AsyncSaveGameToSlot`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
