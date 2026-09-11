# Hodgepodge project context

Verified from project source/config on 2026-09-10. Recheck source when changing architecture; this document is not proof of runtime correctness.

- Project: `D:/Hodgepodge/Hodgepodge.uproject`; UE 5.5.4, installed engine `D:/UE_5.5`; Windows development environment.
- Follow root `AGENTS.md` and `Docs/AI_DEVELOPMENT.md`. Existing source/config take precedence over generic skill examples. Do not upgrade UE to follow a skill.
- Single Runtime module `Hodgepodge`, Public/Private layout, `Hodge` naming, IncludeOrderVersion Unreal5_5. Editor and Game targets exist; no dedicated Server target was found.
- Enabled project features include GameplayAbilities, GameFeatures, AnimationLocomotionLibrary and AnimationWarping. ALS-Refactored is present. Do not refactor third-party ALS to fit generic animation examples.
- `AHodgePlayerState` owns `UHodgeAbilitySystemComponent`, replicates it with Mixed mode, and owns `UHodgeHealthSet`. Preserve PlayerState ownership and Pawn avatar initialization; account for authority, replication, respawn and teardown.
- `UHodgeHealthSet` exposes OnHealthChanged, OnMaxHealthChanged and OnOutOfHealth. Prefer existing event-driven presentation hooks over widget Tick polling.
- Experience loading: `UHodgeExperienceManagerComponent`; Pawn initialization: `UHodgePawnExtensionComponent` and the InitState chain. Extend these existing paths instead of assuming a complete stock Lyra framework.
- `UHodgePawnData` currently exposes PawnClass; several ability/input/camera fields remain commented out. Do not instruct users to configure properties that do not exist.
- `GameFeatureAction_AddWidget` is commented out. The runtime Build.cs does not currently declare a full UMG/CommonUI framework. Do not assume CommonUI/UIExtension integration is implemented.
- Input: HodgeInputComponent/InputConfig/UserSettings and Main/Input IA/IMC assets. Preserve project input tags and mappings.
- Animation: HodgeAnimInstance uses GameplayTagBlueprintPropertyMap and GAS integration. Main/Character/Hero contains ABP_Pover_Base and linked animation-layer assets. An event-graph parser is not a full AnimGraph editor.
- Current UI examples are under `/Game/CodexText`: WBP_MainMenu, WBP_SurvivalHUD, hosts and blank menu/HUD maps. These task-specific examples use native engine parents. Their displayed survival data is sample data, not a completed gameplay system. Do not generalize the example folder/native-parent choice into a rule for all future UI without task context.
- Existing bridge: `Tools/UnrealMCP/server.py`, loopback port 55557, expected project identity Hodgepodge. Read available schemas and actual results. Connection may require the project editor to be open. Existing exposed graph tools do not imply complete UMG tree or animation state-machine editing.
- Read-only Blueprint CLI: `C:/Users/LocalUser/.codex/tools/bp-inspect/v1.0.0/bp-inspect.exe`. Local smoke tests decoded the menu travel calls and host BeginPlay. Partial metadata/animation output must not be interpreted as a complete asset audit.
- Installed knowledge skills are under `C:/Users/LocalUser/.codex/skills`: ue-project-context, ue-game-features, ue-gameplay-abilities, ue-ui-umg-slate, ue-input-system, ue-animation-system, unreal-bp-inspect. Generic examples require local UE5.5 API checks; installation is not compatibility certification.
- Verification: distinguish C++ build, Blueprint compile, PIE, multiplayer, and packaging results. Do not save unrelated dirty assets or stage/commit changes automatically.
