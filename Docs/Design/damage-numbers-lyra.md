# 本地伤害跳字：Lyra 最小适配

## 当前交付与暂缓决定（2026-10-17）

用户决定先提交当前已完成成果，剩余工作暂缓，不继续扩展实现。保留现有 Lyra NiagaraText 首版、实际生命损失口径和攻击者本地显示规则；既有验证结果见 [验收记录](../Validation/damage-numbers-lyra.md)，本次提交整理不代表重新执行构建或游戏测试。

- **外观扩展暂缓：** 暴击差异化、按伤害元素配色、字体／材质／轨迹调整及《鸣潮》视觉拟合。等待角色属性与正式暴击／元素规则确定，并由用户明确安排恢复；不伪造暴击结算。
- **专项验证暂缓：** 分屏渲染隔离、完整重生／重连、延迟与丢包压力、Cook／打包及独立服务器构建。未执行项继续保留为验证缺口，不宣称通过，也不在本轮继续补测。
- **恢复标准：** 按届时源码、资产和需求重新确认范围，补齐相关测试与必要的视觉证据后再更新验收结论；当前可用首版不因暂缓而停用。

## 范围与接口

以 UE 5.5.4、本地 Lyra 和 Hodge 当前 main 为基线。伤害跳字只显示攻击者自己的输出，不改变 GAS 伤害计算、属性复制、受击或奖励。受击系统封存分支不参与本次开发。

唯一正式输入是 `UHodgeHealthSet::OnDamageAccepted`。数值为夹取后的实际生命损失：剩余 10 HP、结算 100，显示 10。保留浮点原值，Niagara 整数字形边界四舍五入；正的小额伤害至少显示 1。治疗、免疫、零伤害、直接修改 Health 不产生输出跳字。最后一击在死亡通知前提取完整数值和位置。

链路：`HealthSet → HealthComponent → 攻击者 PlayerController → Client RPC → HodgeNumberPopComponent_NiagaraText → NS_DamageNumbers`。

- HealthComponent 与既有 ASC 监听一起绑定、解绑，要求 ASC 当前 Avatar 是组件所属目标。
- 来源优先使用原始 Instigator ASC 的 ActorInfo.PlayerController，兼容 PlayerState 持有 ASC；不使用玩家索引，不把 AI 归给本地玩家。
- PlayerController 分配每个接收者独立的正数 EventId，同帧事件入队后由 Timer 阶段经 Unreliable Client RPC 分批发送，每次处理最多一批 64 笔，余量后续发送；只合并传输，不合并每刀结果。当前不显示自伤。
- 本地组件保留最近 128 个非零身份，抑制重复；同额的不同事件不合并。零身份仅供显式本地样式预览。
- 服务器单帧待发送队列最多 256 笔，极端压力下保留最新事件。Controller EndPlay 清除发送 Timer 和待发队列；事件计数表示已接受并入队的请求数。
- 本地 Controller 复用一个 NiagaraComponent，Pawn 更换不清空尾部数字；Controller EndPlay 销毁。样式软定位、强引用保活，远端和专服不加载表现依赖。
- 正式暴击始终 false，未增加暴击公式。SourceTags/TargetTags 保留接口，原 Lyra 渲染器暂不按元素换色。
- 可靠的非穿透 Blocking Hit 使用接触点；Overlap、ConfirmedTarget 和无 HitResult 回退目标碰撞包围中心。事件不保留目标引用，目标销毁不移到原点。

## 直接复用的 Lyra 部分

参考项目：`E:/Project/UProject/study/Epic/LyraStarterGame`。

- `Source/LyraGame/Feedback/NumberPops/LyraNumberPopComponent.h`：ControllerComponent 门面与请求结构。
- `LyraNumberPopComponent_NiagaraText.cpp::AddNumberPop`：单 NiagaraComponent 复用，XYZ 世界位置加 W 数值的 Vector4 数组，负 W 为暴击表现。
- `LyraDamagePopStyleNiagara.h` 和 `Content/Effects/Blueprints/Damage_BasicNiagaraStyle`：样式与 `DamageInfo` 数组契约。
- Lyra `GCNL_Character_DamageTaken`：本地攻击者显示的参考；Hodge 采用实际 HP 损失事件和拥有者 RPC，避免直接把 Cue 原始伤害当作实际损失。

迁入六个原资源，保持 `/Game/Effects/...` 虚拟路径：NS_DamageNumbers、MI_numba、MI_numberSingleCard、M_numba、NM_BPSystemEvent、numbers_color。不复制依赖 Lyra 原生类的样式资产或蓝图，改用 Hodge DataAsset。依赖已有 Niagara、ModularGameplay、GAS 模块，不引入 UI 框架或额外业务模块。

`Tools/DamageNumbers/migrate_lyra_assets.py` 拒绝覆盖不同字节的已有资产；SHA256 清单位于 `Saved/DamageNumbers/lyra-migration.json`。`setup_style.py` 在编辑器核实真实 AssetRegistry 依赖并创建样式，不重存迁入资源。

## 配置与边界

样式：`/Game/Main/UI/Feedback/DA_DamageNumberStyle`。

- TextNiagara：迁入的 `/Game/Effects/Particles/Impacts/NS_DamageNumbers`。
- NiagaraArrayName：`DamageInfo`，与原资源一致。
- MaxBufferedRequests：默认 64，合法 1～512；限制一次仿真前的队列，超限保留最新事件，不改变实际伤害。

字体图集、材质、颜色、运动和寿命目前保留 Lyra 原资源。首版交付目标是确认本地真实伤害显示，并非已经还原《鸣潮》；元素配色、方向与轨迹美术拟合单独验收。Niagara 使用浮点数传输字形数值，极大整数存在浮点精度限制。

Unreliable 在丢包时允许少显示纯表现事件。当前没有跨网络补发、预测合并或观战显示。分屏共用世界的渲染隔离需单独验证，不等同于两个独立客户端的接收隔离。

## 验证标准

原生测试覆盖真实 GE 过量伤害、最后一击、免疫、治疗/直接扣血排除、小数、事件去重、PlayerState ASC、解绑重绑和旧 Avatar。Editor/Game 常规构建分别执行；PIE 用正式攻击 Notify 产生伤害，对比服务器实际损失、发送计数、各客户端接收数和 Niagara 输出。

至少检查普通攻击、五段顺序 01→02→03→05→04、E/R、多目标、Listen 主机、远端拥有者和 Dedicated PIE。视觉需查看真实画面，不能把收到 RPC 或编译通过当作数字可见。实际执行结果及限制见 [验证记录](../Validation/damage-numbers-lyra.md)。

## 外观扩展评估：暂缓，等待角色属性设计（2026-10-16）

**用户决定：先记录，不实施。** 角色各项属性尚未开始完整构筑；当前已有生命、资源及部分伤害元素链路，不等于完整的角色属性、暴击和元素规则已经确定。现阶段保留已完成的 Lyra 跳字首版，不新增暴击公式、元素属性、样式字段或 Niagara/材质改造。该暂缓仅针对后续外观扩展，不代表停用现有跳字。

### 已核查的基础与缺口

- `FHodgeNumberPopRequest` 已有 `bIsCriticalDamage`，NiagaraText 沿用 Lyra 的负 W 表示暴击协议；正式伤害路径仍为 false，本地样式预览不代表存在实战暴击。
- Melee 已把 DamageElement 写入 GE 动态资产标签，HealthComponent 将其传入 SourceTags；已有湮灭标签 `GameplayEffect.DamageElement.Havoc`。当前 Niagara 后端没有消费这些标签进行元素配色。
- 现有 `DA_DamageNumberStyle` 只有系统引用、数组名称与队列上限，尚无颜色映射、暴击尺寸倍率等建议字段。
- Lyra 的 `LyraNumberPopComponent_MeshText.cpp::DetermineColor` 与 `LyraDamagePopStyle.h` 已有标签匹配、普通/暴击颜色，MeshText 另有暴击尺寸倍率。可借鉴配置思路；它匹配 TargetTags，不能原样当成本项目伤害元素选择逻辑，也不代表 Niagara 后端已具备同样功能。

### 保留的候选方向，均未实施

- 继续复用现有服务器实际扣血、拥有者路由和本地 Niagara，不另建伤害计算或渲染后端。颜色依据本次伤害的元素快照，不依据技能按键、目标属性或角色当前状态猜测。
- 元素决定基础色相，真实暴击结果决定尺寸、亮度与弹出强调；暴击不必统一变金色。保留默认样式和未知元素回退，不提前铺设尚不存在的全部元素。
- 优先扩展现有样式资产，采用少量元素配色映射与共享表现参数，避免每个“元素×暴击×技能”组合各建一份完整配置。
- 同一个共享 Niagara 中，不同数字应在生成时保存各自的颜色/样式；如使用额外颜色或样式数组，必须与伤害事件同步截断、消费和清空，避免串色、错配。具体协议在恢复开发时重新核查。
- 先确认蓝紫渐变来自图集还是材质，再决定是否需要中性字形遮罩、粒子颜色接线或其他资产修改。本轮未在编辑器逐节点确认现有 Niagara/材质参数，不能承诺已有可直接调整的颜色入口。

### 恢复开发的前置条件与验收

先明确角色属性、正式伤害元素分类、暴击的判定与结算事实来源，再由用户安排恢复该扩展；属性设计完成不自动授权实施。恢复时重新核查当前代码、正式攻击资产及 Lyra 对应实现，确定本次伤害的元素唯一性与缺省规则，真实暴击由服务器结算提供，不能按数值大小推测。

保持“10 HP 承受 100 只显示 10”的数值口径；若该次伤害将来确为暴击，显示带暴击样式的 10。届时重点验收普通/暴击与不同元素同屏共存、未知元素回退、连续高频命中和本地玩家隔离。本节是讨论记录，不新增构建、PIE 或美术验收结论。
