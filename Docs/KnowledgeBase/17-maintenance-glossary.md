# 术语、决策与维护规范

[返回知识库](README.md) · [本轮更新](26-update-2026-10-06.md)

> 当前核对：2026-10-06；运行结论仅限已记录范围。

## 现有术语

- Owner/Avatar：玩家 ASC 所有者 PlayerState 与当前身体 Pawn。
- Experience/ActionSet：玩法与动作配置，GameFeatureAction 在激活期间执行扩展。
- PawnData：角色类、输入、相机、AbilitySets、Combo 与默认装备配置。
- Definition：单段技能执行配置；Timeline：Window/Point 统一事件时间轴；CombatComponent：当前连段和检测协调者。
- ComboMemory：最近成功启动段与期限，不是当前动作、不授予攻击状态；InputBuffer 是 0.3 秒单槽输入缓存。
- RotationLock：约束角色 Yaw，保留镜头操作；恢复阶段平滑追随控制方向。
- HandUse：每个执行/窗口独立持有武器表现请求；最后一个释放才启动回收。
- BackSocket：当前默认 WeaponOnBack；BackTransform 是相对插槽的偏移，非旧 Pawn 根位置。
- Detection Mesh / Visual Mesh：手部命中来源与可移动显隐模型；外观轨迹不构成权威伤害。
- Equipped Bundle：Experience 资源分组；不表示不存在的授予期预加载已完成。

旧 ComboSet、双数组 Timeline、PlayerState 独立 ComboComponent 是历史命名/实现，当前不再作为操作入口。

## 已采用的组织约定

玩家 ASC 留在 PlayerState；Avatar 通过组件初始化链绑定。Equipment/Combat 用 Experience 注入，基础旋转策略是 CombatCharacter 原生组件。当前单把默认武器的表现放 WeaponInstance，不为该功能新增角色常驻组件。

固定移动动画层不随武器切换；Start 无入口、Stop 保留、常态 Pivot 禁用，冲刺 GA 与 Pivot 后续另做。武器背部跟随骨骼插槽，配置保留局部偏移。

同一项目自有类的非内联成员集中一个主 cpp，编辑器实现使用 WITH_EDITOR；禁止 _Combo/_Presentation/_Montage/_Validation 等同类片段及 include cpp 绕过。独立类型、测试、内联模板和生成代码不属该限制；不顺手重构第三方。

## 文档维护

创建者/调用者/配置入口/运行证据分别记录；静态源码、资产读取、蓝图编译、PIE、联机、打包分别陈述。历史日期/hash 不改写为新成功；旧快照归档后刷新当前索引。人工页变化在 refresh 前完成，最终 check 必须无断链/漂移。

不要根据某个组件没有 CreateDefaultSubobject 就认定未挂载，也不要从类/标签存在推断能力已运行。CodexText 有正式引用依赖，不能整目录清理。

新决策记录：日期、状态、触发问题、现状依据、职责、配置迁移、网络/生命周期、验证范围与剩余工作。
