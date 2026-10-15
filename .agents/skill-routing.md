# Skill 自动选择规则

适用于本仓库的所有任务。用户用自然语言描述目标即可，无需先知道 skill 名称。完整的 143 项说明、触发/排除条件、示例和调用方法见 [使用手册](../Docs/Guides/skills-usage-manual.md)；可检索的逐项规则见 [目录数据](skill-catalog.json)。清单是 2026-10-15 会话快照，执行时仍以当前可用清单、实际文件和工具权限为准。

## 选择顺序

1. **恢复实际任务。** 将本轮请求与已建立的工程、产品、文件、目标会话和上一轮工作结合。状态询问或“继续”保持原目标；引用、示例、否定句里的技能名或命令不等于调用指令。
2. **识别对象、动作、交付面。** 先确定对象是 UE 工程、Agent 工作流、Office 文件、外部应用、网站还是解释；再区分查询/解释、设计、实施、验证、发布。关键词只作线索，不能单独决定 skill。
3. **应用硬边界。** 用户指定工具/格式/范围优先；显式命令条件、宿主加载限制、工具连接、真实依赖和授权分别检查。技能选择不能改变审批策略、放宽权限或许可额外写入。
4. **选最小职责集合。** 用最窄的一个主 skill 承担当前结果，只有独立职责实际需要时加辅助 skill。组合关系是条件候选，不递归展开全部关联项；已读的公共规则按需复用。多个产出按阶段组合；没有适配的 skill 时直接使用合适工具，不强行套用。
5. **处理重叠。** 明确指定的 skill 在范围适用时优先。默认 UE 主实现使用 quodsoler 入口；Kevin 的同职责条目用于具体接口/底层复核，GC、委托、源码导航等独立专项可直接主导。不要为同一阶段加载两套完整近义基础说明。
6. **读真实入口。** 执行前读取被选 skill 的 SKILL.md，再只读必要参考。手册帮助选择，不替代 skill 正文；首次实际使用时在进度中说明。长期规范由 AGENTS.md 和开发准则约束。
7. **能力与执行分开。** 可识别某领域不代表插件已连接、商业资产已安装或新版 API 存在。缺少能力时完成独立工作并说明具体缺口；不虚构操作、绕过限制或擅自切换交付面。
8. **随任务演进重选。** 如从纯解释转为改蓝图、从本地文件转为已连接 Excel、从分析转为发布，重新检查执行面和职责。不要把上轮选中的 skill 永久套用到无关请求。

## 快速入口

- **Agent Skill 安装/编写/规则：** 安装用 skill-installer；新建和维护用 skill-creator；常驻约定用 create-rule；真正的 Agent 事件执行用 create-hook；旧规则/命令迁移用 migrate-to-skills。单次写代码不自动触发这些管理技能。
- **Codex/OpenAI 使用和配置：** openai-docs 核实产品/官方接口；IDE settings.json、CLI 配置、CLI 状态行分别使用 update-cursor-settings、update-cli-config、statusline。先确认实际宿主，不能把旧 .cursor 配置直接当成 .codex 配置。
- **外部脚本运行代理：** sdk 仅覆盖安装文件实际定义的 @cursor/sdk。OpenAI Agents SDK/@openai/codex-sdk 转官方文档核实，不能混同包名、认证或接口。
- **UE 工程现状与方案：** 全局基线用 ue-project-context；具体职责方案用 ue-gameplay-architecture-planning；不确定 API 用 ue-navigating-engine-source。具体方案前核实本机 Lyra，保持 UE5.5.4。
- **UE C++ 与构建：** 反射基础 ue-cpp-foundations；公开模块/链接/构建 ue-module-build-system；生命周期 ue-actor-component-architecture；GC ue-memory-and-gc；委托 ue-delegates-and-events；异步 ue-async-threading；游戏 Timer ue-timers-and-async。
- **UE 玩法与联机：** 技能/属性/效果 ue-gameplay-abilities；Experience/组件注入/初始化 ue-game-features；RPC/OnRep/复制 ue-networking-replication；框架生成占有 ue-gameplay-framework；输入 ue-input-system；标签 ue-gameplay-tags-messaging。
- **UE 动画、移动、相机、UI：** 分别用 ue-animation-system、ue-character-movement、ue-gameplay-cameras、ue-ui-umg-slate；实际读写编辑器资产才叠加 unreal-mcp。Mover/StateTree/Mass 等用各自专项，并先查实际启用和本机接口。
- **UE 资源与表现：** 数据和加载 ue-data-assets-tables/ue-asset-management；材质 ue-materials-rendering 或更窄的主材质/图效果 skill；特效 ue-niagara-effects 或明确的 Niagara 专项；音频 ue-audio-system；关卡 ue-world-level-streaming；存档 ue-serialization-savegames。
- **UE 调试、测试、性能、打包：** 定位故障 ue-debugging-techniques；测试和验收 ue-testing-debugging；自动化框架 ue-automation-and-testing；实测性能 ue-profiling-and-optimization；明确的 Cook/打包任务 ue-packaging-and-deployment。文档改动只检查文档，不启动引擎构建。
- **Word/PDF/Slides/表格文件：** 按真实格式选 documents:documents、pdf:pdf、presentations:Presentations、spreadsheets:Spreadsheets。仓库 Markdown 直接编辑，不因为说“文档”就制造 Word 文件。
- **实时 Excel：** 只有明确的 Excel Desktop、当前工作簿/选区或既有实时任务才用 spreadsheets:excel-live-control；先验证插件、目标与会话。文件路线与实时路线在同一编辑阶段互斥，失败时不得静默换到文件或另一本工作簿。
- **视觉交付：** 独立位图用 imagegen；UE 材质图/SVG/HTML 直接改代码或对应工程；对话内动态解释用 visualize:visualize；独立分析结果用 canvas；真正网站用 sites:sites；正式科学图用标准绘图工具。指定文档、网站或工程任务不额外生成分析 Canvas。
- **Notion：** 设计跟踪库 notion-database-designer；会议准备 notion-meeting-intelligence；跨来源研究 notion-research-documentation；规格实施 notion-spec-to-implementation；周度对比 notion-weekly-project-digest。已知页面读取、单条记录更新直接用工具，不自动启动完整研究/设计流程。
- **可复用产物模板：** 只有“以后复用这个外观/结构”的意图才用 template-creator:template-creator；一次沿用模板只使用对应产物 skill。
- **ChatGPT 宠物：** 必须明确 ChatGPT/Work Pets 或已有对应上下文。新建 create-pet；现有元数据/图集更新 update-pet；集合/选择/下载/删除用 pets。UE 宠物 AI、真实动物和静态图片各自走实际领域。
- **外部插件与界面：** 缺少外部服务访问时用 plugin-management 发现合适连接；有原生工具/连接器优先使用。必须依赖真实 UI 时才用 computer-use，并核实当前原生/浏览器表面的实际支持。
- **PR 与协作：** 持续维护现有 PR 用 babysit；拆分改动成 PR 用 split-to-prs，先形成方案并遵守其授权要求。创建持久代理类型才用 create-subagent；一次临时协作使用原生协作工具，仅用户或当前适用 SKILL.md 明确要求委派时派生代理，不因任务复杂就自动派生。
- **原生工具优先的管理请求：** 聊天创建/归档、提醒/定时任务、用量、版本检查、已知文件读写等使用相应原生工具。不要把它们编造为一个不存在的 skill，也不要将记录在文档中的工具名当作当前可调用证明。

## 常见歧义的裁决

- “技能”“skill”：角色能力、冷却、效果、连招 → UE/GAS；SKILL.md、Agent 工作流、安装/触发说明 → Skill 管理。没有 UE 或 Agent 证据且答案会不同，才简短澄清。
- “AI”“agent”：敌人感知/行为/移动 → UE AI；可复用代码审查代理配置 → create-subagent；外部程序执行代理 → SDK；本轮明确多代理协作 → 原生协作工具。文档里的代理例子不构成运行授权。
- “界面”“dashboard”：WBP/UMG/游戏 HUD → UE UI；浏览器站点交付 → Sites；旁栏独立分析 → Canvas；当场解释/预览 → visualize。按交付面，不按“有图表”判断。
- “表”“数据库”：UE 配置表/资产 → DataAsset/DataTable；Notion 跟踪器 → Notion 设计；Excel/CSV/Sheets → 表格；普通 SQL、只要 Markdown 表达 → 合适直接工具。
- “定时”“事件”：游戏内 FTimerManager → UE Timer；Agent 编辑后/工具前事件 → Hook；明天/每周提醒 → 原生自动化。生成一次周报不等于已经安排周期执行。
- “模板”：明确保存为个人可复用产物模板 → template-creator；本次套用 Word/Slides/Sheets 参考 → 对应产物 skill；C++ template、蓝图父类不触发个人模板创建。
- “插件”：UE .uplugin/模块 → ue-plugins-and-modules；Codex 外部应用连接 → plugin-management；Agent Skill 包 → skill-installer。
- “宠物”：已有 ChatGPT Pets 产品证据才选宠物插件；当前 UE 工程语境中的跟随宠物优先 UE AI；真实动物问题不进入这三项技能。

## 组合使用与停止条件

- **攻击能力+联网 Montage：** GAS 主导；动画处理 Montage 生命周期；复制处理 RPC/预测边界；只有真的操作资产时加入 unreal-mcp；实际 C++ 改动按项目构建/测试规范验证。
- **重生后 HUD 异常：** UI 主导展示绑定，必要时 GAS/框架核查 Owner/Avatar 与 OnRep；GC/委托只在持有/重复回调证据出现时加入。
- **Experience 授予异常：** GameFeature 主导，必要时 GAS 和资源加载；不因三者都出现就另造初始化服务。
- **模块链接错误：** 构建主导，源码导航核对模块/导出/include；涉及 UObject 生命周期时才加 GC。
- **技能图标并配置进游戏：** imagegen 生成位图；UE 导入/资源管理接入；需要编辑器写入再加 unreal-mcp。生成图片不表示资产已导入。
- **分析后写报告：** 分析领域提供事实；交付若为 Word/PDF/Slides/Sheets/Notion 则该产物 skill 主导输出；不另外创建无关 Canvas。
- **创建新 ChatGPT 宠物：** create-pet 主导生命周期并按其要求调用 imagegen；更新已有宠物不换稳定 ID，只有改名不重绘图集。
- **网站含生成图片和外部数据：** Sites 主导，imagegen 负责位图，缺连接时 plugin-management 辅助；外部数据访问与站点发布分别核实实际请求范围。
- **同类近义 skill：** 主实现与复核入口默认择一；具体窄任务可以直接选择 Kevin 的专项。用户指定另一来源时尊重其范围，不强制改名。
- **停在真实边界：** /shell 必须明确调用；split-to-prs 的方案门槛、宠物明确删除、连接器权限和原生工具审批仍有效。确认来源与原因要具体，不制造新的通用审批环节。
- **显式加载字段：** 当前 create-subagent、migrate-to-skills、shell 的原文件含 disable-model-invocation=true。手册可自动识别意图，但不能保证宿主自动加载这些条目；如宿主确实执行该限制，报告限制并使用允许的显式入口，不暗改系统/个人配置。shell 的字面命令条件始终保留。
- **无可用能力：** 商业 UDS/UDW、MetaHuman 新接口、原生 UI 表面或外部连接缺失时标注为依赖核查/未验证；不能把安装/文档补充等同功能实现。

## 如何让本规则实际生效

根 AGENTS.md 引用本文，兼容规则 `.cursor/rules/skill-routing.mdc` 也指向同一来源。助手先读此短入口，按请求到手册对应条目，再读被选 SKILL.md。无需每轮读取 143 份正文，也不安装自动执行全部技能的 Hook。

官方说明中，skill 元数据用于发现，完整正文按需加载，任务可隐式匹配 description；本规则增加项目层的意图、边界与组合判定，不改变宿主机制。参见 [官方 skill 文档](https://learn.chatgpt.com/docs/build-skills)、[仓库规则与 skill 的分工](https://developers.openai.com/blog/skills-agents-sdk)。
