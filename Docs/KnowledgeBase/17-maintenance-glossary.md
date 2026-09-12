# 术语、决策记录与维护规范

[返回首页](README.md)

## 项目术语

- Experience：玩法配置及加载单元，包含 PawnData、插件和 Actions。
- ActionSet：供多个 Experience 组合复用的一组动作配置。
- PawnData：生成类、能力集合、输入和相机等 Pawn 配置。
- ASC：AbilitySystemComponent，管理能力、效果、标签和属性关联。
- OwnerActor：ASC 的所属对象；玩家通常是 PlayerState。
- AvatarActor：当前执行能力的身体；玩家通常是 Hero Pawn。
- Ability Spec：某次能力授予的描述与运行句柄，不等于 Ability 类或实例。
- GE：GameplayEffect，表达属性/Tag 等效果。
- EffectContext：效果的来源、命中等上下文；自定义字段需考虑分配与序列化。
- Cue：表现事件/资源入口，不替代伤害结算。
- IMC：物理输入到 InputAction 的映射上下文。
- InputConfig：InputAction 与项目 GameplayTag 的映射配置。
- Init State：组件依赖就绪协议，不能天然保证全部业务完成。
- Receiver：参与 ModularGameplay 扩展事件的 Actor。
- CDO：类默认对象；Experience 加载方式会读取它。
- PrimaryAssetId：由类型和名称组成的资产标识，不等同于文件路径。
- Mixed：GAS 效果复制策略；并不意味着所有数据只复制给拥有者。

## 现有架构选择及依据

### 玩家 ASC 放 PlayerState

依据：PlayerState 构造创建 ASC，Hero 双入口以 PS 为 Owner。收益是玩家数据与身体分离。后续必须定义换 Pawn 的重绑定、属性重置与能力保留策略。

### 使用 Experience 控制玩法

依据：GameMode 选择 ID，ExperienceManager 加载并通知，再允许生成玩家。扩展新玩法应优先配置数据，避免在 GameMode 按地图名堆业务分支。

### 使用组件协调初始化

依据：PawnExtension 有有效状态机，HeroComponent 有停用草稿。方向已表达但迁移未完成；必须标注“目标协调组件尚未启用”。

### 移除 ALS 代码依赖

依据：CharacterBase 直接继承 ACharacter，Build.cs 不含 ALS。资源迁移和动画替换仍是独立工作，不因移除 include 而自动完成。

## 待决策事项

伤害是直改 Health 还是元属性转换；敌人 ASC 放哪个对象；玩家重生保留哪些效果；基础输入与玩法输入各由谁管理；在线 Session 是否近期引入；临时能力如何持有回收句柄。这些未形成完整实现，不能伪装成既定规范。

## 文档维护规则

人工章节只写能说明依据的结论。新增“已接通”结论应同时给出创建者、调用者、配置入口和验收证据；新增“缺失”结论应说明扫描范围以及是否排除蓝图。不要根据全文 rg 命中次数把注释调用算有效调用。

代码发生变化后先运行 check，阅读漂移文件，再更新相关人工章节，最后 refresh。refresh 不修改章节事实，只生成导航和快照；快照刷新不等于人工核对完成。

Reference 中的类型和函数通过轻量词法规则提取，会遗漏宏生成、复杂模板、多行特殊签名等情况；预处理分支也不等于当前目标一定编译。源码是最终依据，编译器是语法和链接的判定者。

## 新增决策记录模板

题目；日期；状态（提议/采用/废弃）；触发问题；现状证据；可选方案；选定方案；取舍；需修改的代码与资产；网络和生命周期影响；验收方法；关联 KB 条目。

避免为了“完整”大量复制上游文档。优先记录本项目与上游的差异、当前调用链和未接通位置。
