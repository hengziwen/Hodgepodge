# 共鸣回路／暗涌首版实施与测试报告

日期：2026-10-15；UE 5.5.4；设计基线：[havoc-dark-surge.md](../Design/havoc-dark-surge.md) v0.4。

首版代码、正式资产、输入及Experience HUD已接通。Editor／Game常规构建通过；39项原生测试、30项资产检查、四种核心PIE场景、三种真实命中场景及单人E/R HUD回归通过。通知帧与Box为首版调试值，美术及手感校准仍需继续。没有自动提交Git。

## 1. 已冻结并实现的行为

- 暗流独立于R能量，容量100、初值0；满条才能灭音。灭音无CD、不一次扣空100。
- 灭音使用AM_Ex_Skill01_01源动画包装的蒙太奇，保留根运动，允许位移。专用提交通知在角色当时位置生成一次快照Box；每目标一次，空挥同样入态，查询结束后再建立Buff。
- 暗涌只按实际暗流维持，不另设时长上限。每秒10点、周期0.1秒；回能补燃料，超100部分截断。不同系统不各自维护Buff倒计时。
- 地面起手、可打断；普通受击保留独立Buff；死亡、驱散和换Pawn清空暗流。常态死亡也清空，旧Avatar回调不能影响新Avatar。
- 普通E／破命共享同一8秒冷却GE；形态切换不刷新CD，CD失败不回退其他形态。
- 普通攻击实际生命伤害每有效阶段回5点；普通E／破命成功施放回20点。强化普攻不默认回暗流。
- 暗涌提高20%湮灭伤害；强化普攻额外回1点R能量，全角色每秒最多一次，叠加原有每阶段R收益。
- 两套普攻共用同一连段图，退出只清理续接记忆，不结束当前动作或R。原有普通链3→5→4次序保留。

## 2. 模块与接口

UHodgeHavocSet由PlayerState原生创建，拥有者复制。GE_DarkFlow_Init仅初始化容量，不重复创建属性集或补满当前资源。

UHodgeGameplayAbility_HavocRules采用既有Lyra式OnSpawn／Independent／ServerOnly链路，管理当前Avatar、会话GUID、两个GE句柄和全局拢音限流。TryEnterDarkSurge校验实际攻击来源；EndDarkSurge只清理匹配会话；失败事务配对回滚。生命周期清空通过原生增量GE，不受收益GE误配置影响。

UHodgeGameplayAbility_ResonanceCircuit继承Melee；UHodgeAnimNotify_DarkSurgeCommit复用PostPhysics查询批次、伤害、受击、阵营及去重。Melee分别管理暗流阶段收益和施放提交锁存，伤害执行只对Havoc元素使用来源加成。

ASC.ResolveAbilityVariant(InputTag, OutFailureReason)按状态选择唯一Spec，再由GAS检查CD／成本；失败原因区分InvalidInputTag、NotGranted、NoEligibleVariant与AmbiguousVariants。输入和UI读取同一选择结果。

Combat.InvalidateModeContinuation清理缓冲和续接，不Configure／Shutdown。Hero保持普攻Started即时响应，长按0.35秒后在有界窗口中申请灭音。共享UIDataSource提供GetHavocSnapshot／OnHavocChanged，WBP只显示真实资源。

Lyra参考包括LyraAbilitySet的GiveToAbilitySystem／TakeFromAbilitySystem、LyraAbilitySystemComponent的输入队列及OnPawnAvatarSet、LyraPawnExtensionComponent的解绑委托、LyraGameplayAbility的OnSpawn及结束清理。Lyra没有完整暗涌／双五段系统；本次仅补角色规则、形态选路和元素增伤。普攻Started为Hodge连段和长按需求适配，未改其他输入的Lyra式GAS流程。

## 3. 已接入资产与配置位置

根目录：`/Game/Main/Character/Hero/Ability/Havoc/`。

- GA_HavocRules：DrainPerSecond=10、HoldThreshold=0.35、HoldBufferSeconds=0.3，以及状态／消耗／收益GE引用。
- GE_DarkFlow_Init：容量100；GE_DarkFlow_Gain：SetByCaller增量；GE_DarkSurge_State：Infinite、非周期、20%湮灭加成；GE_DarkFlow_Drain：0.1秒周期及实际时间消费执行。
- GA_Resonance_Circuit、DA_MieYin、AM_Ex_Skill01_01_Montage：灭音资格、动作和一次Box提交。初始Box半尺寸(350,350,150)厘米，中心为提交时角色位置。
- GA_EnhancedNormalAttack、DA_EnhancedAttack_1～5、AM_Ex_Attack01～05_Montage：强化五段共享模板，单次动作与命中差异留在Definition／通知。
- GA_EnhancedResonanceSkill、DA_PoMing、AM_Ex_Skill02_Montage：破命继承既有GA_Resonance_Skill的同一CooldownGameplayEffectClass。
- AS_Havoc：上述七份Definition、独立规则能力及初始化GE；已追加到`/Game/Main/Data/PawnData/DA_Dafult_PawnData`。
- `/Game/Main/Data/Combo/DT_LightCombo`新增互斥入口及五个强化节点；常态定义BlockedSourceTags、强化定义RequiredSourceTags使用State.Combat.Mode.DarkSurge。
- 既有DA_Attack_1～5配置DarkFlowGainPerPhase=5；普通E及破命配置DarkFlowGainOnCast=20；普通E只增补CastCommitted通知，保留已有翼、命中和取消配置。
- `/Game/Main/Input/InputAction/IA_Attack`移除一次性Pressed触发器，普攻C++绑定改Started；真实松开才结束长按。
- `/Game/Main/UI/HUD/WBP_DarkFlow`为Designer序列化控件，注入UI.Slot.DarkFlow。WBP_HodgeHUD.DarkFlowPoint在血条上方；EAS_HodgeGameplayUI追加第四个Widget条目。

不增设空GA_Attack_Base；四类技能复用现有Melee执行基础，R保持原成本派生链。GE资产及GA模板方便后续迭代，不复制四套命中／Montage代码。

## 4. 测试结果

### 4.1 常规构建及蓝图

Editor最终通过：Saved/Havoc/editor-build-ge-assets.log；Runtime及输入修改后的Game通过：Saved/Havoc/game-build-input.log。使用常规UBT，不以Live Coding代替。最后一项修改只涉及Editor测试辅助代码，Game源码未再变化。

八个新GA／GE蓝图的13次原生编译检查全部ERRORS=0。Designer控件此前创建编译错误0，结构与重复制作检查通过；本轮真实Experience注入和运行数据在PIE确认。没有Cook、打包或独立专服可执行文件验收。

### 4.2 原生与资产

39/39通过：24项无警告、15项带既有资产警告，失败0、未运行0。最终原始报告：Saved/Havoc/AutomationGEFinal/index.json；日志：Saved/Havoc/automation-ge-final.log。

新增8项对应需求：Resource（HDS-01）、FuelClock（HDS-02）、HoldInput（HDS-01）、GlobalProcGate（HDS-06）、StateEffect（HDS-04）、VariantSelection（HDS-05／07）、SessionIntegration（HDS-02及生命周期）、AssetConfiguration（HDS-05／09及正式授予）。其余31项覆盖Combo、Combat、ActionRequests、Resonance与UI回归。

SessionIntegration验证真实周期扣除、同帧结算、归零、错误会话、同步驱散回滚、普通受击保留、死亡清空、常态死亡清空、实际Pawn更换和移除规则能力。周期实测0.55秒扣5.5点，燃料94.500008，允许误差0.001。

只读正式资产30项检查通过，涵盖七个指定源动画链接、通知、形态资格、收益、共享CD、PawnData唯一授予、EAS及输入触发器。报告：Saved/Havoc/asset-validation.json。

### 4.3 实际PIE与真实伤害

核心输入：single、listen、dedicated、listen-lag（100ms）各17项，共68项断言通过。使用真实InputAction及GAS激活，覆盖长按灭音、空挥入态、强化五段、普通E／破命共享CD、破命回能、超过基础10秒仍按燃料维持、驱散清空、退出后普通第一段及拥有者HUD。

真实命中：single、listen、dedicated各6项，共18项断言通过。普通普攻实际生命伤害回5点暗流；多目标强化普攻按阶段只回原5点R加拢音1点；换段不足一秒只增加5点，累计11；同配置强化伤害为常态的1.2倍；收益只属于施法者ASC。

E/R HUD单人回归23项通过，覆盖真实8秒／12秒CD、能量环、R资源成本、HUD重建、Experience UI停用恢复和唯一扩展注入。截图位于Saved/Havoc/UIRegression。

这些PIE共109项断言，与39个原生测试及30个资产检查分开计数。结果与具体需求／失败原因保存在[可追溯JSON](havoc-dark-surge-2026-10-15-results.json)。没有生成源码行覆盖率，不虚构覆盖百分比。

## 5. 测试发现及修复

1. IA_Attack的InputTriggerPressed下一帧Completed导致长按提前结束：移除此触发器，普攻使用Started，真实长按PIE复测通过，短按仍即时响应。
2. 原会话结束未清空暗流：补齐死亡、驱散、Pawn解绑／更换和能力撤销；死亡常态储能同样清空。归零与失败回滚不误伤其他效果。
3. 外部驱散发生在第二个GE应用的同步回调中：句柄先局部接收，执行身份失效后单独回收返回句柄，避免孤立消耗GE。
4. 新Pawn测试夹具误用普通Actor，未触发项目只对Pawn发送的OnPawnAvatarSet：改为真实APawn验证。
5. 命中夹具在旧连段记忆尚有效时测试长按，实际进入下一段而非入口：分开验证记忆与长按，等待记忆过期后从第一段验收；不通过强行取消前摇让测试通过。
6. Python不能直接写EditDefaultsOnly结构实例、读取保护的SlotAnimTracks：结构用ImportText，源动画通过原生序列化通知链接检查，保留真实编译与玩法断言。
7. Editor关闭请求尚未完成时链接失败LNK1104：确认进程退出后常规重建通过。历史失败日志保留，不覆盖成成功记录。
8. 旧UI测试把扩展数量写死为3：改为读取Experience实际Widget数量，E/R HUD回归通过。

## 6. 已知限制与未验证项

- 新动作命中时间、恢复时间及Box为可调首版初值，尚未由美术逐帧校准。强化动作使用AvatarRoot虚拟Box，未声称已制作完整剑轨／翼面多点判定。
- 新动作朝向Warp暂未开启；原普通攻击、E和R配置保留。没有新增攻击吸附。
- 长按只在合法取消窗口发起，默认等待0.3秒；超时作废并需松开重按，不越权打断不可取消前摇。可在GA_HavocRules调整HoldBufferSeconds或按动作调整取消窗口。
- 新动作对应翅膀、炸气特效、音效及持续Cue仍是美术接入待办，本轮没有默认暗涌全程显示翅膀。
- 重击、强化重击、鸣刃接第三段、变奏及E新图标仍明确暂缓。没有用占位动作冒充这些功能。
- 普通受击／死亡／Pawn更换的资源清理有原生集成断言；真实多人死亡重生、运行中更换Pawn和Buff中释放R的组合场景尚未单独做PIE验收。
- HUD实际显示已验证，更多分辨率／DPI与正式美术精修待后续。没有性能采样报告。
- Dedicated是PIE Dedicated，不能描述为独立专服构建、Cook或打包通过。

## 7. 复现命令与工作区

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Havoc+Hodge.Resonance+Hodge.Combo+Hodge.Combat+Hodge.ActionRequests+Hodge.UI' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/Havoc/AutomationGEFinal' '-abslog=E:/Project/Git/Hodgepodge/Saved/Havoc/automation-ge-final.log'
$env:PYTHONIOENCODING='utf-8'
E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated listen-lag
$env:HODGE_HAVOC_SUITE='hits'
E:/Python/python.exe Tools/Havoc/run_batch.py single listen dedicated
$env:HODGE_ABILITY_HUD_WORK='E:/Project/Git/Hodgepodge/Saved/Havoc/UIRegression'
E:/Python/python.exe Tools/AbilityHUD/run_batch.py single
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/validate_assets.py
E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Havoc/validate_ui_assets.py
E:/Python/python.exe Tools/Havoc/export_test_results.py --report Saved/Havoc/AutomationGEFinal/index.json
```

作者工具Tools/Havoc/configure_assets.py支持增量与幂等重跑，只处理明确资产，先保存二进制备份到Saved/Havoc/BeforeAssets；不重置后续调过的新动作窗口。源动画没有修改。最初源代码副本在Saved/Havoc/Before。用户原有大量未提交修改保留，源码UTF-16文件按原编码写回；没有回退、提交或推送。日志与缓存留在Saved，不纳入源代码交付。
