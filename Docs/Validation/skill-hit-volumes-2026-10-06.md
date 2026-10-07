# 配置检测体、多段攻击与独立技能验证

日期：2026-10-06，UE 5.5.4，Win64 Development。未提交代码。修改前备份：`E:/Project/Backups/Hodgepodge/SkillHitVolumes/20261006-215225/RESTORE.md`，3142 个文件含 SHA256。

## 实际实现范围

- 保留现有武器／身体 SocketSweep、组件 BoxSweep 和普攻连段授权。
- 新增配置球／盒／胶囊、角色／注册组件／世界变换／目标锚点、固定／跟随、Overlap／Sweep、OnceOnEnter、检测中心过滤和显式连续位移。
- 新增 HitPoints，条目身份与标签分离；支持逐段命中、Execution／TriggerTime 共享去重、确认目标及范围内锁定目标。
- 新增 Standalone／ComboCoordinated 分流、独立输入标签授予、正常 GAS 激活与远端取消。旧连段继续走协调器授权。
- 新增服务器 PrepareHitExecutionContext、SetHitAnchor、SetHitTarget、ResetHitGeometryHistory 蓝图入口。
- 无新增角色常驻组件，无独立技能 Actor。
- 补正 HodgeCharacterStatProfile／HodgeEquipmentStatProfile 两个已有文件的首 include 顺序；不改属性实现。
- 增加 Editor 专用 HodgeCombatValidationLibrary，使 PIE 测试动作在原生世界 Tick 执行；Editor 私有依赖显式加入 GameplayAbilities。Runtime 不依赖 Editor。

## 构建与原生测试

Editor／Game 常规构建退出 0。最后一次 Game 检查为 Up to date，之前本轮 Runtime 编译和链接已成功。日志在 `Saved/SkillHitVolumes/Editor-build-final.log`、`Editor-build-validation.log`、`Game-build-final.log`；初轮完整 Game 编译日志在 `Game-build.log`。

28 项 Runtime 原生测试全部 Success、0 errors，其中 4 项带警告。覆盖新几何、锚点、LOS 原点、传送／连续位移、Point 跨帧身份及取消、配置校验、独立授权，以及既有 Timeline、Combo、Attributes、WeaponPresentation 和 Rotation。报告：`Saved/SkillHitVolumes/NativeFinal/index.json`。

原生警告包括未配置 GameplayCueNotifyPaths、无世界上下文的测试 Actor 销毁，以及属性负向场景的预期失败诊断；不声称全日志无警告。

Editor 的实际 Slate DefinitionWorkflow 单独执行并通过（日志 2026.10.06-15.41.07 UTC）；包括编辑、Undo／Redo、复制／删除、预览寻帧和姿势更新。加上 28 项 Runtime 共 29 项原生／编辑器测试通过，不把 NullRHI 构建视为编辑器工作流验证。

## 单人 PIE

最终使用原生排队入口执行激活和取消，10 项全部通过，2170 条采样：

- 三个 Point：150 → 125 → 100 → 75，每段独立。
- 三个 Window：150 → 125 → 100 → 75，每段独立。
- OnceOnEnter 窗口迟到进入：保持 150。
- 固定世界中心，施法者中途移动 2000cm：仍在原中心完成三段，最终 75。
- ConfirmedTarget，目标关闭碰撞并移出普通区域但保持在目标距离内：三段，最终 75。
- 同一 Execution 共享组：只扣一次，最终 125。
- TriggerTime 共享组：三个不同触发时刻，最终 75。
- 第一段前取消：保持 150。
- 第一段后取消：最终 125，无后续命中。
- 第一段之后进入范围：只受后两段，最终 100。

结果：`Saved/SkillHitVolumes/single-tests.json`。测试使用当前正式 GE_MeleeDamage_Instant：来源 BaseDamage=30，GE 对捕获属性另加 20，再乘 0.5，因此单段 25。没有将观察到的 25 误判为框架倍率错误，也没有修改用户 GE。

现有 GE_Heal_SetByCaller 只读检查发现无有效 Modifier 且有旧 LyraGame 导入警告；本轮未修复此无关资产。测试间恢复使用新的 GE_VolumeLabHeal，向 HodgeHealthSet.Healing 加值后夹取到上限。

补充回归：同一个 Spec、同一个 GA 实例连续两次激活，Execution 共享组两次均能重新扣 25；真实 IA_Attack 输入进入 Combo.Light.01 后被独立技能打断，连段记忆仍为 Combo.Light.01，检查时剩余 1 秒。结果：`Saved/SkillHitVolumes/reactivation-combo-tests.json`。

## 双人 Listen Server，100ms 延迟

8 项全部通过，2000 条采样，服务器最终 Health 与客户端复制值一致：

- 主机三段 Point、客户端三段 Point、客户端三段 Window：均 150 → 125 → 100 → 75。
- 客户端第一段前取消：150；第一段后取消：125。
- 客户端固定中心与 ConfirmedTarget：75。
- 客户端预测启动但服务器拒绝：服务器未激活，目标保持 150，客户端预测实例恢复为未激活。

所有例子的服务端／拥有端实例结束后均 inactive。最终测试日志没有预测键深链、互斥计数或活动 Death Spec 的新错误。结果：`Saved/SkillHitVolumes/network-tests.json`，最终会话日志 `EditorSession-verified.log`。

### 初次测试入口问题与恢复

初次直接在 Editor Python／Slate 回调中调用客户端 TryActivateAbility，触发预测键递归并阻塞编辑器。UE Actor.GetFunctionCallspace 在 GAllowActorScriptExecutionInEditor 为 true 时强制 Local，导致客户端 Server RPC 被本地执行。

发送正常关闭请求后该编辑器进程退出；没有强制结束进程或删除用户缓存。修改前文件有备份，该会话只改动本轮测试 AbilitySet 的内存内容。原 Content SHA 验证保持不变。

修正为 Editor 专用原生 World Timer 排队，在下一次 PIE Tick 激活／取消。随后按真实客户端 RPC 重新跑上述八项并通过。保留失败日志 `EditorSession-final.log`，不得将失败那次当作联机通过证据。

最终会话仍有项目既有 CommonUI GameViewport 配置错误；本轮没有修改 UI 迁移配置，功能通过不等于日志所有系统均正常。

## 资产和保护检查

4 个正式可复用 Profile 位于 `/Game/Main/Combat/HitProfiles/`；18 个新资产总数包含这些 Profile 和 CodexText 教学／测试资产。18 个新资产、正式五段 Definition 与 PawnData 共 24 个资产通过原生 EditorValidator 校验，0 errors／warnings。

6 个蓝图已显式编译通过：教学 GA 与正式五段 GA。原 Main GA 编译不保存，不覆盖用户当前资产修改。

对修改前 2739 个 Content 文件逐项比对 SHA256，全部保持不变。测试未把教学技能安装到正式 PawnData／AbilitySet，也未修改现有武器、动画、死亡或普攻资产。

最终恢复单人 PIE 设置、背景节流开启，停止 PIE，项目回到 ThirdPersonMap；dirty Content／Map 均为空。证据：`final-state.json`、`protected-files.json`。

## 实际命令

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.Combat+Hodge.Timeline+Hodge.Combo+Hodge.Attributes+Hodge.WeaponPresentation+Hodge.Rotation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/SkillHitVolumes/NativeFinal' '-abslog=E:/Project/Git/Hodgepodge/Saved/SkillHitVolumes/NativeFinal.log'
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/SkillHitVolumes/test_single.py ../../Saved/SkillHitVolumes/single-final-receipt.json
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/SkillHitVolumes/test_network.py ../../Saved/SkillHitVolumes/network-tests-receipt.json
python -X utf8 Saved/LyraAnimationWork/bridge_client.py python Saved/SkillHitVolumes/test_reactivation_combo.py ../../Saved/SkillHitVolumes/reactivation-combo-receipt.json
```

PIE 脚本需要对应地图、玩家、夹具和前置准备，不应脱离上下文直接运行。Saved 是本轮证据，不纳入源码变更。

## 未执行／未实现

未 Cook／打包，未独立进程网络／独立专服，未真实凤凰动作与高速曲线历史回放，未完整正式五刀刃覆盖、完整死亡／重生、通用客户端 TargetData、自动锁定选择或独立技能 Actor。

配置步骤见[使用说明](../Design/skill-hit-volumes-usage.md)，后续范围见[设计提案](../Design/skill-hit-volumes-proposal.md)。
