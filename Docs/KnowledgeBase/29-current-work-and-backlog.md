# 当前任务与待办复核 · 更新至 2026-10-15

以当前源码、配置和验收报告为准。旧接通待办中的敌人未初始化、攻击未命中、Dash 未制作、UI 未接通、**伤害恒 0、Timeline 为主链路** 等描述均已过时。本记录区分执行、待验证与用户暂缓，不把所有规划功能算作本轮授权实现。

## 当前节点：共鸣／暗涌技能体系、技能配置工具与架构评审（2026-10-15）

负责人：当前 Codex 任务。资源：UE 5.5.4、现有 Melee 模板／Definition／AnimNotify／GAS／CameraMode／HUD。

- **P0 已完成：** 共鸣能量（`UHodgeResonanceSet`，资源在 PlayerState 的 ASC）、E 键共鸣技能（8 秒 CD）、R 键共鸣解放（满充消耗 / 12 秒 CD / 阶段回能 / 后半回能 / 前半段电影镜头 / 全程翼与金刚体固定 Box 命中）；暗流燃料与灭音、强化五段、破命共享 E 冷却、20% 湮灭增伤；能量环、技能图标、Layout 接入。
- **P0 已完成：** 技能配置工具与**可追溯验证记录** —— `Docs/Validation/` 新增/更新大量 `*-results.json`，结论可回溯到原始数据。
- **验证：** 共鸣解放原生 81（48 成功 + 33 警告、**0 失败**）+ 资产 35 + 四模式各 52 + 补充 17；暗涌原生 39 + 资产 30 + 核心四模式各 17（68）+ 命中 18 + E/R HUD 23。见 [`resonance-liberation-2026-10-15.md`](../Validation/resonance-liberation-2026-10-15.md) 与 [`havoc-dark-surge-2026-10-15.md`](../Validation/havoc-dark-surge-2026-10-15.md)。
- ⚠️ **参数是原型初值**（非逐帧数据）；`HodgeFillResonance` / `HodgeFillDarkFlow` 属**调试填充**（`*-debug-fill-2026-10-15.md`），**不计正式获取途径**。
- **待补：** 正式 Niagara／音效／震屏素材；E 换图标、重击与变奏暂缓。
- **新增评审（未实施）：** [架构审查与分阶段重构方案](../Design/architecture-refactor-review-2026-10-15.md) —— 只读审查，优先 R1 把命中检测从 Combat 抽离（`HodgeCombatComponentBase.cpp` 已 1525 行）。

## 历史节点：锁定镜头获取与屏幕区域跟随（v0.5，2026-10-13）

- **P0 已完成：** 沿现有 CameraMode 加入 LockAcquire／LockTracking，实际 POV 种子、胸口取景、0.35 秒缓入缓出、屏幕死区／软区与分轴响应。
- **P0 已完成：** 可调鼠标抑制／宽限／渐进恢复、近距防翻转、有限自动转速、真实视口投影和技能镜头中途返回连续性；角色朝向和移动仲裁保留。
- **验证：** 36 项原生回归、四模式镜头各 22 项及原锁定各 20 项；最终 Editor／Game 构建、实际最终复验和未验证项见[报告](../Validation/soft-lock-tracking-2026-10-13.md)。
- **后续：** 用 DA_HeroLockOn 调主观手感、物理鼠标 DPI 和复杂地形；候选选取仍按既有可见／视锥契约，没有扩展背后获取、攻击吸附或手柄。

## 历史节点：软锁定镜头修正（2026-10-13，已完成列明验收）

用户确认保留中键目标选择，只修镜头，不加入攻击自动索敌。[调研与方案](../Design/soft-lock-camera-research-2026-10-13.md)区分官方资料、玩家反馈与项目建议；[验收](../Validation/soft-lock-camera-2026-10-13.md)记录实际模式与 POV。

- **P0 已完成：** 移除 ±8° 硬限制和 0.12 秒视角缓存；手动环绕、停手宽限、构图死区、最短弧与有界自动修正，原始输入继续走 Controller。
- **P0 已完成：** 复用实际 CameraModeStack，修正 GetBlendInfo 栈顶查询；技能模式优先并接管输入，返回／解除继承当前画面。混合层共享本帧角位移，避免输入被权重削弱或旧输入在返回时重复应用。
- **验证已完成：** 31 项原生回归、四模式镜头专项各 24 项、锁定／Dash／Sprint 回归各 20 项、10 项资产核对；Editor／Game 常规构建通过。
- **P1 仍待处理：** 物理鼠标 DPI／主观手感、复杂墙角／高差／实际技能镜头组合、换 Pawn／热卸载／丢包与发布验证。没有把这些专项写成已通过。

## 历史节点：锁定首版（2026-10-12，已完成当时列明验收）

负责人：当前 Codex 任务。资源：UE 5.5.4、现有 Experience／PawnData／GAS／朝向请求／CameraMode／Indicator、正式 Hero 与 Guardian、本地编辑器 gateway。未指定整体截止日，以本阶段交付为节点。

- **P0 已完成：** 中键获取／解除与鼠标左右甩动切换；组件持久状态＋短 LocalPredicted GA／TargetData，服务器资格与版本确认、拒绝恢复、旧请求隔离和有限超时。
- **P0 已完成：** PawnData／Init State 初始化、来源句柄清理，实际 HUD 投影层／标记、Guardian 胸口锚点；修复初始化屏障互锁和中键真实映射。
- **P1 已完成：** 普通 Controller＋ReservedStrafe、Dash／Sprint 暂让与恢复；目标保留、锁定镜头与统一控制旋转；攻击起手记录确认目标，不开启吸附。
- **验收已完成：** Editor／Game 构建退出 0，29 项原生回归，10 项资产检查，single／listen／dedicated PIE／listen-lag 各 20 项检查通过。最终构建后再次单人冷启动；原始帧数据／截图／构建日志和恢复结果可追踪。
- **交付已完成：** [设计](../Design/lock-on-system.md)、[配置手册](../Guides/lock-on-configuration.md)、[实施报告与临时选择清单](../Validation/lock-on-implementation-2026-10-12.md)。地图／性能恢复，无 PIE 或未保存包；原播放设置未完整记录，恢复为单人基线。
- **P1 待处理：** 1～3 人日手感与复杂镜头／遮挡／攻击组合验证，2～4 人日真实换 Pawn／热卸载／丢包专项，3～7 人日独立进程／Cook／打包。先处理前两项，发布验证沿用下文；不能把当前 PIE 通过写成这些项目也已完成。

逐项步骤／验收见锁定报告第 7 节。当前没有已知阻断首版获取、切换、Dash／Sprint 恢复的未修复失败；待验证项保留状态，不把它们伪装成闭环。

## 历史节点：参考 HUD（2026-10-11，已完成实施与运行验收）

负责人：当前 Codex 任务。资源：UE 5.5.4、本地编辑器 gateway、现有 UMG 作者工具与 UI 数据源、正式 Experience、用户 [参考图](../References/HUD/main-hud-reference.png)。

执行顺序与状态：

1. 保存原图、备份受影响资产：已完成，备份在 Saved/HUDReference/Before。
2. 底部居中血条、真实生命数值和等级，右上角终焉按钮：正式资产已保存。
3. 按钮复用所属菜单生命周期，打开可关闭占位页面：已完成。
4. 常规 Editor／Game 构建、蓝图编译与制作：通过。软类引用转换、重复变量、Slot 对齐与跳过保存问题均已修正，失败记录保留。
5. 单人／Listen Server：最终冷启动各 12/12，通过数据、按钮事件链／返回、输入隔离、HUD 重建及运行几何断言；实际 Slate 截图已检查。桌面鼠标坐标与手柄体验未验收。
6. 实际结果见 [验收报告](../Validation/main-hud-reference-2026-10-11.md)，地图／设置已恢复，无未保存包。

验收：正式 Experience 只显示所需两个 HUD 元素；血条与当前玩家 Health／MaxHealth 一致；按钮悬停／点击有效，关闭恢复玩法输入；远端不读取其他玩家生命；参考图与配置路径可追踪。终焉业务暂未实现，入口只显示“敬请期待”。

阶段已交付，不再列为待实施。后续人工体验若发现问题，以报告路径和场景继续修正；未将 Cook／打包或桌面工具不可用的验证范围算作通过。制作脚本幂等，避免重复按钮／Graph。

## 已完成或已有验证

- Guardian 初始化、追击、GAS 攻击、受击与死亡：见 [实施报告](../Validation/enemy-ai-implementation-2026-10-11.md)。用户已修复装备版行走抽搐，不重复处理。
- 玩家／AI 共享战斗请求：见 [实施报告](../Validation/combat-action-requests-2026-10-11.md)。
- 受击、旋转、Dash／Sprint、Pivot 与同步：已有各自验证，不重复开发；体力按用户要求暂时移除。
- UI 根布局、属性数据源与 LocalPlayer 隔离：已有 [UMG 验证](../Validation/umg-authoring-refactor-2026-10-08.md)；参考 HUD 已交付。**方案明确不引入 CommonGame／CommonUser／ModularGameplayActors**。
- 装备导入／材质修复已完成；旧 Pover 翅膀仍按[历史暂停](../Validation/pover-wing-disabled-2026-10-11.md)处理。后续授权的新 Anzhu 翅膀已完成[攻击限定表现](../Validation/attack-wing-presentation-2026-10-12.md)、[淡出](../Validation/wing-fade-2026-10-12.md)、[Box 判定](../Validation/wing-box-hit-2026-10-12.md) 与[通知蒙太奇改造](../Validation/wing-notify-montage-2026-10-14.md)，不能再把所有翅膀写成禁用。
- 武器手持通知新增[回背／在手部淡出选择](../Validation/weapon-notify-end-behavior-2026-10-12.md)，默认保留旧策略，具体攻击由通知配置。
- **独立技能移动取消**（非连段）已修复：见 [设计](../Design/definition-move-cancel.md) 与[验证](../Validation/definition-move-cancel-2026-10-14.md)（动作 12 + 连段 4、PIE 151 项）。
- **攻击前摇 Motion Warping**：见[设计](../Design/attack-facing-motion-warp.md) 与[验证](../Validation/attack-facing-motion-warp-2026-10-13.md)；首版仅 `DA_Attack_1` 开启。
- **伤害倍率恒 0 已修复**：`HodgeDamageExecution.cpp:186` 按 `FHodgeDamageRules::CanDamage` 取倍率，判敌我启用中。
- **Timeline 已退役**：代码/资产迁入 `Archive/Timeline/`，攻击窗口改由 Montage 动画通知驱动，见 [30 号记录](30-update-2026-10-15.md)。
- **`GetBlendInfo` 栈顶查询缺陷已修复**（2026-10-13 随软锁定镜头一并处理）。

历史通过范围不自动覆盖新改动，也不等于 Cook／独立进程通过。

## 后续优先级与验收

以下是后续计划，不是本轮自动开工清单；时间为粗略人日估算。

**P1：结构重构阶段 0～1，约 14～24 人日（分阶段）。** 按 [架构审查与分阶段重构方案](../Design/architecture-refactor-review-2026-10-15.md) 推进；先做 R1（命中检测从 `CombatComponent` 抽成 `UHodgeHitDetectionComponent`），再按 R2 / R3 / R5 / R7 顺序。验收：行为不变（既有验证报告逐项回归）、公共层不再吸收角色专属规则。风险：抽取期间命中语义漂移，必须逐项回归而非只看编译通过。

**P1：玩家生命周期与装备／能力归属，约 3～6 人日。** 追踪 PawnData AbilitySets、默认装备和属性协调器 → 明确来源句柄与重生保留策略 → 补换 Pawn／解绑测试 → 修复实际失败 → 双客户端回归。资源是现有属性、装备、战斗、UI 测试。验收：不重复授予／装备、不意外回血、旧回调不影响新 Pawn、UI 重绑正确。风险是误撤销跨 Pawn 保留能力，以来源记录与场景回归控制。

**P1：独立进程、Cook／打包与专服目标，约 3～7 人日。** Game Cook／打包 → 资产与 Runtime 依赖修正 → 独立进程双玩家 → 延迟／丢包 → 按发布需求评估 Server Target 和专服骨骼／通知策略。当前无 Server 专用 Target。每种模式单独验收，不将 PIE 专服当独立专服通过；工具链阻塞明确记录，不擅自升级引擎。

**P2：GameplayCue 生命周期与命中表现，约 2～4 人日。** 移动 Cue 目录已有；Feature Observer、注销与启动预加载未完整接入。确定目录／加载策略 → 一个命中 Cue → 首次播放及网络观察者 → 注册／撤销／再激活。需要特效／音效资产。验收：无残留路径，重复加载可用，表现不承担伤害权威。

**P2：GameFeature 必需能力扩展，约 3～5 人日。** 当前 Guardian 不依赖它，按实际需求启动。明确软资源预加载 → Actor 与配置条目授予记录 → 成功／失败／副作用确认 → 停用撤销及再激活。验收：不丢条目、不重复授予、资源缺失不伪装成功；不可逆副作用停止并诊断。

**P2：阵营与 AI 边界／规模，约 3～6 人日。** 先固定友伤／自伤／无阵营规则 → 目标销毁与不可达导航 → 受击／死亡同帧 → 多怪性能采样。验收：无旧请求回调、错误友伤或无限导航等待。复杂仇恨／EQS 随怪物需求加入。

**P3：加载屏、战斗消息、交互、库存 UI 和完整前端。** 加载屏缺执行器，消息 Router 未接，交互是占位。先选择具体场景，再定义服务器入口、数据、UI 和验收；未确定场景时不估算总时长或批量迁移所有 Lyra 子系统。

## 用户暂缓与完成边界

- [Inventory 集成](../Design/inventory-equipment-integration.md)：草案完成，用户明确只设计，不实现；首版估算 6～10 人日，编码前确认归属和旧记录迁移。C++ 当前**无任何 `UHodgeInventory*`**。
- 攻击吸附暂不启用（[草案](../Design/melee-attack-assist.md) 接口尚未创建）；旧翅膀禁用不覆盖后续新 Anzhu 攻击限定方案；Guardian 行走问题按用户已修复闭环，本轮不改插槽。
- 下蹲（Crouch）绑定仍临时屏蔽；**体力系统按用户要求暂时移除**（不列为缺口）。
- 共鸣／暗涌的 E 换图标、重击与变奏暂缓；正式 Niagara／音效／震屏素材待补。
- 未指定全项目截止日。其他项目按后续需求逐项推进；只有实际执行并完成对应验收才标记闭环，规划与待验证保持待办。
