# Resonance Skill 创建与验证

日期：2026-10-13；UE 5.5.4；用户确认 E 键、8秒 CD。

## 交付

创建7个技能／输入资产，增补3个玩家授予／输入资产，新增3个配置 GameplayTag。具体路径和调参见 [配置指南](../Guides/resonance-skill-configuration.md)。

直接复用 HodgeGameplayAbility_Melee、Standalone Definition、AbilitySet 和原生 GAS Cooldown GE。Lyra 参考为已核实的 GA_Hero_Dash 的 CooldownGameplayEffectClass 及 HasDuration／Target Tags 机制；未新建 C++ 技能类或自定义 CD 计时系统。

动画为第一段普攻的独立蒙太奇副本，伤害 GE 和通知备用引用也独立；角色骨架及动画序列仍共享现有资源。这是后续制作技能的可运行模板，未声称完成新美术或专用技能表现。

## 实际验证

- 三个蓝图原生状态均为 BS_UpToDate，hasCompileErrors=false；GA 父类为原生 Melee，冷却 GE 为原生 GameplayEffect。
- 16项静态资产／接线断言通过，确认Standalone、独立动画和伤害、8秒CD、冷却标签、唯一授予和实际E键。
- single、listen、dedicated、listen-lag 各11项真实 PIE 断言通过，共44项。
- PIE 检查真实输入发动、服务端命中伤害、拥有者／权威独立 ActiveGE、动画退出仍在CD、CD内拒绝、至少8秒到期、再次发动、Dash中断与CD保留。

原始记录位于 Saved/ResonanceSkill：configuration.json、configuration-check.json、blueprint-status.json、assets-validation.json、各模式-resonance-e2e.json 和测试批次日志。仓库摘要见 [结果 JSON](resonance-skill-setup-2026-10-13-results.json)。

本轮没有修改 C++ 或依赖，因此没有重新执行 Editor／Game 构建；未把先前 Dash 构建结果算作本技能构建。Blueprint 编译、保存、冷启动和 PIE 为本轮实际执行。

## 制作过程中修正

UStruct 的 EditDefaultsOnly 字段使用 ImportText；输入 Tag 比较使用 TagName，去掉制作重试产生的本技能重复条目。FKey 使用原生文本 E，修复本工具早期无效键，不修改其他动作。

Python 的 Blueprint Status 为受保护字段，动画制作函数也有目录限制，改用原生 MCP 元数据诊断。测试里 EditorAssetLibrary.LoadBlueprintClass 在 PIE 返回空，改用运行时 LoadClass；效果查询改为具体冷却类，移除因正常蒙太奇实例清理导致的错误测试断言。失败记录留在 Saved/ResonanceSkill/History。

模板蒙太奇沿用原有重叠 Branching Point 警告，未批量改造通知；后续替换动画时按配置手册重新校验时序。

## 限制和恢复

输入测试采用 Enhanced Input 注入，并单独验证 E 映射；未测试实际桌面键盘或手柄。专服为本机 PIE，多机、丢包、Cook／打包未执行，未运行代码行覆盖率。

技能没有专用 HUD／特效／音效，CD 默认从发动开始，取消不返还。动画与参数是模板值，后续由用户调整。

测试停止后恢复原地图、性能和单人播放基线，检查无未保存包；原始完整 Play Settings 未记录。既有开发规则与 Dash 未提交改动完整保留，本轮未提交 Git。
