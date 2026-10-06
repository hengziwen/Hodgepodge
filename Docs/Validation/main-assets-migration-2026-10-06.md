# 39 个正式资产迁入 Main · 2026-10-06

本次按用户指定范围，将正式角色/战斗使用的 39 个资产由 CodexText 移入 Main。使用 UE AssetTools.rename_assets 更新包、对象和引用；目标预检无同名资产，不覆盖已有 Main 资源。菜单地图及三个菜单蓝图保持原位置，测试、实验资源和历史副本未列入迁移。

正式默认 Experience / PawnData / Hero / 默认武器 / ThirdPersonMap 的递归依赖链已不再包含 `/Game/CodexText/`。这不代表整个 Main 测试目录都不引用 CodexText；Exp_MeleeValidation 仍使用原测试夹具。

## 当前调参位置

- 普攻 GA 与 Definition：`/Game/Main/Character/Hero/Ability/BasicAttack`；其 `Timeline` 子目录保存五条正式时间轴。
- 连段：`/Game/Main/Data/Combo`；AbilitySet：`/Game/Main/Data/AbilitySet/AS_LightCombo`。
- 武器表现：`/Game/Main/Weapon/Presentation`，Profile 为 DA_SwordPresentation。
- 成长配置与曲线：`/Game/Main/Data/CharacterStats`，见 [配置说明](../../Content/Main/Data/CharacterStats/README.md)。
- 角色材质/贴图：`/Game/Main/Character/Hero/Anim/Model/Materials` 与 `Textures`。

## 来源与目标：39 个

- `/Game/CodexText/BasicAttack/DA_Attack01_Timeline` → `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack01_Timeline`
- `/Game/CodexText/BasicAttack/DA_Attack02_Timeline` → `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack02_Timeline`
- `/Game/CodexText/BasicAttack/DA_Attack03_Timeline` → `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack03_Timeline`
- `/Game/CodexText/BasicAttack/DA_Attack04_Timeline` → `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack04_Timeline`
- `/Game/CodexText/BasicAttack/DA_Attack05_Timeline` → `/Game/Main/Character/Hero/Ability/BasicAttack/Timeline/DA_Attack05_Timeline`
- `/Game/CodexText/CharacterStats/CT_Pover_Growth` → `/Game/Main/Data/CharacterStats/CT_Pover_Growth`
- `/Game/CodexText/CharacterStats/CT_Sword_Growth` → `/Game/Main/Data/CharacterStats/CT_Sword_Growth`
- `/Game/CodexText/CharacterStats/DA_Pover_Stats` → `/Game/Main/Data/CharacterStats/DA_Pover_Stats`
- `/Game/CodexText/CharacterStats/DA_Sword_Stats` → `/Game/Main/Data/CharacterStats/DA_Sword_Stats`
- `/Game/CodexText/DefinitionCombo/AS_LightCombo` → `/Game/Main/Data/AbilitySet/AS_LightCombo`
- `/Game/CodexText/DefinitionCombo/DA_Attack_1` → `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_1`
- `/Game/CodexText/DefinitionCombo/DA_Attack_2` → `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_2`
- `/Game/CodexText/DefinitionCombo/DA_Attack_3` → `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_3`
- `/Game/CodexText/DefinitionCombo/DA_Attack_4` → `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_4`
- `/Game/CodexText/DefinitionCombo/DA_Attack_5` → `/Game/Main/Character/Hero/Ability/BasicAttack/DA_Attack_5`
- `/Game/CodexText/DefinitionCombo/DA_LightCombo` → `/Game/Main/Data/Combo/DA_LightCombo`
- `/Game/CodexText/DefinitionCombo/DT_LightCombo` → `/Game/Main/Data/Combo/DT_LightCombo`
- `/Game/CodexText/DefinitionCombo/GA_Attack_1` → `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_1`
- `/Game/CodexText/DefinitionCombo/GA_Attack_2` → `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_2`
- `/Game/CodexText/DefinitionCombo/GA_Attack_3` → `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_3`
- `/Game/CodexText/DefinitionCombo/GA_Attack_4` → `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_4`
- `/Game/CodexText/DefinitionCombo/GA_Attack_5` → `/Game/Main/Character/Hero/Ability/BasicAttack/GA_Attack_5`
- `/Game/CodexText/Model/MI_3XingStar` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_3XingStar`
- `/Game/CodexText/Model/MI_R2T1PlayerMaleMd10011Down_2` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_R2T1PlayerMaleMd10011Down_2`
- `/Game/CodexText/Model/MI_R2T1PlayerMaleMd10011Eyes` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_R2T1PlayerMaleMd10011Eyes`
- `/Game/CodexText/Model/MI_R2T1PlayerMaleMd10011Face` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_R2T1PlayerMaleMd10011Face`
- `/Game/CodexText/Model/MI_R2T1PlayerMaleMd10011Hair` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_R2T1PlayerMaleMd10011Hair`
- `/Game/CodexText/Model/MI_R2T1PlayerMaleMd10011Up_2` → `/Game/Main/Character/Hero/Anim/Model/Materials/MI_R2T1PlayerMaleMd10011Up_2`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Down_D` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Down_D`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Down_N` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Down_N`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Eyes_D` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Eyes_D`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Face_D` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Face_D`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Hair_D` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Hair_D`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Up_D` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Up_D`
- `/Game/CodexText/Model/T_R2T1PlayerMaleMd10011Up_N` → `/Game/Main/Character/Hero/Anim/Model/Textures/T_R2T1PlayerMaleMd10011Up_N`
- `/Game/CodexText/WeaponPresentation/BP_Weapon_SwordPresentation` → `/Game/Main/Weapon/Presentation/BP_Weapon_SwordPresentation`
- `/Game/CodexText/WeaponPresentation/DA_SwordPresentation` → `/Game/Main/Weapon/Presentation/DA_SwordPresentation`
- `/Game/CodexText/WeaponPresentation/MI_SwordPresentation` → `/Game/Main/Weapon/Presentation/MI_SwordPresentation`
- `/Game/CodexText/WeaponPresentation/M_SwordPresentation` → `/Game/Main/Weapon/Presentation/M_SwordPresentation`

## 引用与配置保持

- 正式 PawnData 的 AbilitySets、ComboDefinition、StatProfile，装备定义的 ActorToSpawn / StatProfile，武器实例的 PresentationProfile，正式角色 Mesh 的六个材质槽均同步迁后路径。
- 旧 GA_BasicAttack、Melee 测试 Definition、CodexText 旧模型、Wuwa 原模型和两个历史 Definition 副本的引用同步更新；这些资产自身未迁移。外部备份保留迁移前文件。
- 五个带下划线的旧 DA_Attack_1_Timeline～5 及测试 DA_Test_Stats 保留原位置。正式时间轴为 Main 下的 DA_Attack01_Timeline～05。
- AssetTools 移动结束即已移除 39 个旧包；旧位置无资产、无引用、无重定向。没有批量清理其他目录的重定向。
- 共 22 份数据/蓝图默认值导出，归一化新旧路径后迁前迁后完全一致；没有更改攻击窗口、连段边、成长数值或武器表现参数。
- 唯一新增 C++ 差异为 HodgeAbilityEditorTests.cpp 的 DA_Attack_5 测试加载路径。Runtime C++、Config、uproject、Target 和插件保持任务开始时的内容。
- 当前指南、配置 README、夹具与复查脚本读取路径同步更新；历史创建/修复脚本保留实验输出目录并补充说明，本次未执行它们重建资产。

## 备份

备份目录：`E:/Project/Backups/Hodgepodge/MainAssetMigration/20261006-145907`。该目录含 RESTORE.md、plan.json、manifest.json，以及迁前 Main、CodexText、Source、Config、Docs、Tools 和外部引用者文件。
共 1131 个文件逐一 SHA-256 校验；最终核对 891 个不应改动的资产、源码和配置文件保持一致。备份包含任务开始时已有未提交修改，不能用 git reset 代替恢复。

## 本次实际验证

- Editor Win64 Development 常规构建退出 0，Game Win64 Development 常规构建退出 0（后者 Up to date）。
- 12 个相关蓝图编译 UpToDate，包括五个迁移 GA、武器 Actor、Hero、装备定义、武器实例、主动画图/固定层及旧 GA_BasicAttack。
- 39 个资产的数据校验全部 VALID、无校验警告；重启编辑器后从磁盘重新加载并复查，也全部 VALID。
- 23 项原生回归：Attributes 4、Combo 5、Combat 5、Rotation 1、Timeline 6、WeaponPresentation 2 全部 Success；属性测试保留现有诊断/Cue 等 10 条警告，无错误。
- 技能编辑器 Hodge.Editor.DefinitionWorkflow 在实际有渲染的 UnrealEditor 进程执行 Success，0 错误/警告，验证迁后 Definition 打开、时间轴编辑/撤销、预览和关闭；没有使用 NullRHI 来验收该 UI 用例。
- 单人 PIE 共 552 条采样：实际默认 Hero，五段 01→02→03→05→04 后直接接 01；Main 材质、FullBody 权重、武器显现→回背 WeaponOnBack→悬浮→消隐/隐藏；出生 150/150、伤害 30，角色升级后 85/170、32，武器升级后 85/180、34。
- 双人 Listen Server，100ms 延迟，共 1908 条采样、4 个角色副本：服务器、拥有者和模拟代理实际使用 Main 的武器/动画/攻击资源，攻击、回背与最终隐藏成立；Health=85、MaxHealth=180、等级 2 / Ready 复制一致。BaseDamage 按现有 COND_OwnerOnly 契约检查：服务器与拥有者为 34，模拟代理保留 0。

首次 Editor 构建遇到 UE5.5 GitSourceFileWorkingSet 无法解析中文路径的 Git 八进制转义；首次通过构建进程的 GIT_CONFIG_COUNT 临时设置 core.quotepath=false 后构建成功；为了后续 IDE 构建不再解析失败，最终设置本仓库 Git 的 core.quotepath=false。原仓库设置保存在外部备份 git-setting-before.json；没有修改全局 Git 设置、引擎或项目构建设置。双人测试首次设置未切成 Listen Server，随后核对 native 设置再重开；一次脚本误要求模拟代理复制 BaseDamage，按现有源码复制契约修正断言并复跑。失败记录保留在 Saved/MainAssetMigration，不计通过。

编辑器结束于 ThirdPersonMap，PIE=0、脏资产/地图=0，临时网络延迟已清零，原单人 PIE 设置与后台节流已恢复。

## 命令与证据

执行 `git config --local core.quotepath false` 修复本仓库的中文路径构建兼容性。

资产操作通过现有 MCP Python 入口执行 plan.py、move.py、compile.py、audit.py、fresh_load.py 和 PIE 脚本。脚本、详细映射、编译回执、导出对照及运行采样位于 `Saved/MainAssetMigration`；不纳入源码提交。

```powershell
& "E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat" HodgepodgeEditor Win64 Development "-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject" -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& "E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat" Hodgepodge Win64 Development "-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject" -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
& "E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "E:/Project/Git/Hodgepodge/Hodgepodge.uproject" -unattended -NullRHI -nosplash "-ExecCmds=Automation RunTests Hodge.Combo+Hodge.Attributes+Hodge.WeaponPresentation+Hodge.Combat+Hodge.Rotation+Hodge.Timeline" "-TestExit=Automation Test Queue Empty" "-ReportExportPath=E:/Project/Git/Hodgepodge/Saved/MainAssetMigration/NativeRegression" "-abslog=E:/Project/Git/Hodgepodge/Saved/MainAssetMigration/NativeRegression.log"
```

EditorWorkflow 使用 UnrealEditor.exe、ThirdPersonMap、-unattended -nosplash -NoSound、`-ExecCmds=Automation RunTests Hodge.Editor.DefinitionWorkflow`、相同 TestExit，报告路径为 Saved/MainAssetMigration/EditorWorkflow。

## 验证边界

未执行 Cook/打包、独立进程客户端或 Dedicated Server；未将正式 HitWindows 接入伤害，未重做完整死亡/复活/库存系统或网络取消/预测拒绝的所有边界测试。测试中只修改 PIE 中角色等级/资源，未保存到资产或存档。没有自动提交。

本次最后执行 `git diff --check`、96 份 Markdown 的相对链接检查、修改脚本的 AST 语法检查，以及 kb.py refresh/check；均通过。知识库检查 52 份 Markdown：0 链接错误、0 文件漂移。PIE 日志仍有项目既有的 CommonUI GameViewportClient 未接通错误；本次未修改 UI 模块，不将玩法冒烟通过描述为日志零错误。
