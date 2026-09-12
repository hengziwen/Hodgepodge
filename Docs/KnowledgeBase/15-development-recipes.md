# 新增角色、技能、玩法的操作手册

[返回首页](README.md)

> 以下是开发步骤和验收要求，不代表本轮已经执行。当前先完成 [接通清单](12-integration-backlog.md) 的基础依赖。

## 新增可操作角色

1. 创建 AHodgeHeroCharacter 派生蓝图，填写 Mesh、Skeleton 匹配的 AnimClass、胶囊和基础移动参数。
2. 确认有效 PawnExtension 与 HeroComponent 挂载，避免蓝图和 C++ 重复添加同类组件。
3. 创建或复用 PawnData，指定 PawnClass、InputConfig、DefaultCameraMode、AbilitySets 与关系映射。
4. 将 Experience.DefaultPawnData 指向它；如果使用 AssetManager 默认回退，明确记录原因。
5. 检查 GameMode 数据注入路径和正确默认 InputComponent 类型。
6. PIE 记录实际类名、数据名、ASC Owner/Avatar，验证移动视角。

验收成功后再调细致动画，避免“模型显示出来”被当作角色功能完成。

## 新增一个输入动作

选择 Native 或 Ability：移动/视角通常 Native，战斗动作通常 Ability。创建正确 ValueType 的 IA，将按键添加到目标 IMC，再在 InputConfig 添加对应 Tag。

Native 需要调用 BindNativeAction 绑定回调；Ability 需要 AbilitySet 技能项使用同一个 InputTag，并确保每帧消费。新 GameplayTag 应先查已注册清单，避免仅因为名字不同创建语义重复标签。

测试 Triggered、Completed 及需要时的取消路径，尤其是打开 UI、换 Pawn 或停用 IMC 时能否清掉 Held 状态。不要只测按下。

## 新增最小测试技能

先补自定义 EffectContext 配套。创建 HodgeGameplayAbility 派生技能，明确激活策略、激活组、实例化和网络执行策略。第一版只做可观察的开始与正常结束，验证输入链后再增加 Montage 和 GE。

在 AbilitySet 配置技能类、等级和 InputTag，由权威端授予。确认客户端 Spec 可见后按键测试；激活时记录 Owner/Avatar，结束后确认独占组恢复。随后增加 Cost/Cooldown 并验证失败结果。

测试技能完成的标准是可重复激活、可结束、可取消且网络行为符合预期，而不是只打印一次日志。

## 新增伤害技能

先选定目标 ASC 初始化方案；建立可控的伤害 GE；明确 SetByCaller 名称和默认缺值行为；实现服务器命中与去重；确保 Health 结算和零血量事件；最后接 Cue、Montage、受击表现。

测试零伤害、过量伤害、连续命中、目标销毁和技能取消。不要让表现资源加载失败改变最终结算。

## 新增 Experience

在当前扫描目录创建符合蓝图类加载方式的 HodgeExperienceDefinition 资源。配置 DefaultPawnData，按需要添加 Actions 和 ActionSets。插件名必须能映射到实际 GameFeature URL。

用启动参数选择该资源名，检查日志来源和最终 ID；测试首次启动与回退；再在客户端验证 CurrentExperience 复制及资源加载。复制现有 Experience 后需要复核其引用，不能只改文件名。

## 新增临时 AbilitySet 来源

为来源保存独立 GrantedHandles，授予在服务器执行，卸载时只撤销本来源。明确 SourceObject 生命周期，避免 Ability 持有失效装备引用。若来源跨重生保留，必须说明 Avatar 变更后如何恢复表现和被动效果。

## 扩展一个 Init State Feature

先写下每个状态的前置条件和谁会发出重试通知，再实现接口和注册/注销。不得让两个 Feature 都等待对方到达更晚状态。初始化动作应幂等，EndPlay 与 Unpossess 的资源清理要和实际所有权匹配。

## 提交时同步哪些文档

改初始化：更新 03、04、12、16；改输入：更新 06、12、14；改 GAS/Health：更新 07、08、12；改资产/配置：更新 05、13，刷新自动索引。保留“源码存在”和“运行通过”的独立证据。
