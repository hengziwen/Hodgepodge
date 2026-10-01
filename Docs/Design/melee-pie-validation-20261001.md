# 近战进入游戏验收记录

日期：2026-10-01。项目：`E:/Project/Git/Hodgepodge/Hodgepodge.uproject`，UE 5.5.4。通过原生 MCP 启动真实 PIE 与双玩家 Listen Server，未执行 C++ 构建或打包。

后续代码与配置已改为复用 GameplayEffectParent_Damage_Basic、通过 Experience 添加 CombatComponent；本记录对应修改前的已编译版本，不作为上述更正的运行验证结果。

## 结论

十四项常规玩法验收中十三项通过，客户端攻击一项失败；追加一次运行时数值重新初始化的诊断测试通过。不能宣称联机验收全部通过，也不能把诊断时手动补应用 GE 当作初始化修复。

本体、武器、Box 的检测 → GA → GE → Health 链条已实际跑通。当前阻塞是后加入玩家的服务器基础伤害初始化：服务器 BaseDamage=0，拥有者客户端 BaseDamage=20；服务器确实执行攻击 Montage 和命中窗口，但最终伤害为 0。

## 测试环境与入口

地图使用 `/Game/ThirdPerson/Maps/ThirdPersonMap`。临时在内存切换 Experience 的 DefaultPawnData 为 `/Game/CodexText/CombatHitWindows/Fixture/DA_MeleeTestPawn_Body`、`DA_MeleeTestPawn_Weapon`、`DA_MeleeTestPawn_HitBox`，未保存正式 Experience 或改动正式连招资产。

单机靶使用第二个本地玩家，经 GameMode 的正常 PlayerState/Pawn/ASC 初始化链生成；联机使用同进程双玩家 Listen Server，核实服务器两个 Authority Pawn，以及客户端 AutonomousProxy 和 SimulatedProxy。按 PlayerState.PlayerId 对齐两端对象，不依赖两端 Actor 名字相同。

单机通过 MCP 向 PIE PlayerController 发送鼠标左键按下/释放；联机分别向主机、客户端 EnhancedInputLocalPlayerSubsystem 注入真实 IA_Attack，经过输入绑定、Combo 授权及 GA 激活。没有在测试里直接对目标调用伤害 GE 来代替攻击。

为稳定检查判定，关闭测试 Pawn 移动，将靶放在指定位置，胶囊之间设置 Overlap；设置只作用于运行时对象。武器测试让靶跟随实际 Sword_Bone01，验证装备来源路由及伤害链，不宣称完整刀身覆盖已经精调。测试窗口 0.30～0.42 秒，BaseDamage=20，倍率 1.5。

每帧记录 Health、Montage、WindowTag；联机额外记录两端角色权限、检测原点及 BaseDamage。用例结束检查最终值与攻击是否实际激活，保存逐帧证据。

## 已执行用例

- `Body_contact_first`：100 → 70，攻击者始终 100；窗口内持续接触及动作结束后均无重复伤害。
- `Body_miss`：靶在范围外，启动攻击后仍为 100。
- `Body_late_entry`：靶在 Montage 0.50 秒才进入，已超过窗口，仍为 100。
- `Body_cancel_before`：0.10 秒取消实际活动 GA，始终 100。
- `Body_cancel_during`：0.35 秒取消实际活动 GA，取消时及后续均为 70，窗口 Tag 清理。
- `Body_after_cancel_reactivate`：同一 GA 再次激活，重新造成一次 30 点伤害。
- `Body_immunity`：目标带 Gameplay.DamageImmunity，仍为 100；测试后移除该 Tag。
- `Body_obstructed`：在攻击者和目标之间放运行时 BlockAll 立方体，仍为 100；测试后销毁障碍。
- `Body_destroy_before_window`：0.10 秒销毁旧攻击 Pawn，由 GameMode 重生，目标仍为 100，无旧执行尾伤。
- `Body_after_respawn`：新 Pawn 只有一件默认装备；真实输入重新攻击，目标 100 → 70。
- `Weapon_contact`：实际装备副本的 SkeletalMesh/Sword_Bone01 来源造成一次 30 点伤害。
- `HitBox_contact`：实际 MeleeTestHitBox 来源造成一次 30 点伤害。
- `ListenServer_host_attack`：主机攻击，目标在服务器和客户端均为 70，主机自身均为 100。
- `ListenServer_client_attack`：失败；客户端和服务器均启动 Montage/Window，但目标在两端仍为 100。
- `ListenServer_client_attack_after_damage_reinit`：补充诊断；仅在服务器移除并重新应用同一个测试初始化 GE，BaseDamage 从 0 恢复到 20，仍保持一个初始化效果。再次从客户端输入攻击，目标在两端均变为 70。

期间修正了测试脚本的 GA 实例查找、无 PlayerState 的预放置 Pawn 过滤等问题，并重新执行受影响用例；这些脚本调试记录不能当作产品失败或通过记录。汇总只采用对应最终有效逐帧文件，保留客户端原始失败和诊断复跑两份证据。

## 联机失败定位

实测证据：后加入玩家服务器 ASC 上有 `GE_MeleeTestAttributes`，但其原生 CombatSet.BaseDamage.CurrentValue=0；拥有者客户端为 20。首次玩家服务器为 20。在失败窗口内，服务器检测原点位于目标位置，服务器已执行 Montage 和 Status.Attack.HitCheck.Body，排除了“客户端没有请求攻击”这一原因。

运行时重新应用同一个初始化 GE 后，服务器 BaseDamage=20，客户端攻击和 Health 复制随即通过。这将问题缩小到初始化数值生效时序，而不是把伤害判断转回 CombatComponent。

代码检查显示 `AHodgePlayerState::PreInitializeComponents` 注册 Experience 回调；Experience 已加载时会同步进入 SetPawnData/GiveToAbilitySystem，此时 Actor 的组件 InitializeComponent 尚未完成。ASC 发现构造时创建的 AttributeSet 在组件初始化阶段进行，存在先应用无限 GE、后注册原生属性集的时序问题。该解释属于基于代码和实验的根因判断，尚未以修改后重建复跑闭环确认。

对照本机 Lyra：`Source/LyraGame/Player/LyraPlayerState.cpp:167` 在 PostInitializeComponents 中初始化 ActorInfo 并注册 Experience 回调；本项目 PostInitializeComponents 目前只调用 Super。建议把该初始化/注册链移到组件初始化完成后，保持服务器门禁、只配置一次 PawnData 和后续 PawnExtension 的 Avatar 重绑定；不通过重复 GE、默认加伤害或客户端应用伤害掩盖问题。

本轮未改 C++。修复后的重点复跑是后加入玩家的服务器 BaseDamage、首次客户端攻击、重生后攻击与服务器/客户端 Health 一致性。

## 恢复与未覆盖项

测试结束已停止全部 PIE，恢复原地图 L_MainMenu、正式 DefaultPawnData、单玩家 Standalone 和后台降频 True。临时网络设置使用 MCP set_project_setting，目标 DefaultEditorPerProjectUserSettings.ini 已按测试前备份恢复；原本不存在时删除本轮生成的文件。没有保存测试时的正式资产引用，没有提交源码。

本轮仍未单独验证友军队伍配置、完整死亡动画与死亡完成流程、窗口中途卸武器、真实运动下完整刀身覆盖、所有正式连招窗口，以及跨进程/专用服务器和打包。原生多窗口/共享 HitGroup/间隔/Context 测试已有通过记录，但这些单元结果不等于全部玩法场景已验收。

已有日志反复出现 CommonUI 与非 CommonGameViewportClient 的错误；本轮没有通过禁用检查隐藏它，也没有顺带改 viewport 配置。该错误与上述已观测的服务器 BaseDamage=0 分别记录。

## 证据与实际操作

- MCP：control_editor.play/stop、simulate_input；system_control.execute_python 执行运行时探针；set_project_setting 临时设置并恢复 PlayNetMode。
- 总汇总：`Saved/Tests/MeleePIE/summary.json`；逐帧记录在同目录各用例 JSON，日志在 `Saved/Logs/Hodgepodge.log`。
- 初始设置：`Saved/Tests/MeleePIE/originals.json`；网络配置字节备份：`net_config_backup.json`；数值重应用证据：`late_player_damage_reinit.json`。
- 探针源码：`Tools/Combat/melee_pie_probe.py` 与 `melee_network_probe.py`，依赖 MCP 会话准备的 HODGE_PIE_TEST 运行时对象；它们不是独立启动项目的一键构建/测试脚本。
- 静态检查：Python compile() 语法检查；检查本轮文档/脚本差异，保留用户已有改动。整个工作区 git diff --check 仍报告原有 Config/DefaultGame.ini 文件末尾空行，不顺带修改。
