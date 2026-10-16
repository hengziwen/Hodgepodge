# 伤害跳字配置与使用

实现依据与边界见 [Lyra 适配方案](../Design/damage-numbers-lyra.md)，实际验证结果见 [验证记录](../Validation/damage-numbers-lyra.md)。适用于 UE 5.5.4 当前实现。

## 默认接入

`AHodgePlayerController.NumberPopComponent` 是默认子对象，使用 `UHodgeNumberPopComponent_NiagaraText`。正式技能只要经既有 Damage 元属性实际扣血，就会自动进入同一路径；普攻、E/R 不需要各写一份创建数字逻辑。

只显示当前攻击者的输出。服务器确认后发给其拥有者，其他玩家不接收；AI、自伤、零实际伤害、治疗、直接设置 Health 不在首版范围。目标最后 10 HP 承受 100 伤害显示 10。小数保留在请求中，Lyra 字形按四舍五入显示整数，正的小额伤害至少显示 1。

## 现有可调字段

Controller 跳字组件的 `Style` 默认为 `/Game/Main/UI/Feedback/DA_DamageNumberStyle`，本地进入游戏时软加载。该资产有三个字段：

- `TextNiagara`：默认 `/Game/Effects/Particles/Impacts/NS_DamageNumbers`。更换系统必须继续支持 XYZ 世界位置、W 数值、负 W 暴击预览的数组协议。
- `NiagaraArrayName`：默认 `DamageInfo`，对应原 Niagara 的 `User.DamageInfo`，拼写必须一致。
- `MaxBufferedRequests`：默认 64，合法 1～512。限制一次仿真前未消费的显示请求，溢出优先保留最新事件，不改变服务器实际伤害。

颜色、字体图集、大小、轨迹和寿命目前沿用原 Lyra Niagara/材质，不存在尚未实现的 UMG 字号、元素配色映射或可配置伤害公式。数字材质为 `/Game/Effects/Materials/DamageNumbers/M_numba`，实例为 `MI_numba` 与 `MI_numberSingleCard`，图集为 `/Game/Effects/Textures/Flipbooks/numbers_color`。改美术前先备份或复制资产，保留依赖路径与参数契约。

## 本地样式预览接口

组件继承的 `AddNumberPop(FHodgeNumberPopRequest)` 可从蓝图调用。只在当前本地 Controller 上预览，填写有限的正数 `DamageAmount` 和有效 `WorldLocation`，`EventId=0` 表示不参与服务器身份去重。`bIsCriticalDamage=true` 仅用于显式暴击样式预览；当前正式战斗没有暴击结算，本系统不会自行掷骰或根据大数值伪造暴击。预览不扣血、不触发奖励，也不经网络广播。

## 诊断与验收

Controller 的 `GetDamageNumberSentCount` 统计服务器接受并入队的事件；`GetLastDamageNumberSent` 保存最后一笔原始数值。组件的 `GetReceivedCount` 与 `GetRenderedCount` 分别表示本地接收和提交给 Niagara 的请求数，后者不等于像素可见性。`GetLastRequest` 可核对数值与位置，`GetNumberPopNiagara` 可确认共享组件是否存在。显式本地预览也会计入组件计数。

日志分类为 `LogHodgeNumberPop`，Verbose 记录 EventId、原始伤害、显示整数和位置。资产缺失会明确记录 Error，不伪装渲染成功。屏幕外和相机背后的数字由 Niagara 渲染自然裁剪；没有屏幕边缘指示或穿墙强制可见功能。

网络使用 Unreliable 批次，丢包时可能少显示数字；不会回滚生命值或补发玩法结果。首版两个独立客户端的接收隔离与分屏共用世界的视图隔离是不同验证项，分屏暂未验证。不要通过每个怪物再创建控件或从 OnRep_Health 差值补字来掩盖配置与网络问题。
