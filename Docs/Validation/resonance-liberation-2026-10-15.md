# 共鸣解放实施与测试结果

日期：2026-10-15，Asia/Shanghai；UE 5.5.4，Win64 Development。工程规则、Lyra链路和当前Source／资产已核查。没有自动提交或推送；测试没有保存验证地图，原有未提交工作保留。

## 1. 交付结论

大招核心链路已落地：R键、满100消耗100、独立12秒CD、按阶段实际伤害回能、死亡清空充能、右下方独立HUD、AM_Burst01主动作、全程翼蒙太奇、前半段三分镜与恢复、金刚体双配置、固定大范围Box单次服务器结算。

实际结果：Editor和Game常规构建均退出0；相关原生测试81项通过、0失败（其中33项带警告）；资产检查35项通过，包括8份蓝图原生编译诊断ERRORS=0；单人、Listen Server、PIE Dedicated、Listen延迟模式各52项通过；补充截图与真实Look输入检查17项通过。

[机器可读完整结果](resonance-liberation-2026-10-15-results.json)记录原生每个测试、资产每个断言、四种网络模式及截图场景逐项结果。原始采样、失败历史和日志留在`Saved/CinematicUltimate`。这里的通过项不包含正式美术制作、全部异常穷举、独立专服或Cook／打包。

## 2. 模块与代码文件

Runtime新增：

- `Source/Hodgepodge/Public/AbilitySystem/AttributeSet/HodgeResonanceSet.h`及对应Private cpp：玩家持久充能、范围约束、拥有者复制。
- `Public/AbilitySystem/Effects/HodgeResonanceEnergyEffect.h`及对应Private cpp：共享增量GE与容量初始化GE。
- `Public/AbilitySystem/Abilities/HodgeAbilityCost_ResonanceEnergy.h`及对应Private cpp：Lyra AdditionalCosts满充检查与预测支付。
- `Public/Camera/HodgeCinematicCameraTypes.h`：完整技能相机身份、采样与只读状态。
- `Public/Data/HodgeCinematicCameraProfile.h`及对应Private cpp：三分镜、曲线、混合、安全配置、本地PP／表现事件与校验。
- `Public/Animation/HodgeAnimNotifyState_CinematicCamera.h`及对应Private cpp：原生窗口，只保存Profile配置。
- `Public/Camera/HodgeCameraMode_Cinematic.h`及对应Private cpp：实际Montage源位置求值、角色局部构图、安全回退、本地事件归属。
- `Public/UI/HodgeResonanceEnergyWidget.h`及对应Private cpp：充能／CD显示、同Tag按钮，非空Designer树可覆盖默认子树。

现有主cpp内的适配：PlayerState、HealthComponent、Melee、Definition、GameplayAbility、Hero、CameraComponent／CameraModeStack／LockOn、AbilityDefinition、GameplayUIDataSource。没有新建业务模块，没有按功能拆同一个类的cpp，没有另建SpringArm／CameraManager或攻击Timeline。

编辑器工具沿用HodgeAnimationAuthoringLibrary、HodgeUIAuthoringLibrary、HodgeCombatValidationLibrary：包装已有序列、配置通知与HUD、限定原生编译诊断、PIE只读数据及身份受限源实例探针。业务Game目标不依赖这些Editor接口。

测试源码：`Private/Tests/HodgeCinematicCameraTests.cpp`、`Private/Tests/HodgeResonanceEnergyTests.cpp`。装配、资产检查、E2E、汇总与恢复工具位于`Tools/CinematicUltimate`。所有新代码和工具含中文职责／契约／关键原因注释。

## 3. Lyra对应关系与修复

已核对本机`E:/Project/UProject/study/Epic/LyraStarterGame/Source/LyraGame`：

1. LyraGameplayAbility的CheckCost／ApplyCost和AdditionalCosts：复用Hodge已迁入入口，新增资源成本对象，不重复Commit或维护CD计时器。
2. LyraGameplayAbility→Hero→CameraComponent→CameraModeStack：复用模式优先级、按类缓存和混合。项目增补执行GUID＋代次归属、精确Montage实例时钟及电影返回种子，因为Lyra没有本需求的多分镜大招实现。
3. LyraPlayerState::PostInitializeComponents：ASC组件初始化／属性集发现后绑定ActorInfo，再CallOrRegister Experience Loaded。Hodge旧实现放在PreInitialize，导致Experience已加载时的新玩家Instant初始化GE过早应用。本次迁移到PostInitialize，仍由原有PawnExtension绑定实际Avatar。
4. Lyra AttributeSet／RepNotify模式：沿用拥有者复制与GAS属性修正。主角PlayerState创建唯一ResonanceSet，不在装备和AS中重复授予。

单人Experience晚加载时旧时序看似正常，远端加入时立即回调暴露缺口；Listen和PIE Dedicated都验证了修复后的真实容量、回能、成本和技能播放。

## 4. 测试用例及需求映射

### 4.1 原生单元与集成回归

新增7项测试均通过：

- `Hodge.Cinematic.Profile.Schema`：连续覆盖、重复Shot、空洞、非法FOV／曲线、安全参数、缺锚点、本地事件时间与PP权重。
- `Hodge.Cinematic.Profile.SourceClock`：边界分镜、末帧、窗口外、非法时间、暂停、正常退出、跳过窗口。
- `Hodge.Cinematic.Ownership.Identity`：同Spec不同代次／执行／授予身份，空句柄。
- `Hodge.Cinematic.Ownership.HeroLease`：调用实际Hero申请／释放入口；旧句柄不能清理新电影，重复释放幂等。
- `Hodge.Resonance.ResourceAndCost`：真实ASC与GE；99拒绝、100支付、上限截断、容量降低、重复初始化不回填、无效成本GE与缺ASC、死亡清零。
- `Hodge.Resonance.PhaseLedger`：实际伤害>0才占用阶段、多目标同阶段一次、不同阶段、执行重置、非法身份与UI解绑。
- `Hodge.Resonance.PlayerStateBeforePawn`：隔离Experience依赖后执行真实组件初始化顺序；无需手工绑定ActorInfo，在无Pawn阶段由实际AbilitySet初始化容量100。

另74项原生回归覆盖LockOn、Combat命中路由／几何／去重、HitReaction、Wing、Movement含Dash离地、ActionRequests含Standalone后摇取消、AttackWarp、UI与Tag Relationship。总81项，48项Success、33项带警告通过、0失败、0未运行。警告明细保留在JSON，不能把退出码0单独当作所有断言通过。

### 4.2 蓝图和资产集成

35项通过：真正AnimMontage引用、Standalone Melee、三种构图、单一前半窗口、全程翼、源长度、固定Box单次与默认Reaction、自身／输出金刚体、普攻5／技能20／大招0、一份成本与空原生Cost、12秒CD、PawnData唯一授予、R键与InputTag唯一性、8份限定原生编译诊断。

实际资产：`/Game/Main/Character/Hero/Ability/ResonanceLiberation`中的GA、DA、AS、AM_Burst01、Camera／HitProfile和GE；翼蒙太奇`/Game/Main/Character/Hero/Wing/Montages/AM_Wing_Burst01`；UI为`/Game/Main/UI/HUD/WBP_Resonance_Energy`。

### 4.3 真实PIE端到端

每种模式52项断言，全部通过：

- 单人：实际EnhancedInput R、普攻真实扣血回能5、HUD真值／位置、99拒绝且无CD、100消费一次。
- Listen Server：2玩家，选远端AutonomousProxy释放；同时检查服务端／拥有者，其他玩家不出现电影相机。
- PIE Dedicated：2客户端及服务器世界；同样检查真实客户端输入、权威伤害、资源及镜头作用域。
- Listen延迟：服务器／客户端配置`Net PktLag=100`；这是每端模拟设置，不声称实测RTT为100ms。

各模式都覆盖free、lock、pause、dash、replay、death：真实中键锁定后释放及恢复LockOn CameraMode；暂停精确实例在1.0源秒后电影位置停止，再以1.5倍继续；后半GA继续且翼显示；重复释放会话换代；Dash在电影阶段中断；死亡清理电影并从持久PlayerState确认资源0且原CD保留；电影Yaw不写回游戏方向。

锁定场景还把敌人只放置一次于冲击范围内：实际生命下降恰好一次，大招不回能；其余场景敌人隔离，验证打空与取消不返还消耗。资源夹具只用正式增量GE补能，重复释放仍等待真实12秒CD，不更改生产数值。

补充截图场景17项通过，包括真实鼠标输入在电影期间受抑制、电影退出后恢复；三个构图有实际渲染文件。截图使用仅PIE临时补光并临时关闭已有命中调试线，不保存地图或生产灯光。

## 5. 实际执行命令

PowerShell，工作目录`E:/Project/Git/Hodgepodge`：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Cinematic+Hodge.Resonance+Hodge.LockOn+Hodge.Combat+Hodge.HitReaction+Hodge.Wing+Hodge.Movement+Hodge.ActionRequests+Hodge.AttackWarp+Hodge.UI+Hodge.Relationship' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/CinematicUltimate/FinalAutomationFixed' '-abslog=E:/Project/Git/Hodgepodge/Saved/CinematicUltimate/final-automation-fixed.log'
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CinematicUltimate/configure_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CinematicUltimate/validate_assets.py
& E:/Python/python.exe Tools/CinematicUltimate/run_batch.py single listen dedicated listen-lag
& E:/Python/python.exe Tools/CinematicUltimate/run_batch.py capture
& E:/Python/python.exe Tools/CinematicUltimate/export_results.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/CinematicUltimate/restore_editor.py
git diff --check
```

网络模式在开发中分批重跑；最终每个模式最新报告均通过。UBT日志`editor-lifecycle-final.log`／`game-lifecycle-final.log`，原生报告`FinalAutomationFixed/index.json`。早期失败日志和history报告保留；没有用Live Coding替代常规构建。原生测试多前缀使用引擎支持的`+`过滤，避免把多个未执行的RunTests命令误计为回归完成。

没有采集MSVC／Clang行覆盖率或分支覆盖率。本次提供需求编号与具体用例映射，不能据此虚构覆盖率百分比。

## 6. 发现的问题、修复与剩余事项

已修复：PlayerState PreInitialize过早授予；资产装配重试重复InputTag；空Canvas默认充能树不显示风险；Game公共头文件缺完整ASC类型；UE5.5 API兼容；测试死亡后访问解绑ASC、未等复制位置就锁定、读取Python受保护字段。最终相关断言均通过。

剩余事项与边界：

1. **P1美术／手感调参**：0.68T命中帧、Box半尺寸(600,450,200)厘米、前方400厘米、伤害倍率2、5／20回能和镜头偏移是可配置原型初值。Impacts留空，金刚体不自动制造控制或无敌。
2. **P2正式表现**：当前使用已有角色／翼动作及轻量PP。掌心聚能、暗翼轨迹、爆发Niagara、音效和CameraShake正式素材未制作；LocalEvents接口已实现，空素材不能计为正式效果。也没有取得可逐帧核对的原作视频。
3. **P1扩展验收**：近墙安全回退、各画幅、任意朝向／位置完整视觉矩阵、UI相机覆盖、其他CameraModifier、换Pawn、服务器拒绝预测与同执行新实例恢复没有逐项E2E故障注入；对应身份、有限值与跳时单元测试不代替这些实机场景。
4. **通知警告**：AM_Burst01的同帧Branching Point重叠有引擎警告。现有Definition按源位置扫描状态窗口，本次翼全程、后半、取消和结束测试已通过；后续应统一通知轨警告处理，不能据此推断引擎所有Notify回调都执行了。
5. **发布验证**：PIE Dedicated已通过，独立Server Target／专服可执行程序、Cook与打包未运行。CIN-08本轮执行原生回归，没有重跑旧Dash／Sprint／Warp的所有实机矩阵。

临时内容位于资产配置和测试夹具，不在Runtime写死世界坐标、伤害时间或分镜偏移。具体属性与调整步骤见[配置指南](../Guides/resonance-liberation-configuration.md)。截图见`Saved/CinematicUltimate/Screenshots`；这些是验证产物，不作为生产素材入库。

## 7. 编辑器恢复

PIE已停止，测试前实际地图`/Game/CodexText/L_MainMenu`恢复；无Dirty地图或内容包。性能恢复为原记录：Throttle=false、Slate.bAllowThrottling=1、t.IdleWhenNotForeground=0。截图用命中调试开关恢复为本轮禁用前的开启值。

最初未记录全部Play设置，因此PIE参数恢复为明确的单人基线，不声称还原用户所有PIE选项。没有执行git commit／push，没有回退用户原有改动。
