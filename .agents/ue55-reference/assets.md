# 数据资产与资源加载：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 可调参数沿用项目已有 PawnData/Definition/Profile 资产，字段和路径以当前头文件及资产读取结果为准。
2. 区分软路径、异步加载、实际驻留和 GC 持有；FStreamableHandle 与取消/释放的所有者保持明确。
3. 回调检查对象和本次加载/Pawn 身份；无资源、加载失败与等待中的状态不能统一当作成功。
4. AssetManager 的 PrimaryAssetType、扫描、Bundle 和 Cook 规则分别核实；磁盘存在 uasset 不证明内部配置有效或已包含在包中。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Engine/Classes/Engine/AssetManager.h:244`：`LoadPrimaryAsset`。
- `Source/Runtime/Engine/Classes/Engine/StreamableManager.h:511`：`RequestAsyncLoad`。
- `Source/Runtime/Engine/Classes/Engine/DataAsset.h:46`：`UPrimaryDataAsset`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
