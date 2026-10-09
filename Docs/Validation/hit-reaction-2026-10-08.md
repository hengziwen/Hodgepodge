# 受击核心接入与验证记录

日期：2026-10-08。UE 5.5.4，Win64 Development。设计见[受击系统](../Design/hit-reaction-system.md)，操作见[受击配置手册](../Guides/hit-reaction-configuration.md)。

本文保留首次核心接入的构建与原生测试记录；随后执行的真实 PIE、Listen Server、动画资源及发现的问题见[受击 PIE／联机补测](hit-reaction-pie-network-2026-10-08.md)。

## 1. 本次改动

- 注册四级 Body、独立判定和 Controlled 原生 Tag，采用大于等于比较，无 Body 默认内部 0。
- Definition 的 ExecutionBodyTag 按执行作用域管理；Reaction 配置接入默认命中和通知独立覆盖。
- 共同 Melee 提交入口持有 Context 的服务器本地结算身份，HealthSet 回传 Damage 接受／拒绝；保持免疫、数值与死亡职责。
- 新增目标 Profile、默认反应组件及目标自己的 HitReaction GA，接入六类 Impact、动画／运动阶段、资源清理、限幅和结果接口。
- 输入／能力／AI 请求和旋转消费 Controlled，移动 SavedMove 保存受控快照；运动资源绑定捕获的旧 Avatar，避免清理新 Pawn。
- 受击复用可用 Combat 的服务器姿势 lease；没有 Combat 时临时保持目标姿势更新，先清理自身资源再解除控制。
- EnemyPawnData 提供可选 Pawn 所有权 ASC 初始化、现有 StatProfile 基础属性与 AbilitySet 授予；外部 ASC 绑定保留。
- 更新设计及配置文档。首次核心接入未修改 .uproject、Build.cs、第三方插件或二进制资产；随后补测新增隔离测试资产，见补测报告。没有自动提交或回退。

## 2. 常规构建

Editor 和 Game 均退出 0。反射修改前本项目编辑器未运行；没有使用 Live Coding。

实际执行命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=2 -gather
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=2 -gather
```

`-gather` 刷新新增源码的 UBT Makefile，不改变项目构建设置。初次自动化未匹配新测试后使用该参数重新收集并核对 unity 文件包含测试；不把 0 项测试的退出 0 算作通过。

日志副本位于 Saved/HitReaction/EditorBuild.log、GameBuild.log。引擎 IncludeOrder、已有 UI／插件等警告保留，没有通过升级构建设置或清缓存掩盖问题。

## 3. 原生测试与回归

最终报告 Saved/HitReaction/Validated/index.json：**18 项 Success，0 失败，0 未执行；其中 3 项带已有警告。** 8 项受击测试无警告，另有 5 项 Combat、4 项 Combo、1 项 Rotation 回归。

受击覆盖：四级 16 组合、配置冲突、取消前 Body 快照、低判定仍扣血、同级控制、免疫／无输出／致死拒绝、空 Impact、显式回退、预留削韧、更新／过期／解绑、不可取消能力、输入恢复、地面运动源、空中计划、倒地恢复和敌人原生初始化。

额外核对：取消未消费的自有 Launch；移动组件不可用不报告位移成功；旧 Avatar 清理不会删除新 Avatar 的 PendingLaunch。测试中目标 Profile 明确允许无蒙太奇，因此这些结果不是正式受击动画观感或真实联机位移通过的证据。

实际命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash '-ExecCmds=Automation RunTests Hodge.HitReaction+Hodge.Combat+Hodge.Combo+Hodge.Rotation' '-TestExit=Automation Test Queue Empty' '-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/HitReaction/Validated' '-abslog=E:/Project/Git/Hodgepodge/Saved/HitReaction/Validated.log'
```

中间测试发现并修复测试 AttributeSet Outer、抽象测试动作类和预览世界移动初始化的问题，以及运行资源／清理边界。初次及失败报告保留在 Saved/HitReaction，最终结论只使用 Validated 报告。

## 4. 编辑器加载与蓝图检查

已用常规编译后的编辑器命令行加载项目并执行 PythonScript commandlet，新增 Profile／组件／GA 三类可加载。

缩小范围后，对 Main/Character 下的 12 项当前角色／攻击／动画蓝图执行编译，命令退出 0，日志汇总 0 errors、28 warnings，没有保存资产。Python 不能直接读取 protected Blueprint.Status，因此以编译调用、命令结果和日志为证据，不虚构每项状态读取结果。

范围包括 BP_Hero_Pover、五段 GA_Attack_1～5、GE_MeleeDamage_Instant、ABP_Pover_Base、ALI_Pover_LocomotionInterface、ABP_Pover_LocomotionBase，以及 EnemyBase 的 ALI／ABP。未创建或编译本次尚不存在的受击专用 Profile、Montage 或怪物蓝图。

实际命令：

```powershell
& 'E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe' 'E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -unattended -NullRHI -nosplash -run=pythonscript '-script=E:/Project/Git/Hodgepodge/Saved/HitReaction/inspect_affected_editor.py' '-abslog=E:/Project/Git/Hodgepodge/Saved/HitReaction/EditorAffectedInspection.log'
```

结果列表在 Saved/HitReaction/editor-affected-inspection.json。

扩大范围的首次检查退出 1，发现 `/Game/Main/Character/Hero/GA_Attack` 仍引用已退役的 PlayTimeline，存在 ProxyFactoryClass null／缺失函数编译错误。该资产不是正式五段入口；本次未修改或重存它，也不据此宣称全项目蓝图无错误。日志在 Saved/HitReaction/EditorInspection.log。

## 5. 正式配置与验证边界

- 正式受击 Profile、各类目标动画／Slot 接法和普通 AbilitySet 中的受击 GA 需要按配置手册制作／绑定。
- 正式攻击 Definition 的 Body／Reaction、终结刀覆盖未自动重配；原技能没有配置 Reaction 时保持原行为。
- 首次交付尚未执行受击 PIE／联机；现已使用隔离动画资源补测单人 PIE、双玩家 Listen Server、拥有者／模拟代理和 100ms 模拟包延迟，结果及碰撞覆盖见补测报告。
- 正式受击资产的美术观感、独立进程／跨机器网络和丢包：未验证。测试资源的通过不等于所有正式技能已完成受击配置。
- 玩家自动升空追击、空中连段与完整吸附：未实现；当前只提供实际 Impact 成功的结果接口。
- 韧性属性、恢复、破韧：未实现，只预留数值。
- Cook、打包与 Dedicated Server 专用构建：未执行。

## 6. 工作区检查

开始时已有 Docs/README.md 修改及两份未跟踪设计文档，全部保留。首次核心接入改动限定本功能源码、测试与文档；后续补测新增 Tools/HitReaction 和 Content/CodexText/HitReactionValidation。生成物、缓存、日志和源码编辑前副本保留在忽略的 Saved/HitReaction／Saved/HitReactionPIE，不纳入源码提交。

执行 git status --short、git diff --check 与文档本地链接检查；检查通过。UTF-16 的既有 EnemyCharacter 文件保留原编码，Git 显示 binary diff 不代表修改了二进制游戏资产。
