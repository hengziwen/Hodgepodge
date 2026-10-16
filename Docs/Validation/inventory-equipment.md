# 背包数据层与单武器接入验收

日期：2026-10-16。UE 5.5.4，Win64 Development。设计：[Inventory／Equipment v0.3](../Design/inventory-equipment-integration.md)。

## 实现范围

- 物品 Definition／Fragment／Instance、稳定身份和等级、整数物品状态、FastArray 容器与网络子对象。
- 服务器添加、移除、堆叠、容量预检、跨堆叠消耗、批量默认发放、快照和变更通知。
- PlayerState 单武器选择、旧装备身份／等级接续、兼容记录镜像、原 Equipment 授予／撤销链和 Pawn 更换恢复。
- PlayerController 请求门面、Server RPC、关联客户端结果及拥有者库存复制。观察者继续使用公开装备表现。
- 正式默认 PawnData／Experience 接线、长剑定义、普通堆叠测试物品。

不制作背包 UI、图标、拾取、存档、多槽快捷栏、新成长公式或视觉效果。不恢复暂缓的受击重做，不覆盖既有伤害跳字工作，不自动提交。

## 构建与静态检查

实际执行的常规构建命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
```

最终 Editor／Game 常规构建均通过，退出码均为 0。日志保存在 `Saved/Inventory/EditorBuildFinal.log`／`GameBuildFinal.log`，未使用 Live Coding 代替构建。

`git diff --check` 与 `Tools/Inventory/*.py` 的 Python AST 检查通过。源码检查覆盖服务器权限、输入数量溢出、同步回调重入、旧 Pawn 身份、来源设置时机和复制子对象注册／注销。文档／源路径检查另见收尾结果。

保留了现有构建警告，包括 UI 废弃字段、模块／插件声明及 include-order 提示，没有为消除无关警告更改插件或构建设置。

## 原生功能测试与回归

执行：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/Inventory/RegressionDelivery' '-abslog=E:/Project/Git/Hodgepodge/Saved/Inventory/RegressionDelivery.log'
```

最终交付复跑为 139 项通过（74 无警告、65 带警告），0 失败、0 未运行，进程退出码 0。结果以 `Saved/Inventory/RegressionDelivery/index.json` 和对应日志为准。警告不等于失败，也不宣称警告已清理。覆盖已有属性、装备、战斗请求、GAS 授予、敌人、死亡／受击、资源与跳字等已有用例。

新增 `Source/Hodgepodge/Private/Tests/HodgeInventoryTests.cpp` 的三组公开 API 用例：

- `Hodge.Inventory.StacksCapacityAndNotifications`：跨堆叠数量、容量满但可补堆叠、容量不足／消耗不足无部分修改、零／负数／溢出、抽象定义、通知重入拒绝、移除与 GC 回收。
- `Hodge.Inventory.AtomicDefaultsAndValidation`：默认批次坏配置不留下半成品、不提前标记初始化、修正后成功、重复初始化不再发物品、同类独立身份与等级、可装备物品不能堆叠。
- `Hodge.Inventory.EquipmentIdentityLifecycleAndGuards`：既有身份／等级接续、提前建立来源、阻止旧等级接口旁路修改、换装幂等、攻击限制、选择物品消耗保护、非法身份、同步通知解绑、解绑／重新绑定、更换 Pawn、旧 Pawn 迟到通知、显式卸装保留、删除选择物品及兼容镜像、组件注销／重新注册清理。

这些是需求场景测试，不是行覆盖率统计；不声明任意 GA、Instant GE 或蓝图副作用都可回滚。

## 正式资产、冒烟和 PIE 集成

配置工具为 `Tools/Inventory/setup_assets.py`。读取实际 Experience 蓝图 CDO 和原默认武器后制作数据资产，先备份旧包到 `Saved/Inventory/BeforeInventory/Content`。成功执行记录为 `Saved/Inventory/SetupAssetsFinal.log` 与 `asset-setup.json`；修改前的脚本适配失败保留在历史日志中，没有据此宣称资产完成。

实际保存的资产：

- `/Game/Main/Data/Inventory/ID_Sword`：映射原 `BP_Equipment_Sword`，最大堆叠 1。
- `/Game/Main/Data/Inventory/ID_TestMaterial`：测试用普通物品，最大堆叠 20，不发放进正式初始背包。
- `/Game/Main/Data/PawnData/DA_Dafult_PawnData`：启用库存，容量初值 40，默认一把 1 级长剑。
- `/Game/Main/Experiences/Exp_HodgeDefaultExperience`：原组件 Action 增加 PlayerState 的 InventoryManager／EquipmentSelection，其余 Action 保留。

通过保存后的冷启动加载和正式 Experience 运行检查确认配置，不以文件存在代替内部字段验证。测试地图为现有 `/Game/Main/Maps/L_EnemyAIValidation`，不另建背包演示页面。

PIE 集成命令为 `python Tools/Inventory/run_batch.py single listen listen-host`，进程退出码 0，最终报告分别为 `Saved/Inventory/<mode>-inventory-e2e.json`：

- 单人 Standalone PIE：21／21 条断言通过。
- Listen Server，远端客户端作为请求拥有者：23／23 条断言通过。
- Listen Server，主机作为请求拥有者：23／23 条断言通过。

三种模式均完成正式 Experience 初始化、实际装备／卸装及重生恢复，冒烟通过。共 67 条 PIE 集成断言通过；不将两玩家 PIE 结果等同于独立进程或正式专服验证。

每个模式检查默认发放一次、初始实际装备、服务器新增第二把独立武器、拥有者接收、真实装备 RPC 与关联结果、客户端选择收敛、未知身份拒绝、卸装保留物品、重装、同 PlayerState 重生后保留物品与选择并恢复装备。双玩家另外验证其他 PlayerState 可见但私有物品不可读，包含远端拥有者和 Listen 主机两种调用者。

测试适配说明：UE 5.5 `Actor.cpp::GetFunctionCallspace` 在 `GAllowActorScriptExecutionInEditor` 为真时强制 Local，Python 直接调用会误入本地 RPC 实现。因此 Editor-only `HodgeInventoryValidationLibrary::DispatchInventoryRequest` 仅在本地 Controller 的真实 PIE 发送作用域将该保护设为 false，调用原生请求门面后立即恢复；不直接调用服务器实现、不查找服务器替身执行、不绕过服务器持有权校验。Guid 按值比较，避免 Python 包装对象身份造成误报。早期脚本误报保留为 history 文件。

## 验证边界与收尾

按用户要求未执行截图、录屏、逐帧或其他视觉校验，全部编辑器／自动化运行使用 NullRHI。界面不在本轮交付范围。

未执行 Cook／打包、独立专服构建、独立进程联机、重连／更换 PlayerState 持久化、网络丢包或延迟压力测试、Iris 运行模式验证。组件注销／重新注册由原生测试覆盖，整套 Experience 跨模式持久策略未实现；不把这些算作通过。

收尾已通过文档链接／代码围栏、Python AST 和 `git diff --check` 检查。原生／构建结果与最终 PIE 报告已交叉核对，未使用早期失败的脚本报告替代最终结果。

测试结束后停止全部 PIE，恢复记录的性能参数，将地图返回本次编辑器冷启动时的 `/Game/CodexText/L_MainMenu`，无未保存资产，再正常关闭本轮启动的编辑器。PIE 选项复位为单人 Standalone；原始 PIE 选项未完整记录，不宣称精确恢复。读回配置确认两个组件均已保存，正式初始发放仍只有一把长剑；收尾记录为 `Saved/Inventory/editor-restored.json`。

运行日志和缓存留在 Saved，不作为源码提交。保留已有未提交代码、二进制资产与文档工作，仅交付本轮新增实现及必要接线；未自动提交或推送。
