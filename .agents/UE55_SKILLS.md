# Hodgepodge UE 5.5.4 skills

安装日期：2026-10-15。三套上游共 108 个条目，全部安装在仓库内；新一轮对话可发现并按任务选择，显式调用可使用 `$技能名称`。

## 来源与适配方式

- quodsoler/unreal-engine-skills：31 个；保留原技能名。
- kevinpbuckley/unreal-engine-skills：74 个；两个重名入口改为 `kb-ue-animation-system`、`kb-ue-gameplay-framework`，其他保留原名。
- EpicGames/unreal-engine-skills-for-claude-code-plugin：3 个；改为现有 UE5.5 桥接和标准 Agent Skill 工作流。

完整提交号、原路径、文件摘要和本机源码锚点见 [锁定清单](ue55-skills.lock.json)。上游许可保留在各自 `skill-sources/<来源>/`；Kevin 固定提交未提供仓库级 LICENSE，不将其标为 MIT。

[项目契约](ue55-policy.md) 与 49 份领域参考负责本地执行；`skill-sources/` 保留完整上游 skill 内容、references、assets 和 scripts，不参与自动发现。上游代码块未整体移植或编译认证；采用具体片段时仍需查本机 5.5 源码和调用链。

## 已处理的版本差异

- 构建固定 V5/Unreal5_5、本机引擎路径和 Editor/Game 验证流程；源码导航移除 5.8/5.7 默认环境。
- GAS 保留 5.5 已有 GetAssetTags/GetDynamicSpecSourceTags，区分引擎默认与 Hodge InstancedPerActor，并复用 PlayerState/PawnExtension/Init State。
- GameFeature、相机、输入和动画指向已核实的 Lyra 与 Hodge 源码入口。
- Mover、StateTree、PCG、IK 等版本敏感接口以本机头文件重建调用，避免直接调用 5.8 新接口。
- Epic 的 ToolsetRegistry/UAgentSkill/官方 MCP 路径被本地桥接发现、HodgeAbilityEditor 和文件型 SKILL.md 流程替代。
- MetaHuman 新版 Creator/Animator 与 UDS/UDW 商业资产未安装；相关条目提供依赖诊断和本机可用路径，不伪装成已启用功能。

## 编辑器连接状态

`.codex/config.toml` 已将两个服务的旧 D 盘路径修正为当前工作区，使用 `Saved/UESkillsRuntime` 隔离环境；蓝图桥通过 HODGE_PROJECT 限定工程，原生网关的回环请求通过 NO_PROXY 绕过系统代理，令牌仍由已有网关读取，不写入配置。

本次通过真实 MCP 握手核实蓝图桥 15 个工具，并通过 bridge_status 确认工程为 `E:/Project/Git/Hodgepodge/Hodgepodge.uproject`；ChiR24 网关返回 `unreal`，只读 search/describe 检查通过。没有修改资产、启动 PIE 或修改第三方网关、引擎、插件。连接状态只代表本次检查时刻，后续操作仍需核实项目身份和实际 schema。

如果清理 Saved 或迁移机器，先核实当前路径并重建隔离环境（项目根目录 PowerShell）：

```powershell
# 重建被忽略的运行环境，依赖清单保留在仓库。
python -m venv Saved/UESkillsRuntime
& './Saved/UESkillsRuntime/Scripts/python.exe' -X utf8 -m pip install -r .agents/requirements.txt
# 只读验证握手、工具目录、项目身份和能力发现。
& './Saved/UESkillsRuntime/Scripts/python.exe' -X utf8 .agents/scripts/check_ue55_mcp.py
```

更换工作区根目录时同步 `.codex/config.toml` 的 command/args/cwd/HODGE_PROJECT；不要复制旧用户目录。技能可在下一轮对话发现；MCP 配置若尚未重新加载，重新打开项目或重启 Codex 后使用。

## 验证与维护

在项目根目录执行 `python -X utf8 .agents/scripts/validate_ue55_skills.py`。校验元数据、名字唯一性、链接、源文件完整性、引擎版本、源码锚点和 Lyra 路径。它不启动编辑器、不执行上游脚本，也不替代具体功能的编译/PIE/联机验证。

更新上游时先下载到暂存目录，比较固定提交和本地适配，重新核查受影响 API；不要用安装器覆盖当前入口或直接把上游 target engine 改成 5.5。

## 可发现条目

### quodsoler（31）

- [ue-actor-component-architecture](skills/ue-actor-component-architecture/SKILL.md) — Actor 与组件生命周期。
- [ue-ai-navigation](skills/ue-ai-navigation/SKILL.md) — AI 与导航。
- [ue-animation-system](skills/ue-animation-system/SKILL.md) — 动画与 Montage。
- [ue-async-threading](skills/ue-async-threading/SKILL.md) — Timer、异步与线程。
- [ue-audio-system](skills/ue-audio-system/SKILL.md) — 音频与 MetaSound。
- [ue-blueprint-cpp-interop](skills/ue-blueprint-cpp-interop/SKILL.md) — C++ 反射与蓝图接口。
- [ue-character-movement](skills/ue-character-movement/SKILL.md) — CharacterMovement。
- [ue-cpp-foundations](skills/ue-cpp-foundations/SKILL.md) — C++ 反射与蓝图接口。
- [ue-data-assets-tables](skills/ue-data-assets-tables/SKILL.md) — 数据资产与资源加载。
- [ue-editor-tools](skills/ue-editor-tools/SKILL.md) — 编辑器工具与 Python。
- [ue-game-features](skills/ue-game-features/SKILL.md) — GameFeature、Experience 与 Init State。
- [ue-gameplay-abilities](skills/ue-gameplay-abilities/SKILL.md) — GAS 能力、属性和效果。
- [ue-gameplay-cameras](skills/ue-gameplay-cameras/SKILL.md) — 相机与 CameraMode。
- [ue-gameplay-framework](skills/ue-gameplay-framework/SKILL.md) — Gameplay Framework。
- [ue-gameplay-tags-messaging](skills/ue-gameplay-tags-messaging/SKILL.md) — GameplayTag 与消息。
- [ue-input-system](skills/ue-input-system/SKILL.md) — Enhanced Input。
- [ue-mass-entity](skills/ue-mass-entity/SKILL.md) — Mass Entity。
- [ue-materials-rendering](skills/ue-materials-rendering/SKILL.md) — 材质与着色器。
- [ue-module-build-system](skills/ue-module-build-system/SKILL.md) — 模块与 UBT 构建。
- [ue-mover](skills/ue-mover/SKILL.md) — 实验性 Mover。
- [ue-networking-replication](skills/ue-networking-replication/SKILL.md) — 复制与服务器权限。
- [ue-niagara-effects](skills/ue-niagara-effects/SKILL.md) — Niagara 特效。
- [ue-physics-collision](skills/ue-physics-collision/SKILL.md) — 碰撞与 Chaos。
- [ue-procedural-generation](skills/ue-procedural-generation/SKILL.md) — PCG 与程序化几何。
- [ue-project-context](skills/ue-project-context/SKILL.md) — 项目上下文核查。
- [ue-sequencer-cinematics](skills/ue-sequencer-cinematics/SKILL.md) — Sequencer 与渲染队列。
- [ue-serialization-savegames](skills/ue-serialization-savegames/SKILL.md) — 存档与序列化。
- [ue-state-trees](skills/ue-state-trees/SKILL.md) — StateTree。
- [ue-testing-debugging](skills/ue-testing-debugging/SKILL.md) — 自动化、调试和验证。
- [ue-ui-umg-slate](skills/ue-ui-umg-slate/SKILL.md) — UMG、Slate 与 CommonUI。
- [ue-world-level-streaming](skills/ue-world-level-streaming/SKILL.md) — 世界分区与关卡流送。

### kevinpbuckley（74）

- [ue-actors-and-components](skills/ue-actors-and-components/SKILL.md) — Actor 与组件生命周期。
- [ue-ai-and-navigation](skills/ue-ai-and-navigation/SKILL.md) — AI 与导航。
- [kb-ue-animation-system](skills/kb-ue-animation-system/SKILL.md) — 动画与 Montage。
- [ue-asset-management](skills/ue-asset-management/SKILL.md) — 数据资产与资源加载。
- [ue-audio-and-metasounds](skills/ue-audio-and-metasounds/SKILL.md) — 音频与 MetaSound。
- [ue-auto-landscape-materials](skills/ue-auto-landscape-materials/SKILL.md) — Landscape、RVT 与植被。
- [ue-automation-and-testing](skills/ue-automation-and-testing/SKILL.md) — 自动化、调试和验证。
- [ue-blueprint-cpp-integration](skills/ue-blueprint-cpp-integration/SKILL.md) — C++ 反射与蓝图接口。
- [ue-blueprint-fundamentals](skills/ue-blueprint-fundamentals/SKILL.md) — 蓝图图表与编译。
- [ue-character-and-movement](skills/ue-character-and-movement/SKILL.md) — CharacterMovement。
- [ue-coding-standards](skills/ue-coding-standards/SKILL.md) — C++ 反射与蓝图接口。
- [ue-control-rig-and-ik](skills/ue-control-rig-and-ik/SKILL.md) — Control Rig 与 IK。
- [ue-core-types-and-containers](skills/ue-core-types-and-containers/SKILL.md) — 核心类型、日志和容器。
- [ue-cpp-fundamentals](skills/ue-cpp-fundamentals/SKILL.md) — C++ 反射与蓝图接口。
- [ue-data-driven-design](skills/ue-data-driven-design/SKILL.md) — 数据资产与资源加载。
- [ue-debugging-techniques](skills/ue-debugging-techniques/SKILL.md) — 自动化、调试和验证。
- [ue-delegates-and-events](skills/ue-delegates-and-events/SKILL.md) — 委托与事件。
- [ue-editor-scripting-and-python](skills/ue-editor-scripting-and-python/SKILL.md) — 编辑器工具与 Python。
- [ue-enhanced-input](skills/ue-enhanced-input/SKILL.md) — Enhanced Input。
- [ue-game-thread-performance](skills/ue-game-thread-performance/SKILL.md) — 性能分析。
- [ue-gameplay-ability-system](skills/ue-gameplay-ability-system/SKILL.md) — GAS 能力、属性和效果。
- [ue-gameplay-architecture-planning](skills/ue-gameplay-architecture-planning/SKILL.md) — 玩法架构方案。
- [kb-ue-gameplay-framework](skills/kb-ue-gameplay-framework/SKILL.md) — Gameplay Framework。
- [ue-gameplay-tags](skills/ue-gameplay-tags/SKILL.md) — GameplayTag 与消息。
- [ue-importing-content](skills/ue-importing-content/SKILL.md) — 资源导入。
- [ue-landscape-and-foliage](skills/ue-landscape-and-foliage/SKILL.md) — Landscape、RVT 与植被。
- [ue-landscape-rvt-and-grass](skills/ue-landscape-rvt-and-grass/SKILL.md) — Landscape、RVT 与植被。
- [ue-levels-and-world-partition](skills/ue-levels-and-world-partition/SKILL.md) — 世界分区与关卡流送。
- [ue-lighting-and-lumen](skills/ue-lighting-and-lumen/SKILL.md) — 渲染、Lumen 与性能。
- [ue-logging-and-assertions](skills/ue-logging-and-assertions/SKILL.md) — 核心类型、日志和容器。
- [ue-master-material-authoring](skills/ue-master-material-authoring/SKILL.md) — 材质与着色器。
- [ue-material-graph-effects](skills/ue-material-graph-effects/SKILL.md) — 材质与着色器。
- [ue-materials-and-shaders](skills/ue-materials-and-shaders/SKILL.md) — 材质与着色器。
- [ue-memory-and-gc](skills/ue-memory-and-gc/SKILL.md) — 对象生命周期与 GC。
- [ue-meshes-static-and-skeletal](skills/ue-meshes-static-and-skeletal/SKILL.md) — 网格与骨骼资源。
- [ue-module-and-build-system](skills/ue-module-and-build-system/SKILL.md) — 模块与 UBT 构建。
- [ue-mover-movement-system](skills/ue-mover-movement-system/SKILL.md) — 实验性 Mover。
- [ue-nanite-and-rendering](skills/ue-nanite-and-rendering/SKILL.md) — 渲染、Lumen 与性能。
- [ue-navigating-engine-source](skills/ue-navigating-engine-source/SKILL.md) — 引擎源码定位。
- [ue-networking-and-replication](skills/ue-networking-and-replication/SKILL.md) — 复制与服务器权限。
- [ue-niagara-custom-modules](skills/ue-niagara-custom-modules/SKILL.md) — Niagara 特效。
- [ue-niagara-flamethrower](skills/ue-niagara-flamethrower/SKILL.md) — Niagara 特效。
- [ue-niagara-mesh-disintegration](skills/ue-niagara-mesh-disintegration/SKILL.md) — Niagara 特效。
- [ue-niagara-renderers-and-materials](skills/ue-niagara-renderers-and-materials/SKILL.md) — Niagara 特效。
- [ue-niagara-sequenced-effects](skills/ue-niagara-sequenced-effects/SKILL.md) — Niagara 特效。
- [ue-niagara-vfx](skills/ue-niagara-vfx/SKILL.md) — Niagara 特效。
- [ue-packaging-and-deployment](skills/ue-packaging-and-deployment/SKILL.md) — Cook、打包与部署。
- [ue-physics-and-chaos](skills/ue-physics-and-chaos/SKILL.md) — 碰撞与 Chaos。
- [ue-plugins-and-modules](skills/ue-plugins-and-modules/SKILL.md) — 模块与 UBT 构建。
- [ue-profiling-and-optimization](skills/ue-profiling-and-optimization/SKILL.md) — 性能分析。
- [ue-project-structure](skills/ue-project-structure/SKILL.md) — 模块与 UBT 构建。
- [ue-save-and-load](skills/ue-save-and-load/SKILL.md) — 存档与序列化。
- [ue-sequencer-and-cinematics](skills/ue-sequencer-and-cinematics/SKILL.md) — Sequencer 与渲染队列。
- [ue-subsystems](skills/ue-subsystems/SKILL.md) — Subsystem 生命周期。
- [ue-timers-and-async](skills/ue-timers-and-async/SKILL.md) — Timer、异步与线程。
- [ue-umg-and-slate](skills/ue-umg-and-slate/SKILL.md) — UMG、Slate 与 CommonUI。
- [ue-metahuman-animator](skills/ue-metahuman-animator/SKILL.md) — MetaHuman 可用性与旧版流程。
- [ue-metahuman-creator](skills/ue-metahuman-creator/SKILL.md) — MetaHuman 可用性与旧版流程。
- [ue-metahuman-live-link](skills/ue-metahuman-live-link/SKILL.md) — MetaHuman 可用性与旧版流程。
- [ue-uds-cinematics-rendering](skills/ue-uds-cinematics-rendering/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-clouds](skills/ue-uds-clouds/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-fog-and-atmosphere](skills/ue-uds-fog-and-atmosphere/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-lighting-and-shadows](skills/ue-uds-lighting-and-shadows/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-modifiers-configs-state](skills/ue-uds-modifiers-configs-state/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-performance-mobile-troubleshooting](skills/ue-uds-performance-mobile-troubleshooting/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-setup-and-modes](skills/ue-uds-setup-and-modes/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-simulation](skills/ue-uds-simulation/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-sun-moon-stars](skills/ue-uds-sun-moon-stars/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-uds-time](skills/ue-uds-time/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-udw-material-and-screen-effects](skills/ue-udw-material-and-screen-effects/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-udw-particles-lightning-wind-sounds](skills/ue-udw-particles-lightning-wind-sounds/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-udw-random-seasons-temperature](skills/ue-udw-random-seasons-temperature/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-udw-setup-and-state](skills/ue-udw-setup-and-state/SKILL.md) — UDS/UDW 商业资产接入核查。
- [ue-udw-spatial-weather](skills/ue-udw-spatial-weather/SKILL.md) — UDS/UDW 商业资产接入核查。

### epicgames（3）

- [create-toolset](skills/create-toolset/SKILL.md) — UE5.5 编辑器工具扩展。
- [unreal-mcp](skills/unreal-mcp/SKILL.md) — UE5.5 编辑器 MCP 操作。
- [unreal-skill](skills/unreal-skill/SKILL.md) — UE5.5 项目工作流 Skill 编写。
