# Hodge Ability Definition 编辑器（第一版）

更新：2026-09-29。布局参考用户提供的技能编辑器截图，使用 UE 原生 Slate 停靠面板。

## 使用

双击 `/Game/CodexText/DefinitionCombo/DA_Attack_5`（或任意 HodgeAbilityDefinition）打开。

- 左侧 **Ability Definition**：编辑 AbilityTag、AbilityClass、Montage、倍率、混合和 Timeline 引用。
- 中间 **Preview**：Play / Pause、Step（源动画 1/30 秒）、Reset（回到 0 秒，不重置配置）、时间输入和预览 Mesh。
- 中间下方 **Timeline**：Add Point / Add Window / Duplicate / Delete；拖动条目移动时间、拖两端调整窗口。绿色表示窗口，黄色表示选中，红色提示时间越界，竖线表示播放位置。条内显示 EventID。
- 右侧 **Ability Tasks**：直接编辑 Timeline 完整 Events 数组；**Selected Event** 编辑时间轴选中的单个事件；**Validation** 查看配置错误、当前窗口数及最近播放事件。
- Ctrl+滚轮缩放，Shift+滚轮平移；Snap 30 fps 使用固定 30 fps 源时间网格；Delete 删除选中条目，空格播放/暂停。Ctrl+Z / Ctrl+Y 撤销、重做。
- 点击工具栏 Save 保存 Definition 与引用的 Timeline。当前 Definition 校验失败时阻止这个保存入口，并在 Validation 显示原因；编辑器外的 Save All 不受此入口拦截。

Timeline 总长自动取 Definition Montage 的源动画长度。PlayRate 和 Montage.RateScale 只改变预览推进速度，不缩放配置中的事件时间；不需要另外填写 Duration。

多个 Definition 共用一个 Timeline 时，编辑会影响共用者。左侧 **Create / Copy Timeline Config** 创建专属副本并替换当前 Definition 引用；新资产尚需保存。Duplicate 事件保留时间和 Tag，仅生成新 EventID，因此可能产生同标签重叠，需调整后通过校验。

## 实现边界

`HodgeAbilityEditor` 是 Editor-only 工具模块，业务仍在原有 `Hodgepodge` Runtime 模块。Game Target 不依赖 Slate 编辑器模块。

Runtime 和编辑器使用 `FHodgeTimelineEvaluator`：

- `EvaluateRange` 收集 `(PreviousTime, CurrentTime]` 越过的边界；同刻顺序为 WindowEnd、WindowBegin、Point，再按 Priority 和原数组索引排序。
- `Collect(..., bInitialize=true)` 恢复起始时刻的窗口并处理恰好位于起点的 Point。
- `EvaluateAt` 重建任意时刻的活动窗口，拖动或倒退不补发 Point，不执行 GE、GameplayEvent、动画 Notify 或角色位移。
- AbilityTask 继续负责运行时时钟、网络执行策略、窗口归属和 GAS 副作用；Evaluator 不持有 ASC。
- 窗口终点超过 Montage 长度且不超过 0.001 秒时归一到末尾。Point / 窗口起点严格要求落在合法时间范围，修复 REVIEW-02 的不可达 Point。

动画预览采用 `UAnimPreviewInstance` 显式定位，先初始化暂停且权重为 1 的 Montage 预览实例。停止/拖动关闭预览视口 realtime；不修改全局渲染或性能配置。

第一版仅支持现有单 Section、连续正向技能。预览展示源动画姿势与逻辑时间，不模拟 GAS、混合进出、网络连招、攻击判定和位移。截图中的 Authored Displacement 等字段未接入当前系统，因此没有添加空字段。窗口显示名暂用 EventID；每个条目一行，未做 Points 合并轨。

## 验证

常规 Editor / Game Win64 Development 构建通过；不是 Live Coding。`Automation RunTests Hodge.` 包含 9 项测试，覆盖共享 Evaluator、Definition/Combo 校验、Task 清理以及编辑器添加、复制、删除、Undo/Redo、播放、姿势定位和窗口恢复。

编辑器工作流测试使用现有 DA_Attack_5 的内存副本，不保存原资产。已实际打开 DA_Attack_5 并通过 MCP 全窗口截图检查布局。首次测试发现的多资产标题断言和预览姿势不变问题已修复。

本次未执行完整 PIE、联机、Cook/打包，也未用鼠标逐一回归所有拖拽和保存后重启流程。已有 REVIEW-01 / REVIEW-03 仍按原决定暂缓。

测试期间临时关闭后台 CPU 节流，60 秒回调恢复读取到的原值 True；不调用 SaveConfig，不持久修改这项设置。

实际执行命令（退出码均为 0）：

```powershell
& 'D:/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=D:/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64
& 'D:/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=D:/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64
git diff --check
```

MCP 在编辑器执行 `Automation RunTests Hodge.`，最终 2026-09-29 14:35 的 9 项均成功。`Saved/Logs/Hodgepodge.log` 为当前会话原始日志，重启会轮转；截图位于 `Saved/Screenshots/hodge_ability_editor.png`。没有自动提交源码。

## 指示器拖动刷新修复（2026-09-29）

骨骼 CPU 姿势定位正确不等于渲染画面已刷新：自定义预览世界不 Tick，原先仅 MarkRenderDynamicDataDirty 会留下未提交的帧末更新。SetTime 现更新包围盒、标记变换和骨骼渲染数据，并调用预览世界 SendAllEndOfFrameUpdates；仍不 Tick 游戏逻辑、不触发 Notify、不常驻开启 realtime。

本轮常规 Editor Win64 Development 构建通过；只修改 Editor 代码，未重复 Game 构建。编辑器工作流自动化复查，真实鼠标连续拖动因桌面控制工具启动失败未完成。编译前保存了用户已有未保存修改的 `/Game/CodexText/BasicAttack/DA_Attack05_Timeline`。

后续修正：单独提交渲染数据仍不足。`SEditorViewport::Invalidate` 只维持 Slate active timer；须同时 `FEditorViewportClient::Invalidate(false, false)` 才会经 `FSceneViewport::InvalidateDisplay` 设置 `bNeedsRedraw`。每次 Seek 现同时请求视口重绘。工作流测试补充连续前进/后退 Seek，每次先清除重绘标志，再断言重新设置；旧实现会失败，不再仅检查 CPU 姿势。
