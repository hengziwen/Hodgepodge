# 全部可用 Skill 使用手册

快照日期：2026-10-15；适用：Hodgepodge 当前 Codex 会话。

本手册覆盖 **143 个可用 skill**：项目 UE skill 108 个，系统/个人/插件 skill 35 个。每项说明功能、适用场景、全部必要触发条件、识别信号、排除条件、前置能力、组合方式、调用示例和验收边界。

这里的“可用”是本次会话清单提供且已读取到真实 SKILL.md；不是“所有外部工具已经连接”，也不是“所有上游示例已经移植”。磁盘上的未公开内部技能不纳入，例如仅存在于系统目录、未在本次清单公开的 review-agent；`.agents/skill-sources/` 中的上游副本不重复计数。

## 1 阅读与调用方法

- **默认只说目标。** 例如“角色重生后生命条不刷新，修一下”，助手先识别 UE HUD、重生、ASC 绑定，再选择 UI 主 skill 与必要的 GAS/委托辅助，不要求你提供 skill 名称。
- **需要指定时用自然语言。** 每项都给出“请使用 完整标识 完成：具体任务”的例子。普通 skill 在支持的界面也可用 `$skill-name`；插件以当前选择器中的完整条目为准。不要假定冒号命名可以直接拼成工具名或所有界面都接受同一种快捷语法。
- **名称是选择器，不是执行函数。** 助手读选中项的 SKILL.md 后调用当前真正存在的工具；例如 `unreal-mcp` 在本项目走实际桥接，不是把字符串当作接口执行。
- **全部必要条件必须同时满足。** “识别信号”只是线索，不是看到一个词就触发；不要求逐字命中，可以根据代码路径、附件、上一轮对象与请求含义判断。
- **显式指定仍需范围适用。** 指定 skill 不能凭空增加工具、权限或商业资产。若指定项与目标明显不符，解释冲突并按真实目标完成可独立的工作；只有影响结果的缺失信息才澄清。
- **调用前重读必要正文。** 本手册负责选对工作流；具体工具顺序、脚本、渲染、配对清理和产物验证以真实 skill、项目规则和更高层指令为准。

### 1.1 维护入口

- [短版自动选择规则](../../.agents/skill-routing.md)：任务开始时读取，避免全量装入 143 项细节。
- [目录数据](../../.agents/skill-catalog.json)：逐项规则、来源路径/摘要和组合关系的维护源。
- [UE 安装与版本适配](../../.agents/UE55_SKILLS.md)：108 项来源、重名和 UE5.5.4 限制。
- [仓库 AGENTS.md](../../AGENTS.md)：自动选择的项目入口与长期工程约定。

## 2 自动选择与组合规则

### 选择顺序

1. **恢复实际任务。** 将本轮请求与已建立的工程、产品、文件、目标会话和上一轮工作结合。状态询问或“继续”保持原目标；引用、示例、否定句里的技能名或命令不等于调用指令。
2. **识别对象、动作、交付面。** 先确定对象是 UE 工程、Agent 工作流、Office 文件、外部应用、网站还是解释；再区分查询/解释、设计、实施、验证、发布。关键词只作线索，不能单独决定 skill。
3. **应用硬边界。** 用户指定工具/格式/范围优先；显式命令条件、宿主加载限制、工具连接、真实依赖和授权分别检查。技能选择不能改变审批策略、放宽权限或许可额外写入。
4. **选最小职责集合。** 用最窄的一个主 skill 承担当前结果，只有独立职责实际需要时加辅助 skill。组合关系是条件候选，不递归展开全部关联项；已读的公共规则按需复用。多个产出按阶段组合；没有适配的 skill 时直接使用合适工具，不强行套用。
5. **处理重叠。** 明确指定的 skill 在范围适用时优先。默认 UE 主实现使用 quodsoler 入口；Kevin 的同职责条目用于具体接口/底层复核，GC、委托、源码导航等独立专项可直接主导。不要为同一阶段加载两套完整近义基础说明。
6. **读真实入口。** 执行前读取被选 skill 的 SKILL.md，再只读必要参考。手册帮助选择，不替代 skill 正文；首次实际使用时在进度中说明。长期规范由 AGENTS.md 和开发准则约束。
7. **能力与执行分开。** 可识别某领域不代表插件已连接、商业资产已安装或新版 API 存在。缺少能力时完成独立工作并说明具体缺口；不虚构操作、绕过限制或擅自切换交付面。
8. **随任务演进重选。** 如从纯解释转为改蓝图、从本地文件转为已连接 Excel、从分析转为发布，重新检查执行面和职责。不要把上轮选中的 skill 永久套用到无关请求。

### 快速入口

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

### 常见歧义的裁决

- “技能”“skill”：角色能力、冷却、效果、连招 → UE/GAS；SKILL.md、Agent 工作流、安装/触发说明 → Skill 管理。没有 UE 或 Agent 证据且答案会不同，才简短澄清。
- “AI”“agent”：敌人感知/行为/移动 → UE AI；可复用代码审查代理配置 → create-subagent；外部程序执行代理 → SDK；本轮明确多代理协作 → 原生协作工具。文档里的代理例子不构成运行授权。
- “界面”“dashboard”：WBP/UMG/游戏 HUD → UE UI；浏览器站点交付 → Sites；旁栏独立分析 → Canvas；当场解释/预览 → visualize。按交付面，不按“有图表”判断。
- “表”“数据库”：UE 配置表/资产 → DataAsset/DataTable；Notion 跟踪器 → Notion 设计；Excel/CSV/Sheets → 表格；普通 SQL、只要 Markdown 表达 → 合适直接工具。
- “定时”“事件”：游戏内 FTimerManager → UE Timer；Agent 编辑后/工具前事件 → Hook；明天/每周提醒 → 原生自动化。生成一次周报不等于已经安排周期执行。
- “模板”：明确保存为个人可复用产物模板 → template-creator；本次套用 Word/Slides/Sheets 参考 → 对应产物 skill；C++ template、蓝图父类不触发个人模板创建。
- “插件”：UE .uplugin/模块 → ue-plugins-and-modules；Codex 外部应用连接 → plugin-management；Agent Skill 包 → skill-installer。
- “宠物”：已有 ChatGPT Pets 产品证据才选宠物插件；当前 UE 工程语境中的跟随宠物优先 UE AI；真实动物问题不进入这三项技能。

### 组合使用与停止条件

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

### 如何让本规则实际生效

根 AGENTS.md 引用本文，兼容规则 `.cursor/rules/skill-routing.mdc` 也指向同一来源。助手先读此短入口，按请求到手册对应条目，再读被选 SKILL.md。无需每轮读取 143 份正文，也不安装自动执行全部技能的 Hook。

官方说明中，skill 元数据用于发现，完整正文按需加载，任务可隐式匹配 description；本规则增加项目层的意图、边界与组合判定，不改变宿主机制。参见 [官方 skill 文档](https://learn.chatgpt.com/docs/build-skills)、[仓库规则与 skill 的分工](https://developers.openai.com/blog/skills-agents-sdk)。

## 3 常见请求判定示例

这些是后续行为检查和人工复核的正反场景，不表示本次执行了全部技能，也不是自然语言分类准确率测试。

1. **请求：**“给角色增加火球技能和冷却”。**判定：**ue-gameplay-abilities 主导；不选择 Agent Skill 创建。若还明确需要图标，才加 imagegen。
2. **请求：**“把团队代码审查流程保存成一个 skill”。**判定：**skill-creator 主导；需要长期强制约束才补 create-rule；不选择 GAS。
3. **请求：**“继续刚才的工作”。**判定：**继承未完成目标和上一轮执行面；不因句子没有领域词重新开始其他任务。
4. **请求：**“只是解释示例 /shell git status，不要执行”。**判定：**不调用 shell、不执行命令；引用和否定语义优先。
5. **请求：**“当前 Excel 的这个选区加一列公式”。**判定：**excel-live-control；先验证同一工作簿与连接，不静默导出 XLSX。
6. **请求：**“生成一份可下载的 .xlsx”。**判定：**文件型 Spreadsheets；不要求用户打开 Excel 桌面程序。
7. **请求：**“在仓库写 Markdown 文档”。**判定：**普通文件工具；可按文档内容用领域 skill，但不调用 Word 渲染，也不创建无关 Canvas。
8. **请求：**“根据模板做一份 Word”。**判定：**documents；不是 template-creator。只有要求以后复用时才创建模板 skill。
9. **请求：**“给我一个网站，只在本地保存”。**判定：**Sites local-only；不注册或部署，不额外要求发布批准。
10. **请求：**“在聊天里演示延迟对技能预测的影响”。**判定：**visualize；需要 UE/GAS 事实时用其专项核实，不因此建立托管站点。
11. **请求：**“制作正式论文的 SVG 统计图”。**判定：**标准绘图工具；不把内嵌互动展示或图片生成当成可导出科学绘图。
12. **请求：**“给 UE 游戏加一个宠物跟随 AI”。**判定：**UE AI/移动；不调用 ChatGPT Pets 插件。
13. **请求：**“把 ChatGPT 现有伙伴改名”。**判定：**update-pet 元数据路线；不生成图集、不新建替代 ID、不自动激活。
14. **请求：**“把 ChatGPT 伙伴隐藏起来”。**判定：**pets 的取消选择路线；不解释成删除自定义宠物。
15. **请求：**“在 Notion 更新一条已知任务状态”。**判定：**直接已连接工具；不新建任务库、不启动全量跨来源研究。
16. **请求：**“根据 Notion 的多个方案比较本周变化”。**判定：**research-documentation 或 weekly-project-digest 按研究/报告期目标择主；不是单页摘要。
17. **请求：**“创建一个以后复用的代码审查子代理”。**判定：**create-subagent 配置；先尊重宿主显式加载限制，不等同当场运行代理。
18. **请求：**“请派三个代理分别审查这三个模块”。**判定：**原生协作工具属于明确的本轮分工；不为此自动保存持久子代理类型。
19. **请求：**“每周一提醒我跑测试”。**判定：**原生自动化工具；不是 UE FTimerManager，也不是 afterFileEdit Hook。
20. **请求：**“安装一个 .uplugin”。**判定：**UE 插件/模块；不是 Agent Skill 安装或 Codex 外部应用连接。
21. **请求：**“5.8 文档中的这个接口在 5.5 能用吗”。**判定：**ue-navigating-engine-source，按实际功能补最窄领域；不升级引擎来满足示例。
22. **请求：**“已有 UDS 的天空时间不同步”。**判定：**ue-uds-time 先验证商业资产/版本；复制专项处理权威时间。没有资产则只给具体依赖缺口。
23. **请求：**“解释这份 PR 的 CI 状态”。**判定：**只读原生 GitHub 工具；不把一次状态查询扩大为 babysit 持续维护。
24. **请求：**“拆成三个 PR，先给我具体方案”。**判定：**split-to-prs 先给可审查方案；未获得所需授权前不建分支、提交、推送或开 PR。

## 4 查阅索引

- [5.1 Agent Skill 与长期约定（7）](#group-1)
- [5.2 助手产品 配置 SDK 与 PR（9）](#group-2)
- [5.3 图像 视觉 界面与网站（5）](#group-3)
- [5.4 文档 表格 幻灯片与模板（6）](#group-4)
- [5.5 Notion 工作流（5）](#group-5)
- [5.6 ChatGPT 动画宠物（3）](#group-6)
- [5.7 UE 基础 架构与构建（23）](#group-7)
- [5.8 UE 玩法 输入 网络与相机（14）](#group-8)
- [5.9 UE 动画 骨架与网格（4）](#group-9)
- [5.10 UE 数据 资源 世界与存档（8）](#group-10)
- [5.11 UE 蓝图 编辑器与 UI（8）](#group-11)
- [5.12 UE AI Mass 与 StateTree（4）](#group-12)
- [5.13 UE 渲染 特效 音频与几何（23）](#group-13)
- [5.14 UE 调试 测试 性能与打包（6）](#group-14)
- [5.15 UE 外部内容依赖核查（18）](#group-15)

### 按名称检索

- [babysit](#skill-babysit) — 持续维护 PR 可合并状态。
- [canvas](#skill-canvas) — 独立分析 Canvas。
- [computer-use:computer-use](#skill-computer-use-computer-use) — Windows 应用界面操作。
- [create-hook](#skill-create-hook) — Agent 事件 Hook。
- [create-rule](#skill-create-rule) — 常驻开发规则。
- [create-skill](#skill-create-skill) — 兼容的 skill 编写指引。
- [create-subagent](#skill-create-subagent) — 持久子代理类型配置。
- [create-toolset](#skill-create-toolset) — UE5.5 编辑器工具扩展。
- [documents:documents](#skill-documents-documents) — Word 与 DOCX 文档。
- [imagegen](#skill-imagegen) — 位图生成与编辑。
- [kb-ue-animation-system](#skill-kb-ue-animation-system) — 动画与 Montage。
- [kb-ue-gameplay-framework](#skill-kb-ue-gameplay-framework) — Gameplay Framework。
- [migrate-to-skills](#skill-migrate-to-skills) — 旧规则和命令迁移。
- [notion:notion-database-designer](#skill-notion-notion-database-designer) — Notion 数据库设计。
- [notion:notion-meeting-intelligence](#skill-notion-notion-meeting-intelligence) — Notion 会议准备与行动项。
- [notion:notion-research-documentation](#skill-notion-notion-research-documentation) — Notion 跨来源研究。
- [notion:notion-spec-to-implementation](#skill-notion-notion-spec-to-implementation) — Notion 规格到实施。
- [notion:notion-weekly-project-digest](#skill-notion-notion-weekly-project-digest) — Notion 周期项目摘要。
- [openai-docs](#skill-openai-docs) — OpenAI 与 Codex 官方文档。
- [pdf:pdf](#skill-pdf-pdf) — PDF 与可填写表单。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management) — 应用插件发现与连接管理。
- [presentations:Presentations](#skill-presentations-presentations) — 演示文稿与幻灯片。
- [sdk](#skill-sdk) — 已安装 @cursor/sdk 集成指引。
- [shell](#skill-shell) — 显式原样 shell 请求。
- [sites:sites](#skill-sites-sites) — 网站创建、修改与托管。
- [skill-creator](#skill-skill-creator) — 创建和维护 Agent Skill。
- [skill-installer](#skill-skill-installer) — 安装 Agent Skill。
- [split-to-prs](#skill-split-to-prs) — 拆分为小型 PR。
- [spreadsheets:excel-live-control](#skill-spreadsheets-excel-live-control) — 已连接 Excel 实时控制。
- [spreadsheets:Spreadsheets](#skill-spreadsheets-spreadsheets) — 文件型工作簿与 Google Sheets。
- [statusline](#skill-statusline) — CLI 状态行。
- [template-creator:template-creator](#skill-template-creator-template-creator) — 可复用个人产物模板 Skill。
- [ue-actor-component-architecture](#skill-ue-actor-component-architecture) — Actor 与组件生命周期。
- [ue-actors-and-components](#skill-ue-actors-and-components) — Actor 与组件生命周期。
- [ue-ai-and-navigation](#skill-ue-ai-and-navigation) — AI 与导航。
- [ue-ai-navigation](#skill-ue-ai-navigation) — AI 与导航。
- [ue-animation-system](#skill-ue-animation-system) — 动画与 Montage。
- [ue-asset-management](#skill-ue-asset-management) — 数据资产与资源加载。
- [ue-async-threading](#skill-ue-async-threading) — Timer、异步与线程。
- [ue-audio-and-metasounds](#skill-ue-audio-and-metasounds) — 音频与 MetaSound。
- [ue-audio-system](#skill-ue-audio-system) — 音频与 MetaSound。
- [ue-auto-landscape-materials](#skill-ue-auto-landscape-materials) — Landscape、RVT 与植被。
- [ue-automation-and-testing](#skill-ue-automation-and-testing) — 自动化、调试和验证。
- [ue-blueprint-cpp-integration](#skill-ue-blueprint-cpp-integration) — C++ 反射与蓝图接口。
- [ue-blueprint-cpp-interop](#skill-ue-blueprint-cpp-interop) — C++ 反射与蓝图接口。
- [ue-blueprint-fundamentals](#skill-ue-blueprint-fundamentals) — 蓝图图表与编译。
- [ue-character-and-movement](#skill-ue-character-and-movement) — CharacterMovement。
- [ue-character-movement](#skill-ue-character-movement) — CharacterMovement。
- [ue-coding-standards](#skill-ue-coding-standards) — C++ 反射与蓝图接口。
- [ue-control-rig-and-ik](#skill-ue-control-rig-and-ik) — Control Rig 与 IK。
- [ue-core-types-and-containers](#skill-ue-core-types-and-containers) — 核心类型、日志和容器。
- [ue-cpp-foundations](#skill-ue-cpp-foundations) — C++ 反射与蓝图接口。
- [ue-cpp-fundamentals](#skill-ue-cpp-fundamentals) — C++ 反射与蓝图接口。
- [ue-data-assets-tables](#skill-ue-data-assets-tables) — 数据资产与资源加载。
- [ue-data-driven-design](#skill-ue-data-driven-design) — 数据资产与资源加载。
- [ue-debugging-techniques](#skill-ue-debugging-techniques) — 自动化、调试和验证。
- [ue-delegates-and-events](#skill-ue-delegates-and-events) — 委托与事件。
- [ue-editor-scripting-and-python](#skill-ue-editor-scripting-and-python) — 编辑器工具与 Python。
- [ue-editor-tools](#skill-ue-editor-tools) — 编辑器工具与 Python。
- [ue-enhanced-input](#skill-ue-enhanced-input) — Enhanced Input。
- [ue-game-features](#skill-ue-game-features) — GameFeature、Experience 与 Init State。
- [ue-game-thread-performance](#skill-ue-game-thread-performance) — 性能分析。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities) — GAS 能力、属性和效果。
- [ue-gameplay-ability-system](#skill-ue-gameplay-ability-system) — GAS 能力、属性和效果。
- [ue-gameplay-architecture-planning](#skill-ue-gameplay-architecture-planning) — 玩法架构方案。
- [ue-gameplay-cameras](#skill-ue-gameplay-cameras) — 相机与 CameraMode。
- [ue-gameplay-framework](#skill-ue-gameplay-framework) — Gameplay Framework。
- [ue-gameplay-tags](#skill-ue-gameplay-tags) — GameplayTag 与消息。
- [ue-gameplay-tags-messaging](#skill-ue-gameplay-tags-messaging) — GameplayTag 与消息。
- [ue-importing-content](#skill-ue-importing-content) — 资源导入。
- [ue-input-system](#skill-ue-input-system) — Enhanced Input。
- [ue-landscape-and-foliage](#skill-ue-landscape-and-foliage) — Landscape、RVT 与植被。
- [ue-landscape-rvt-and-grass](#skill-ue-landscape-rvt-and-grass) — Landscape、RVT 与植被。
- [ue-levels-and-world-partition](#skill-ue-levels-and-world-partition) — 世界分区与关卡流送。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen) — 渲染、Lumen 与性能。
- [ue-logging-and-assertions](#skill-ue-logging-and-assertions) — 核心类型、日志和容器。
- [ue-mass-entity](#skill-ue-mass-entity) — Mass Entity。
- [ue-master-material-authoring](#skill-ue-master-material-authoring) — 材质与着色器。
- [ue-material-graph-effects](#skill-ue-material-graph-effects) — 材质与着色器。
- [ue-materials-and-shaders](#skill-ue-materials-and-shaders) — 材质与着色器。
- [ue-materials-rendering](#skill-ue-materials-rendering) — 材质与着色器。
- [ue-memory-and-gc](#skill-ue-memory-and-gc) — 对象生命周期与 GC。
- [ue-meshes-static-and-skeletal](#skill-ue-meshes-static-and-skeletal) — 网格与骨骼资源。
- [ue-metahuman-animator](#skill-ue-metahuman-animator) — MetaHuman 可用性与旧版流程。
- [ue-metahuman-creator](#skill-ue-metahuman-creator) — MetaHuman 可用性与旧版流程。
- [ue-metahuman-live-link](#skill-ue-metahuman-live-link) — MetaHuman 可用性与旧版流程。
- [ue-module-and-build-system](#skill-ue-module-and-build-system) — 模块与 UBT 构建。
- [ue-module-build-system](#skill-ue-module-build-system) — 模块与 UBT 构建。
- [ue-mover](#skill-ue-mover) — 实验性 Mover。
- [ue-mover-movement-system](#skill-ue-mover-movement-system) — 实验性 Mover。
- [ue-nanite-and-rendering](#skill-ue-nanite-and-rendering) — 渲染、Lumen 与性能。
- [ue-navigating-engine-source](#skill-ue-navigating-engine-source) — 引擎源码定位。
- [ue-networking-and-replication](#skill-ue-networking-and-replication) — 复制与服务器权限。
- [ue-networking-replication](#skill-ue-networking-replication) — 复制与服务器权限。
- [ue-niagara-custom-modules](#skill-ue-niagara-custom-modules) — Niagara 特效。
- [ue-niagara-effects](#skill-ue-niagara-effects) — Niagara 特效。
- [ue-niagara-flamethrower](#skill-ue-niagara-flamethrower) — Niagara 特效。
- [ue-niagara-mesh-disintegration](#skill-ue-niagara-mesh-disintegration) — Niagara 特效。
- [ue-niagara-renderers-and-materials](#skill-ue-niagara-renderers-and-materials) — Niagara 特效。
- [ue-niagara-sequenced-effects](#skill-ue-niagara-sequenced-effects) — Niagara 特效。
- [ue-niagara-vfx](#skill-ue-niagara-vfx) — Niagara 特效。
- [ue-packaging-and-deployment](#skill-ue-packaging-and-deployment) — Cook、打包与部署。
- [ue-physics-and-chaos](#skill-ue-physics-and-chaos) — 碰撞与 Chaos。
- [ue-physics-collision](#skill-ue-physics-collision) — 碰撞与 Chaos。
- [ue-plugins-and-modules](#skill-ue-plugins-and-modules) — 模块与 UBT 构建。
- [ue-procedural-generation](#skill-ue-procedural-generation) — PCG 与程序化几何。
- [ue-profiling-and-optimization](#skill-ue-profiling-and-optimization) — 性能分析。
- [ue-project-context](#skill-ue-project-context) — 项目上下文核查。
- [ue-project-structure](#skill-ue-project-structure) — 模块与 UBT 构建。
- [ue-save-and-load](#skill-ue-save-and-load) — 存档与序列化。
- [ue-sequencer-and-cinematics](#skill-ue-sequencer-and-cinematics) — Sequencer 与渲染队列。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics) — Sequencer 与渲染队列。
- [ue-serialization-savegames](#skill-ue-serialization-savegames) — 存档与序列化。
- [ue-state-trees](#skill-ue-state-trees) — StateTree。
- [ue-subsystems](#skill-ue-subsystems) — Subsystem 生命周期。
- [ue-testing-debugging](#skill-ue-testing-debugging) — 自动化、调试和验证。
- [ue-timers-and-async](#skill-ue-timers-and-async) — Timer、异步与线程。
- [ue-uds-cinematics-rendering](#skill-ue-uds-cinematics-rendering) — UDS/UDW 商业资产接入核查。
- [ue-uds-clouds](#skill-ue-uds-clouds) — UDS/UDW 商业资产接入核查。
- [ue-uds-fog-and-atmosphere](#skill-ue-uds-fog-and-atmosphere) — UDS/UDW 商业资产接入核查。
- [ue-uds-lighting-and-shadows](#skill-ue-uds-lighting-and-shadows) — UDS/UDW 商业资产接入核查。
- [ue-uds-modifiers-configs-state](#skill-ue-uds-modifiers-configs-state) — UDS/UDW 商业资产接入核查。
- [ue-uds-performance-mobile-troubleshooting](#skill-ue-uds-performance-mobile-troubleshooting) — UDS/UDW 商业资产接入核查。
- [ue-uds-setup-and-modes](#skill-ue-uds-setup-and-modes) — UDS/UDW 商业资产接入核查。
- [ue-uds-simulation](#skill-ue-uds-simulation) — UDS/UDW 商业资产接入核查。
- [ue-uds-sun-moon-stars](#skill-ue-uds-sun-moon-stars) — UDS/UDW 商业资产接入核查。
- [ue-uds-time](#skill-ue-uds-time) — UDS/UDW 商业资产接入核查。
- [ue-udw-material-and-screen-effects](#skill-ue-udw-material-and-screen-effects) — UDS/UDW 商业资产接入核查。
- [ue-udw-particles-lightning-wind-sounds](#skill-ue-udw-particles-lightning-wind-sounds) — UDS/UDW 商业资产接入核查。
- [ue-udw-random-seasons-temperature](#skill-ue-udw-random-seasons-temperature) — UDS/UDW 商业资产接入核查。
- [ue-udw-setup-and-state](#skill-ue-udw-setup-and-state) — UDS/UDW 商业资产接入核查。
- [ue-udw-spatial-weather](#skill-ue-udw-spatial-weather) — UDS/UDW 商业资产接入核查。
- [ue-ui-umg-slate](#skill-ue-ui-umg-slate) — UMG、Slate 与 CommonUI。
- [ue-umg-and-slate](#skill-ue-umg-and-slate) — UMG、Slate 与 CommonUI。
- [ue-world-level-streaming](#skill-ue-world-level-streaming) — 世界分区与关卡流送。
- [unreal-mcp](#skill-unreal-mcp) — UE5.5 编辑器 MCP 操作。
- [unreal-skill](#skill-unreal-skill) — UE5.5 项目工作流 Skill 编写。
- [update-cli-config](#skill-update-cli-config) — CLI 配置修改。
- [update-cursor-settings](#skill-update-cursor-settings) — IDE settings.json。
- [visualize:visualize](#skill-visualize-visualize) — 对话中的交互解释。
- [work-pets:create-pet](#skill-work-pets-create-pet) — 创建 ChatGPT 动画宠物。
- [work-pets:pets](#skill-work-pets-pets) — 管理 ChatGPT 宠物集合。
- [work-pets:update-pet](#skill-work-pets-update-pet) — 更新现有 ChatGPT 宠物。

## 5 逐项使用说明


<a id="group-1"></a>

### 5.1 Agent Skill 与长期约定

<a id="skill-skill-creator"></a>

#### skill-creator

**功能：**设计或更新具有明确职责和触发范围的 SKILL.md、参考资料、脚本与可选 UI 元数据。

**适用场景：**需要可复用的 Agent 工作流、skill 改写或安装后的项目适配。

**自动触发条件（全部满足）：**

- 目标是创建或维护 Agent Skill，而非游戏能力。
- 需要描述何时选择、如何执行或验证工作流。

**识别信号（可按语义匹配）：**`SKILL.md`、`做一个 skill`、`可复用工作流`、`适配 skill`、`修改触发条件`。

**不触发与边界：**

- 游戏中的技能、连招、冷却等走 GAS/玩法技能。
- 仅安装上游 skill 走 skill-installer。
- 常驻规范走 create-rule，复用外观模板走 template-creator。

**前置条件与实际可用性：**读取目标现有内容和真实任务约束；不要为普通 skill 默认禁止自动选择。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-installer](#skill-skill-installer)：需要导入上游或安装到发现目录时使用；安装不等于完成项目/API 适配。
- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。
- [unreal-skill](#skill-unreal-skill)：仅任务还需要 `unreal-skill` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [template-creator:template-creator](#skill-template-creator-template-creator)：仅用户明确要把参考样式保存为今后复用的个人模板 skill 时使用；一次套模板不触发。

**调用方式与正反例：**

- 自动选择示例：“把我们的 UE 构建检查做成可自动选择的 skill。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 skill-creator 完成：把我们的 UE 构建检查做成可自动选择的 skill。”
- 不触发示例：“给角色增加火球技能。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**校验 frontmatter、名称、链接、触发正反例；结构通过不等于执行质量已验证。

**真实入口与来源：**[系统 SKILL.md](C:/Users/Kenwen/.codex/skills/.system/skill-creator/SKILL.md)。

<a id="skill-skill-installer"></a>

#### skill-installer

**功能：**列出可安装 skill，或将 curated/GitHub skill 安装到指定发现目录。

**适用场景：**查找已安装状态、从仓库导入、为项目或个人安装 skill。

**自动触发条件（全部满足）：**

- 任务包含查找可安装来源或实际安装意图。
- 安装对象是 Agent Skill 包，而非 UE 引擎插件。

**识别信号（可按语义匹配）：**`安装 skill`、`GitHub skill`、`有哪些可安装`、`skill-installer`、`导入 SKILL.md`。

**不触发与边界：**

- 编写技能正文走 skill-creator。
- Codex 外部应用连接走 plugin-management。
- UE .uplugin 安装属于 UE 模块/插件任务，不是 Agent Skill 安装。

**前置条件与实际可用性：**使用该 skill 的安装脚本；指定位置时遵守位置，已有目录不覆盖。网络失败明确诊断，保留许可与版本。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“把这个 GitHub 仓库中的 skill 安装到当前项目。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 skill-installer 完成：把这个 GitHub 仓库中的 skill 安装到当前项目。”
- 不触发示例：“给 UE 工程安装一个 .uplugin。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**确认安装路径、实际文件、元数据与重复名称；说明何时能被会话发现。

**真实入口与来源：**[系统 SKILL.md](C:/Users/Kenwen/.codex/skills/.system/skill-installer/SKILL.md)。

<a id="skill-create-hook"></a>

#### create-hook

**功能：**为 agent 事件创建 hooks.json 与命令/提示 hook，实现事件前后审计、过滤或处理。

**适用场景：**需要编辑后格式化、工具调用前检查、会话事件处理等真实事件触发机制。

**自动触发条件（全部满足）：**

- 目标是运行在 Agent 事件上的程序行为。
- 明确事件、作用域及需要的处理逻辑。

**识别信号（可按语义匹配）：**`hooks.json`、`afterFileEdit`、`preToolUse`、`编辑后自动`、`调用工具前检查`。

**不触发与边界：**

- 自然语言常驻规范走 create-rule。
- 每周定时或稍后提醒走自动化工具。
- UE 动画 Notify/输入事件走 UE 专项，不是 Agent hook。

**前置条件与实际可用性：**先核实当前宿主支持的事件/配置路径，不照抄技能中旧 .cursor 路径；失败行为、过滤范围和外部程序需可验证。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“每次编辑项目 Python 后自动执行格式化检查。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 create-hook 完成：每次编辑项目 Python 后自动执行格式化检查。”
- 不触发示例：“下周一提醒我测试。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**检查 JSON、可执行依赖、真实事件触发和失败路径；不能只保存配置后称已生效。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/create-hook/SKILL.md)。

<a id="skill-create-rule"></a>

#### create-rule

**功能：**创建或修改 AGENTS.md、RULE.md 或 .mdc，使长期约束在适当范围内持续生效。

**适用场景：**需要项目约定、文件类型规范、AI 长期行为或规则入口。

**自动触发条件（全部满足）：**

- 用户需要跨请求持续遵守的规则。
- 明确全项目或具体文件的作用范围。

**识别信号（可按语义匹配）：**`以后都`、`开发约定`、`AGENTS.md`、`.cursor/rules`、`中文注释规范`、`自动选择规则`。

**不触发与边界：**

- 一次性工作流详情走 skill-creator。
- 实际事件执行走 create-hook。
- UE GameplayTag 规则属于业务配置。

**前置条件与实际可用性：**推断已有明确范围，读取当前规则并保留已有约定；作用域不清且影响行为时才澄清。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [create-hook](#skill-create-hook)：仅需要真实 Agent 事件触发行为时使用，不把定时提醒或普通规范实现为 hook。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“以后根据我的请求自动选择合适的 skill。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 create-rule 完成：以后根据我的请求自动选择合适的 skill。”
- 不触发示例：“今天帮我运行一次构建。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对规则链接、作用域、冲突和真实入口；不把文档当作平台权限修改。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/create-rule/SKILL.md)。

<a id="skill-create-skill"></a>

#### create-skill

**功能：**提供个人安装的 skill 结构、描述和渐进披露指导，保留旧工作流兼容入口。

**适用场景：**用户明确使用 create-skill，或维护依赖该指引的已有 skill。

**自动触发条件（全部满足）：**

- 目标是 Agent Skill 编写。
- 用户指定此入口或确需兼容既有写法。

**识别信号（可按语义匹配）：**`create-skill`、`旧技能编写流程`、`迁移 skill 编写规范`。

**不触发与边界：**

- 默认新建/更新优先 skill-creator。
- 不得把旧 .cursor/skills 路径和默认显式调用建议当成本项目现状。
- 游戏技能不适用。

**前置条件与实际可用性：**本项目按最新官方发现位置与用户自动选择要求执行；与系统 skill-creator 冲突时保持更高层要求。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：默认 skill-creator；只有明确旧入口兼容或用户指定时使用 create-skill，不叠加冲突默认值。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“按照 create-skill 检查这个旧 SKILL.md 的结构。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 create-skill 完成：按照 create-skill 检查这个旧 SKILL.md 的结构。”
- 不触发示例：“增加一个 GameplayAbility。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核实实际发现目录与元数据；避免同一职责导入两套相互冲突的写作规则。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/create-skill/SKILL.md)。

<a id="skill-create-subagent"></a>

#### create-subagent

**功能：**创建可复用的代码审查、调试等专用子代理配置和提示。

**适用场景：**用户要求保存一个新的 agent 类型，而非仅分派当前任务。

**自动触发条件（全部满足）：**

- 请求明确创建/修改专用子代理类型。
- 不是 UE AI 角色，也不是一次临时协作。

**识别信号（可按语义匹配）：**`创建代码审查 agent`、`保存一个专用子代理`、`.cursor/agents`、`agent 提示配置`。

**不触发与边界：**

- 仅要求一次多代理协作使用原生协作工具，不创建持久配置。
- 普通编码不得自动派生子代理。
- UE 敌人 AI 走 ue-ai-navigation。

**前置条件与实际可用性：**原文件含 disable-model-invocation=true；须尊重当前宿主显式加载策略，自动识别不等于绕过它。配置目录和格式需要按实际宿主核实。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。
- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“创建一个以后能复用的 UE 网络代码审查子代理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 create-subagent 完成：创建一个以后能复用的 UE 网络代码审查子代理。”
- 不触发示例：“给怪物写一个 AIController。” → 按真实对象、动作和排除条件转交或直接处理。
- 加载例外：原文件含 `disable-model-invocation=true`。自动识别意图不保证宿主允许隐式加载；如被限制，使用允许的显式入口，不暗改用户配置。

**验收：**核对配置解析、名称、范围和实际发现；不因编写配置就自动运行该代理。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/create-subagent/SKILL.md)。

<a id="skill-migrate-to-skills"></a>

#### migrate-to-skills

**功能：**将符合条件的智能 .mdc 规则和 slash 命令迁移成 Agent Skill，并原样保留正文。

**适用场景：**用户明确请求转换旧规则/命令，而不是重写它们。

**自动触发条件（全部满足）：**

- 有规则/命令到 SKILL.md 的明确迁移意图。
- 确认哪些文件符合迁移条件。

**识别信号（可按语义匹配）：**`迁移 .mdc`、`命令转 skill`、`.cursor/commands`、`旧规则转技能`。

**不触发与边界：**

- alwaysApply 或明确 globs 的长期规则不一律转为 skill。
- 新规则编写走 create-rule。
- 不要在迁移时顺手润色或删除正文。

**前置条件与实际可用性：**原文件含 disable-model-invocation=true，遵守宿主显式加载限制；先盘点目标与实际发现位置，正文逐字保留。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。

**调用方式与正反例：**

- 自动选择示例：“把旧的智能应用 .mdc 和 slash 命令迁成 skill。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 migrate-to-skills 完成：把旧的智能应用 .mdc 和 slash 命令迁成 skill。”
- 不触发示例：“保留所有常驻规则，只加一条中文注释要求。” → 按真实对象、动作和排除条件转交或直接处理。
- 加载例外：原文件含 `disable-model-invocation=true`。自动识别意图不保证宿主允许隐式加载；如被限制，使用允许的显式入口，不暗改用户配置。

**验收：**对比迁移前后正文和元数据，保留恢复点，确认新位置可发现。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/migrate-to-skills/SKILL.md)。


<a id="group-2"></a>

### 5.2 助手产品 配置 SDK 与 PR

<a id="skill-openai-docs"></a>

#### openai-docs

**功能：**核实 OpenAI API、ChatGPT/Codex 的模型、设置、skills、MCP、自动化和 SDK 产品信息。

**适用场景：**用户问当前产品能力、价格、配置方法、API 使用或本助手的具体功能。

**自动触发条件（全部满足）：**

- 问题对象确为 OpenAI/ChatGPT/Codex 产品或 API。
- 需要当前产品事实、官方接口或设置依据。

**识别信号（可按语义匹配）：**`你能否`、`这个应用`、`Codex 配置`、`模型价格`、`Responses API`、`MCP`、`自动化`、`skill 如何加载`。

**不触发与边界：**

- 仅提到 Codex，但实质是普通 UE/C++ 实现。
- 其他产品的 SDK 不能直接套用 OpenAI API。
- 创建提醒等执行请求应由相应原生工具完成，文档 skill 只辅助核查。

**前置条件与实际可用性：**优先官方文档搜索并打开正文，只使用指定官方域名；本地配置与当前可调用工具共同核实实际支持。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。
- [sdk](#skill-sdk)：仅任务还需要 `sdk` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [update-cli-config](#skill-update-cli-config)：仅任务还需要 `update-cli-config` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“这些 skill 会自动加载吗，项目应该放在哪里？” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 openai-docs 完成：这些 skill 会自动加载吗，项目应该放在哪里？”
- 不触发示例：“Codex 帮我修复这个 C++ 空指针。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**给支持对应结论的官方来源；不把当前账号、模型或工具权限猜成已开通。

**真实入口与来源：**[系统 SKILL.md](C:/Users/Kenwen/.codex/skills/.system/openai-docs/SKILL.md)。

<a id="skill-babysit"></a>

#### babysit

**功能：**循环处理 PR 未解决意见、明确的合并冲突与 CI 故障，直到达到可合并状态。

**适用场景：**用户要求持续盯住某个 PR 的评审和构建，并修复合理问题。

**自动触发条件（全部满足）：**

- 目标是明确的 PR 或可唯一确定的当前 PR。
- 用户要求持续维护，而非一次只读查看。

**识别信号（可按语义匹配）：**`盯住这个 PR`、`修 CI 直到通过`、`处理评审意见`、`保持 merge-ready`。

**不触发与边界：**

- 只问一条评论含义或一次性审查不必启动维护循环。
- 没有拆分需求时不使用 split-to-prs。
- 达到可合并不表示获得自动合并许可。

**前置条件与实际可用性：**核实 PR、当前分支、未解决评论和最新 CI；推送等行为依据本次明确授权及仓库规则。含糊冲突需澄清实际意图。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [split-to-prs](#skill-split-to-prs)：用户另有明确拆分需求时先形成拆分方案，遵守该技能的执行授权门槛。
- [ue-testing-debugging](#skill-ue-testing-debugging)：仅任务还需要 `ue-testing-debugging` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“持续处理这个 PR 的 CI 和评审，直到能合并。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 babysit 完成：持续处理这个 PR 的 CI 和评审，直到能合并。”
- 不触发示例：“只读告诉我这个 PR 当前 CI 状态。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对未解决评论、冲突及最新提交的 CI；定时持续跟进另用自动化工具，不能靠一次轮询宣称监控已建立。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/babysit/SKILL.md)。

<a id="skill-sdk"></a>

#### sdk

**功能：**处理本机 skill 所定义的 @cursor/sdk、Agent.create/prompt/resume 与 run 流式执行集成。

**适用场景：**外部 TypeScript 脚本、CI、服务或自动化代码调用该具体 SDK。

**自动触发条件（全部满足）：**

- 用户要在代码中运行 agent。
- 目标包/接口与该 skill 实际覆盖的一致。

**识别信号（可按语义匹配）：**`@cursor/sdk`、`Agent.create`、`Agent.resume`、`run.stream`、`CursorAgentError`、`CI 中运行代理`。

**不触发与边界：**

- OpenAI Agents SDK、@openai/codex-sdk 不自动等同 @cursor/sdk。
- 应用中临时创建聊天用原生工具。
- UE GAS 代理/AI 不属于此 SDK。

**前置条件与实际可用性：**读取实际 skill 和包版本；其中旧产品称谓、模型或鉴权名要按包/官方文档复核，凭据不写日志。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。
- [create-hook](#skill-create-hook)：仅需要真实 Agent 事件触发行为时使用，不把定时提醒或普通规范实现为 hook。

**调用方式与正反例：**

- 自动选择示例：“用 @cursor/sdk 写一个 CI 中运行 agent 的 TypeScript 脚本。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 sdk 完成：用 @cursor/sdk 写一个 CI 中运行 agent 的 TypeScript 脚本。”
- 不触发示例：“使用 Python OpenAI Agents SDK 调用 Responses。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对真实包导出、runtime capability、流、取消、错误和认证；不把其他 SDK 示例移植后称兼容。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/sdk/SKILL.md)。

<a id="skill-shell"></a>

#### shell

**功能：**把 /shell 后的文本作为用户给定的字面命令执行，报告退出状态。

**适用场景：**用户明确以 /shell 请求直接执行命令。

**自动触发条件（全部满足）：**

- 消息明确调用 /shell。
- 其后存在要执行的命令。

**识别信号（可按语义匹配）：**`/shell`。

**不触发与边界：**

- 普通请求如运行构建不触发该 skill，只使用常规终端工具。
- 文档、引用或代码块里出现 /shell 不等于当前执行指令。
- 危险命令仍受实际权限和用户授权限制。

**前置条件与实际可用性：**原文件为显式调用；不能因用户希望自动选择所有 skill 而推断 /shell。空命令时需要询问具体命令。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- 本条可独立执行；不默认叠加其他 skill。只有后续子任务产生明确的新交付或职责时按总规则重新选择。

**调用方式与正反例：**

- 自动选择示例：“/shell git status --short” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 shell 完成：/shell git status --short”
- 不触发示例：“解释一下 /shell git status --short 的作用，不要执行。” → 按真实对象、动作和排除条件转交或直接处理。
- 加载例外：原文件含 `disable-model-invocation=true`。自动识别意图不保证宿主允许隐式加载；如被限制，使用允许的显式入口，不暗改用户配置。

**验收：**执行真正的字面命令并报告退出码；不自动润色命令、追加前后操作或扩大授权。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/shell/SKILL.md)。

<a id="skill-split-to-prs"></a>

#### split-to-prs

**功能：**按职责、审阅者和真实依赖将现有改动规划并拆成独立或堆叠 PR。

**适用场景：**用户要求拆分分支、改动集合或大 PR。

**自动触发条件（全部满足）：**

- 已有明确改动或 PR 可拆分。
- 用户希望拆成多个可审查单元。

**识别信号（可按语义匹配）：**`拆 PR`、`分成几个小 PR`、`拆分这批改动`、`stacked PR`。

**不触发与边界：**

- 单纯整理目录不等于拆 PR。
- 当前项目默认不自动提交。
- 该 skill 明确要求拆分方案获批后才能建分支、提交、推送和开 PR。

**前置条件与实际可用性：**先检查工作区、基线、所有权和依赖，形成可审查的拆分方案；授权已有时不重复询问，执行前保留可恢复快照。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [babysit](#skill-babysit)：拆分后只有明确要求持续维护对应 PR 时启动维护循环；不自动合并。
- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。

**调用方式与正反例：**

- 自动选择示例：“把这些混合改动拆成几个独立 PR，先给方案。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 split-to-prs 完成：把这些混合改动拆成几个独立 PR，先给方案。”
- 不触发示例：“只帮我阅读这个 diff。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对各 PR 范围与依赖，创建后附加 PR；保留原工作和恢复引用。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/split-to-prs/SKILL.md)。

<a id="skill-statusline"></a>

#### statusline

**功能：**配置 CLI 提示区状态行及其 stdin/stdout 脚本，展示会话、模型或上下文信息。

**适用场景：**用户明确想修改终端 agent 的状态行显示。

**自动触发条件（全部满足）：**

- 目标是 CLI 会话状态行。
- 存在展示内容或格式修改意图。

**识别信号（可按语义匹配）：**`statusline`、`statusLine`、`CLI 状态栏`、`提示符上方信息`、`上下文百分比`。

**不触发与边界：**

- 游戏 HUD 状态栏走 UE UI。
- 编辑器字体/颜色走 update-cursor-settings。
- 桌面产品是否支持此旧配置需先核实。

**前置条件与实际可用性：**读取实际 CLI 版本与配置；技能中的 ~/.cursor/cli-config.json 不能自动套用当前 .codex/config.toml。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [update-cli-config](#skill-update-cli-config)：仅任务还需要 `update-cli-config` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。

**调用方式与正反例：**

- 自动选择示例：“在 CLI 状态行显示当前模型与上下文用量。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 statusline 完成：在 CLI 状态行显示当前模型与上下文用量。”
- 不触发示例：“在游戏 HUD 显示角色生命值。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**校验配置和模拟输入，验证真实 CLI 渲染及空/null 字段处理。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/statusline/SKILL.md)。

<a id="skill-update-cli-config"></a>

#### update-cli-config

**功能：**按安装 skill 的实际范围读取并最小修改 ~/.cursor/cli-config.json 及相关 CLI 配置。

**适用场景：**用户要改该 CLI 的 Vim、展示、权限偏好或其他受支持设置。

**自动触发条件（全部满足）：**

- 目标是 CLI 配置而非 UE 配置。
- 确认实际产品、文件和设置键。

**识别信号（可按语义匹配）：**`CLI 设置`、`vimMode`、`approvalMode`、`cli-config.json`、`终端 agent 偏好`。

**不触发与边界：**

- 不能把旧 Cursor CLI 配置直接写入 Codex config.toml。
- UE Build.cs/INI 不属于此 skill。
- 设置请求不能绕过本次会话固定的沙箱/审批策略。

**前置条件与实际可用性：**先核实当前产品和官方文档，读取旧值，保留无关字段；鉴权、模型缓存等内部字段不手改。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。
- [statusline](#skill-statusline)：仅任务还需要 `statusline` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“将这个 CLI 的 Vim 输入模式打开，先确认配置位置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 update-cli-config 完成：将这个 CLI 的 Vim 输入模式打开，先确认配置位置。”
- 不触发示例：“修改 DefaultEngine.ini 的默认地图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**解析配置并核对支持范围；重启要求来自实际宿主，不凭保存成功称已生效。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/update-cli-config/SKILL.md)。

<a id="skill-update-cursor-settings"></a>

#### update-cursor-settings

**功能：**修改 Codex/VSCode 类 IDE 的编辑器、外观、文件与终端 settings.json 配置。

**适用场景：**用户要改 IDE 字号、缩进、主题、自动保存或保存格式化。

**自动触发条件（全部满足）：**

- 目标是 IDE 设置。
- 确认用户级或工作区级文件。

**识别信号（可按语义匹配）：**`字号`、`editor.fontSize`、`tabSize`、`formatOnSave`、`settings.json`、`IDE 主题`。

**不触发与边界：**

- UE 编辑器设置不是 VSCode settings.json。
- CLI 配置走对应 CLI skill。
- JSONC 注释和无关用户配置必须保留。

**前置条件与实际可用性：**核实真实应用和设置文件，先读后改；与当前 Codex 桌面设置不兼容时采用官方支持入口。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [openai-docs](#skill-openai-docs)：仅需要核实 OpenAI/Codex 当前产品、宿主配置或官方接口时读取，不把普通业务代码问题转为产品研究。
- [create-rule](#skill-create-rule)：仅需要持续生效的项目约定/规则入口时使用，保持工作流与长期规则职责分开。

**调用方式与正反例：**

- 自动选择示例：“把 IDE 字号设为 16，并启用保存时格式化。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 update-cursor-settings 完成：把 IDE 字号设为 16，并启用保存时格式化。”
- 不触发示例：“调整 UE 的视口相机速度。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**验证 JSON/JSONC 与实际键，报告需要的重载步骤。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/update-cursor-settings/SKILL.md)。

<a id="skill-plugin-management-plugin-management"></a>

#### plugin-management:plugin-management

**功能：**查找集成、建议安装、核实连接权限/依赖，或执行用户明确的移除/权限更改。

**适用场景：**需要当前工具无法访问的外部服务或明确管理 Codex 插件。

**自动触发条件（全部满足）：**

- 外部服务访问或插件管理会直接帮助任务。
- 现有原生工具/已连接插件不足或用户指定该服务。

**识别信号（可按语义匹配）：**`连接 Notion`、`找 Gmail 插件`、`插件权限`、`卸载插件`、`外部账号`。

**不触发与边界：**

- 通用联网搜索、内置图像生成不需另装插件。
- UE .uplugin 不是此处的 Codex 应用插件。
- 不得自行扩权、卸载或声称建议等于连接完成。

**前置条件与实际可用性：**优先现有工具，再查插件目录；建议不阻止独立工作。权限改变和移除必须对应明确用户意图。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-research-documentation](#skill-notion-notion-research-documentation)：仅任务还需要 `notion:notion-research-documentation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [documents:documents](#skill-documents-documents)：仅任务还需要 `documents:documents` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [sites:sites](#skill-sites-sites)：只有真正网站或站点托管成为交付目标时交接；图片、分析或对话预览不自动发布。

**调用方式与正反例：**

- 自动选择示例：“找一个能连接 Google Drive 的插件并检查所需权限。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 plugin-management:plugin-management 完成：找一个能连接 Google Drive 的插件并检查所需权限。”
- 不触发示例：“给 UE 创建一个 Runtime 插件模块。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核实实际安装/连接/权限返回值，不把推荐列表或可搜索名称当作已授权连接。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/plugin-management/0.1.0/skills/plugin-management/SKILL.md)。


<a id="group-3"></a>

### 5.3 图像 视觉 界面与网站

<a id="skill-imagegen"></a>

#### imagegen

**功能：**生成照片、插画、纹理、精灵、透明底切图，或按参考修改已有位图。

**适用场景：**用户需要得到新的 PNG/JPEG 等图像资产，或改动现有图片的视觉内容。

**自动触发条件（全部满足）：**

- 交付物是位图或需要 AI 图像生成。
- 存在生成、编辑、风格变体、去背景等实际意图。

**识别信号（可按语义匹配）：**`画一张`、`生成图标`、`透明背景`、`修改照片`、`贴图`、`PNG`、`sprite`。

**不触发与边界：**

- 现有 SVG、图标系统、HTML/CSS/canvas 可以直接完成的代码型视觉。
- 仅解释图片内容，不需要生成或编辑。
- 动画宠物完整生命周期由 work-pets 主导。

**前置条件与实际可用性：**优先内置 image_gen；读取本地输入图片后再编辑。CLI 需要用户明确选择及 API 凭据，不能静默切换。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [work-pets:create-pet](#skill-work-pets-create-pet)：只有明确 ChatGPT Pets 产品上下文且需要完整宠物生命周期时由宠物技能主导，imagegen 只处理位图。
- [work-pets:update-pet](#skill-work-pets-update-pet)：只有明确 ChatGPT Pets 产品上下文且需要完整宠物生命周期时由宠物技能主导，imagegen 只处理位图。
- [sites:sites](#skill-sites-sites)：只有真正网站或站点托管成为交付目标时交接；图片、分析或对话预览不自动发布。

**调用方式与正反例：**

- 自动选择示例：“为这个 GAS 技能画一张透明底的图标。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 imagegen 完成：为这个 GAS 技能画一张透明底的图标。”
- 不触发示例：“把已有 SVG 的描边改成 2px。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**检查生成图像、透明度、参考一致性；项目使用的图像保存到项目资产位置。

**真实入口与来源：**[系统 SKILL.md](C:/Users/Kenwen/.codex/skills/.system/imagegen/SKILL.md)。

<a id="skill-canvas"></a>

#### canvas

**功能：**将独立的数据分析、架构审查、账单调查、时间线或可比较结果呈现为旁栏 React Canvas。

**适用场景：**数据或分析本身是交付物，需要持续查看、比较、筛选或丰富布局。

**自动触发条件（全部满足）：**

- 交付物是独立分析/审查结果或明确要求 Canvas。
- 不是另一个指定工具或现有文档中的任务。

**识别信号（可按语义匹配）：**`分析结果面板`、`独立分析表`、`架构审查`、`指标对比`、`时间线`、`.canvas.tsx`。

**不触发与边界：**

- 现有代码修复、写使用文档、指定 Word/Excel/Notion/Sites 产出不额外建立 Canvas。
- 简单事实或 Mermaid 足够的结构图。
- 对话内动态解释通常优先 visualize。

**前置条件与实际可用性：**先核实当前宿主的 Canvas SDK、托管目录和运行支持；本机技能内的旧目录不能直接当成可用路径。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [ue-profiling-and-optimization](#skill-ue-profiling-and-optimization)：仅任务还需要 `ue-profiling-and-optimization` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [notion:notion-research-documentation](#skill-notion-notion-research-documentation)：仅任务还需要 `notion:notion-research-documentation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“把这次性能分析做成可以旁边查看的筛选面板。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 canvas 完成：把这次性能分析做成可以旁边查看的筛选面板。”
- 不触发示例：“在仓库写一份完整的 skill 使用手册。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**检查真实 SDK 导出、页面编译与交互；不把缺失渲染支持的源码称为可运行 Canvas。

**真实入口与来源：**[个人 SKILL.md](C:/Users/Kenwen/.agents/skills/canvas/SKILL.md)。

<a id="skill-computer-use-computer-use"></a>

#### computer-use:computer-use

**功能：**通过受支持的计算机控制运行时查看和操作 Windows 应用 UI。

**适用场景：**任务必须依赖真实应用窗口、控件和屏幕状态，且没有更直接的连接器/CLI。

**自动触发条件（全部满足）：**

- 用户需要操作实际界面。
- 所需原生/浏览器表面在当前运行时确实可用。

**识别信号（可按语义匹配）：**`点击窗口`、`打开应用界面`、`在编辑器里操作`、`检查已打开窗口`、`Excel 插件设置`。

**不触发与边界：**

- 有合适的文件工具、连接器或 CLI 时优先使用。
- 不能通过坐标猜屏幕。
- 本次 CUA 明示原生 API 禁用，不能凭 skill 名称宣称可控制原生应用，也不能换隐蔽方式绕过限制。

**前置条件与实际可用性：**完整读取该 skill、guidance 和 confirmations，检查实际运行时支持。Excel 单元格读写走 connected-document，Computer Use 只做其设置/焦点。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [spreadsheets:excel-live-control](#skill-spreadsheets-excel-live-control)：仅明确活动 Excel 目标时使用；连接不足时不默认改用另一表面。
- [unreal-mcp](#skill-unreal-mcp)：仅任务还需要 `unreal-mcp` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“在当前支持的浏览器里打开页面并查看设置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 computer-use:computer-use 完成：在当前支持的浏览器里打开页面并查看设置。”
- 不触发示例：“直接用文件工具修改一个 Markdown 文件。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**读取真实 UI 状态并核对结果；不可用时说明具体表面限制，继续不依赖 UI 的工作。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-bundled/computer-use/26.1007.21434/skills/computer-use/SKILL.md)。

<a id="skill-visualize-visualize"></a>

#### visualize:visualize

**功能：**在对话内用可调参数、模拟、动态图等解释机制或比较情境。

**适用场景：**用户要当场看懂变化关系或交互原型，并不需要独立网站/工程组件。

**自动触发条件（全部满足）：**

- 可视交互显著帮助当前解释。
- 交付面是对话内展示而非新网站或仓库组件。

**识别信号（可按语义匹配）：**`演示变化`、`拖动参数看看`、`交互说明`、`模拟一下`、`解释这个过程`、`对话里预览`。

**不触发与边界：**

- 静态节点与连线能解释时用 Mermaid。
- 论文/可导出的科学图使用标准绘图工具。
- 指定文件或工具产出不强行改为内嵌可视化。

**前置条件与实际可用性：**完整读取该 skill；宿主必须支持相应可视化引用与可写位置。与更高层沟通或格式要求冲突时遵守更高层。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [canvas](#skill-canvas)：通常择一：独立分析交付用 Canvas，对话内动态解释用 visualize；简单结构用 Mermaid。
- [sites:sites](#skill-sites-sites)：只有真正网站或站点托管成为交付目标时交接；图片、分析或对话预览不自动发布。

**调用方式与正反例：**

- 自动选择示例：“用一个滑块演示网络延迟怎样影响技能预测。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 visualize:visualize 完成：用一个滑块演示网络延迟怎样影响技能预测。”
- 不触发示例：“把这张正式论文图导出成 SVG/PDF。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**检查交互、数据与主题适配；不能把对话展示声称已经部署。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-bundled/visualize/1.0.49/skills/visualize/SKILL.md)。

<a id="skill-sites-sites"></a>

#### sites:sites

**功能：**构建网站、网页工具、门户、仪表盘、游戏或管理既有 Sites 托管项目。

**适用场景：**用户需要一个真正的网站或修改已识别的站点。

**自动触发条件（全部满足）：**

- 交付面明确是网站/站点或网站项目变更。
- 不是仅在对话中解释/预览一个交互效果。

**识别信号（可按语义匹配）：**`网站`、`落地页`、`portfolio`、`门户`、`网页工具`、`发布站点`、`Sites`。

**不触发与边界：**

- UE UMG/HUD 不属于网站。
- 对话内动态示意走 visualize，独立分析可走 canvas。
- 只写文档不擅自发布网站。

**前置条件与实际可用性：**按 skill 选择托管或 local-only，核实当前项目、访问范围、工具与原生审批；用户要求本地或只保存时不发布。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [imagegen](#skill-imagegen)：交付确需新位图或位图编辑时调用，既有 SVG/代码视觉不改走图像生成。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management)：仅缺少目标外部服务的连接/能力时发现并建议插件；已有原生工具足够时不额外安装。
- [template-creator:template-creator](#skill-template-creator-template-creator)：仅用户明确要把参考样式保存为今后复用的个人模板 skill 时使用；一次套模板不触发。

**调用方式与正反例：**

- 自动选择示例：“给我做一个展示 Hodgepodge 的网站，只保留本地源码。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 sites:sites 完成：给我做一个展示 Hodgepodge 的网站，只保留本地源码。”
- 不触发示例：“在 UE 中制作角色装备界面。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**运行适当构建检查、核实站点版本/部署终态；local-only 不注册或部署。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/sites/1.0.1/skills/sites/SKILL.md)。


<a id="group-4"></a>

### 5.4 文档 表格 幻灯片与模板

<a id="skill-documents-documents"></a>

#### documents:documents

**功能：**读取、创建、编辑、批注或修订 DOCX，并通过渲染逐页验证版式；支持规定的 Google Docs 导入路线。

**适用场景：**交付是 Word/DOCX，或需要保留文档结构、样式、修订和排版。

**自动触发条件（全部满足）：**

- 交付/输入是 DOCX、Word 或 Google Docs 目标。
- 需要文档文件操作而非普通仓库文字编辑。

**识别信号（可按语义匹配）：**`Word`、`.docx`、`修订模式`、`批注`、`Google Docs`、`合同排版`。

**不触发与边界：**

- 仓库 Markdown 使用普通文件工具。
- PDF 交付走 pdf。
- 仅保存 Notion 页面用 Notion 工具。
- 现有模板的一次性沿用不创建模板 skill。

**前置条件与实际可用性：**使用 workspace dependency loader 的运行时；按 skill 执行开始标记与 render_docx.py。Google Docs 新建默认先验证 DOCX 再通过已连接插件导入。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [pdf:pdf](#skill-pdf-pdf)：仅任务还需要 `pdf:pdf` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [template-creator:template-creator](#skill-template-creator-template-creator)：仅用户明确要把参考样式保存为今后复用的个人模板 skill 时使用；一次套模板不触发。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management)：仅缺少目标外部服务的连接/能力时发现并建议插件；已有原生工具足够时不额外安装。

**调用方式与正反例：**

- 自动选择示例：“把这份报告做成带页眉页脚的 Word 文档。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 documents:documents 完成：把这份报告做成带页眉页脚的 Word 文档。”
- 不触发示例：“在 Docs/Guides 中新增 Markdown 使用手册。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**实际渲染并查看页面图片；Google Docs 原生导入另核实连接和返回的文档。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/documents/26.1013.11914/skills/documents/SKILL.md)。

<a id="skill-pdf-pdf"></a>

#### pdf:pdf

**功能：**提取、创建、修改、渲染和验证 PDF，包括 AcroForm 字段与扁平化结果。

**适用场景：**交付/输入是 PDF，且结构、字段或版面需要真实核查。

**自动触发条件（全部满足）：**

- 任务涉及实际 PDF 文件。
- 需阅读、生成、修改、填写或验证。

**识别信号（可按语义匹配）：**`.pdf`、`填表 PDF`、`AcroForm`、`PDF 排版`、`合并 PDF`。

**不触发与边界：**

- Word 源稿优先 documents，PDF 导出才叠加 pdf。
- 只给网页概念摘要且不需要 PDF 时不必生成文件。
- 视觉显示正确不代表表单逻辑字段正确。

**前置条件与实际可用性：**加载工作区依赖，使用规定的创建/编辑标记；填写后重新打开检查 get_fields、Widget、值与 appearance。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [documents:documents](#skill-documents-documents)：仅任务还需要 `documents:documents` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [presentations:Presentations](#skill-presentations-presentations)：仅任务还需要 `presentations:Presentations` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“填写这个 PDF 的姓名和日期，保留表单可编辑。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 pdf:pdf 完成：填写这个 PDF 的姓名和日期，保留表单可编辑。”
- 不触发示例：“写一段 Markdown 介绍 PDF 格式。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**渲染页与字段逻辑都要检查；只有用户要求时才扁平化。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/pdf/26.1013.11914/skills/pdf/SKILL.md)。

<a id="skill-presentations-presentations"></a>

#### presentations:Presentations

**功能：**读取、创建或编辑 PowerPoint/PPTX/Google Slides，处理结构、版式、模板和渲染验证。

**适用场景：**交付或修改对象是幻灯片，而不是网站或一般文字提纲。

**自动触发条件（全部满足）：**

- 用户目标为演示文稿文件或 Google Slides。
- 需要阅读、解释、编辑、生成或排版。

**识别信号（可按语义匹配）：**`PPT`、`PPTX`、`PowerPoint`、`Google Slides`、`幻灯片`、`演示稿`。

**不触发与边界：**

- 仅要文字演讲提纲可直接写，不擅自制作幻灯片。
- 网站视觉页面走 Sites。
- 仅沿用模板一次，不创建可复用模板 skill。

**前置条件与实际可用性：**使用工作区依赖，按任务读模板/设计/Google Slides 路由；只读提问不得修改文件。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [imagegen](#skill-imagegen)：交付确需新位图或位图编辑时调用，既有 SVG/代码视觉不改走图像生成。
- [pdf:pdf](#skill-pdf-pdf)：仅任务还需要 `pdf:pdf` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [template-creator:template-creator](#skill-template-creator-template-creator)：仅用户明确要把参考样式保存为今后复用的个人模板 skill 时使用；一次套模板不触发。

**调用方式与正反例：**

- 自动选择示例：“把这个 UE 架构方案制作成 10 页 PPT。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 presentations:Presentations 完成：把这个 UE 架构方案制作成 10 页 PPT。”
- 不触发示例：“只给我一段演讲开场白。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按 skill 渲染并检查页面、溢出、字体与素材；Google Slides 输出验证原生结果。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/presentations/26.1013.11914/skills/presentations/SKILL.md)。

<a id="skill-template-creator-template-creator"></a>

#### template-creator:template-creator

**功能：**从参考文档、幻灯片、表格、图片、邮件或站点提炼可复用的个人模板 skill。

**适用场景：**用户明确希望将参考样式保存为今后可重复使用的模板能力。

**自动触发条件（全部满足）：**

- 存在参考产物/样式或明确模板更新目标。
- 用户希望持久复用模板，不是本次套用一次。

**识别信号（可按语义匹配）：**`保存成我的模板`、`以后按这个样式`、`模板 skill`、`可复用报告模板`。

**不触发与边界：**

- 用这份模板生成一份文档只走对应产物 skill。
- C++ 模板、Blueprint 模板不是产物模板。
- 不要把普通图像请求变成模板创建。

**前置条件与实际可用性：**按来源读取参考，遵守个人 skill 安装/保留约定，必要时配合产物 skill 验证代表性输出。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [skill-creator](#skill-skill-creator)：仅需要创建/更新可复用 Agent Skill 的说明、触发或元数据时使用。
- [documents:documents](#skill-documents-documents)：仅任务还需要 `documents:documents` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [presentations:Presentations](#skill-presentations-presentations)：仅任务还需要 `presentations:Presentations` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [spreadsheets:Spreadsheets](#skill-spreadsheets-spreadsheets)：仅任务还需要 `spreadsheets:Spreadsheets` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“把这份周报的版式保存成以后能反复用的模板 skill。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 template-creator:template-creator 完成：把这份周报的版式保存成以后能反复用的模板 skill。”
- 不触发示例：“这次按现有 Word 模板填入新内容。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核实模板保留内容、可变字段、触发描述、安装与可复用性。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/template-creator/26.1013.11914/skills/template-creator/SKILL.md)。

<a id="skill-spreadsheets-spreadsheets"></a>

#### spreadsheets:Spreadsheets

**功能：**读取、生成、编辑或分析 XLSX/XLS/CSV/TSV、Google Sheets，处理公式、格式、图表和重算。

**适用场景：**要交付或修改一个表格文件，或针对明确的 Google Sheets 任务。

**自动触发条件（全部满足）：**

- 对象是表格文件/Google Sheets。
- 没有明确转到已连接的 Excel Desktop 活动会话。

**识别信号（可按语义匹配）：**`.xlsx`、`CSV`、`做一个工作簿`、`公式`、`Google Sheets`、`预算表`、`统计表`。

**不触发与边界：**

- 明确操作 Excel 当前打开会话时走 excel-live-control。
- 普通 Markdown 对照表不自动创建 XLSX。
- 生成分析结果与保存表格是不同交付面。

**前置条件与实际可用性：**使用工作区 dependency loader 与该 skill 规定的 artifact 工具；核实公式/引用/重算支持，不伪造缓存值。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [spreadsheets:excel-live-control](#skill-spreadsheets-excel-live-control)：执行面互斥：文件/Sheets 与活动 Excel 会话不得同阶段混用；只有用户明确切换交付面时交接。
- [canvas](#skill-canvas)：仅独立分析结果是交付物时加 Canvas；用户指定文件、Notion 或网站产出时不额外创建。
- [template-creator:template-creator](#skill-template-creator-template-creator)：仅用户明确要把参考样式保存为今后复用的个人模板 skill 时使用；一次套模板不触发。

**调用方式与正反例：**

- 自动选择示例：“把这份 CSV 汇总成含公式与图表的 Excel 文件。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 spreadsheets:Spreadsheets 完成：把这份 CSV 汇总成含公式与图表的 Excel 文件。”
- 不触发示例：“修改我当前 Excel 窗口选中的单元格。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**校验公式重算、错误、引用和必要的渲染；避免只生成文件却未核实计算。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/spreadsheets/26.1013.11914/skills/spreadsheets/SKILL.md)。

<a id="skill-spreadsheets-excel-live-control"></a>

#### spreadsheets:excel-live-control

**功能：**通过 ChatGPT Excel 插件连接的活动工作簿读取、写入、分析与格式化。

**适用场景：**用户明确指向 Excel Desktop、打开工作簿、当前选区或延续实时会话。

**自动触发条件（全部满足）：**

- 明确 Excel Desktop/已连接工作簿对象或已建立的实时上下文。
- 完成窗口、插件、登录、工具与文档会话验证。

**识别信号（可按语义匹配）：**`当前 Excel`、`打开的工作簿`、`选中区域`、`ChatGPT add-in`、`Microsoft Excel @提及`。

**不触发与边界：**

- 普通新建 Excel 文件走文件型 Spreadsheets。
- 实时连接失败不能静默改为离线文件或另一个工作簿。
- 不能用计算机点击代替单元格读写连接器。

**前置条件与实际可用性：**按 skill 的设置状态机验证目标和登录，再调用 list_document_sessions；原生 UI 表面禁用可能阻塞初次设置，需说明具体缺失项。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [computer-use:computer-use](#skill-computer-use-computer-use)：只做确需 UI 的目标/设置确认，必须核实当前表面支持；Excel 单元格仍由文档连接器操作。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management)：仅缺少目标外部服务的连接/能力时发现并建议插件；已有原生工具足够时不额外安装。

**调用方式与正反例：**

- 自动选择示例：“把我当前 Excel 工作簿选中的数据生成图表。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 spreadsheets:excel-live-control 完成：把我当前 Excel 工作簿选中的数据生成图表。”
- 不触发示例：“帮我生成一个新的 .xlsx 文件，供下载。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**对正确会话执行受支持操作并读回结果，生命周期变化后重新核实目标。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-primary-runtime/spreadsheets/26.1013.11914/skills/excel-live-control/SKILL.md)。


<a id="group-5"></a>

### 5.5 Notion 工作流

<a id="skill-notion-notion-database-designer"></a>

#### notion:notion-database-designer

**功能：**把散乱记录组织为符合实体和工作流程的 Notion 数据源、属性、关系及视图。

**适用场景：**需要新建/改进项目、内容、请求等长期跟踪数据库。

**自动触发条件（全部满足）：**

- 目标是 Notion tracker 的结构设计。
- 涉及实体、字段、视图或关系的设计。

**识别信号（可按语义匹配）：**`Notion 任务库`、`设计数据库`、`字段和视图`、`请求跟踪器`。

**不触发与边界：**

- 普通记录状态更新不需要重新设计数据库。
- UE DataTable 或 SQL 数据库不属于 Notion。
- 只要设计时不自动写入数据库。

**前置条件与实际可用性：**核实已连接的 Notion 工具、现有 schema 和权限；明确建库请求可在范围内执行，设计提问不等于建库请求。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-spec-to-implementation](#skill-notion-notion-spec-to-implementation)：仅任务还需要 `notion:notion-spec-to-implementation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management)：仅缺少目标外部服务的连接/能力时发现并建议插件；已有原生工具足够时不额外安装。

**调用方式与正反例：**

- 自动选择示例：“在 Notion 设计一个团队需求跟踪库，给出字段和视图。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 notion:notion-database-designer 完成：在 Notion 设计一个团队需求跟踪库，给出字段和视图。”
- 不触发示例：“把 Notion 现有任务状态改为完成。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**读取创建后的属性、视图和关系，检查重复创建与只读/同步属性限制。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/notion/3.0.1/skills/notion-database-designer/SKILL.md)。

<a id="skill-notion-notion-meeting-intelligence"></a>

#### notion:notion-meeting-intelligence

**功能：**准备议程、预读、决策材料，沿用周期会议上下文并整理会后行动。

**适用场景：**会议材料需要结合已知项目、参与者、决策与待办。

**自动触发条件（全部满足）：**

- 任务是会议准备或把会议结果转为下一步。
- 需要上下文组织而非单次查找/原样保存。

**识别信号（可按语义匹配）：**`会议议程`、`预读材料`、`会前准备`、`行动项`、`续上次例会`。

**不触发与边界：**

- 只查会议时间或原样存笔记不必启动完整流程。
- 没有用户请求不发送通知或创建无关跟进任务。
- 聊天草稿不自动保存到 Notion。

**前置条件与实际可用性：**使用必要的 Notion 证据，核实受众与权限；外部受众不得混入内部敏感材料。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-research-documentation](#skill-notion-notion-research-documentation)：仅任务还需要 `notion:notion-research-documentation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [notion:notion-spec-to-implementation](#skill-notion-notion-spec-to-implementation)：仅任务还需要 `notion:notion-spec-to-implementation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“根据上次 Notion 会议记录准备明天的评审议程和预读。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 notion:notion-meeting-intelligence 完成：根据上次 Notion 会议记录准备明天的评审议程和预读。”
- 不触发示例：“原样保存我粘贴的会议笔记。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对会议事实、证据、负责人和日期；没有证据的不编造。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/notion/3.0.1/skills/notion-meeting-intelligence/SKILL.md)。

<a id="skill-notion-notion-research-documentation"></a>

#### notion:notion-research-documentation

**功能：**从多份 Notion 证据综合回答、比较、研究报告并解释冲突。

**适用场景：**问题需要连接不同页面、时间或证据，而非总结单页。

**自动触发条件（全部满足）：**

- Notion 是需要检索的证据源。
- 需要跨来源综合、比较或冲突解释。

**识别信号（可按语义匹配）：**`研究这些 Notion 页面`、`综合决策依据`、`跨文档比较`、`证据冲突`。

**不触发与边界：**

- 已知单页查找或单页摘要用直接工具。
- 仅提供聊天分析时不自动创建页面。
- 结论不能只根据最近编辑时间判断真伪。

**前置条件与实际可用性：**核实来源、适用时间和访问范围，必要时做外部研究并区分推断。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-meeting-intelligence](#skill-notion-notion-meeting-intelligence)：仅任务还需要 `notion:notion-meeting-intelligence` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [canvas](#skill-canvas)：仅独立分析结果是交付物时加 Canvas；用户指定文件、Notion 或网站产出时不额外创建。
- [plugin-management:plugin-management](#skill-plugin-management-plugin-management)：仅缺少目标外部服务的连接/能力时发现并建议插件；已有原生工具足够时不额外安装。

**调用方式与正反例：**

- 自动选择示例：“比较 Notion 中三套方案，给出处与尚未解决的分歧。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 notion:notion-research-documentation 完成：比较 Notion 中三套方案，给出处与尚未解决的分歧。”
- 不触发示例：“总结这一页我已提供的文字。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**引用支撑结论的源页面，明确事实、解释和缺口；保存结果只在请求范围内。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/notion/3.0.1/skills/notion-research-documentation/SKILL.md)。

<a id="skill-notion-notion-spec-to-implementation"></a>

#### notion:notion-spec-to-implementation

**功能：**将规格转为可跟踪实施计划、关联任务和证据化进展。

**适用场景：**需要按需求落地、创建实施任务或处理规格变化的影响。

**自动触发条件（全部满足）：**

- 有明确 Notion 产品/技术规格或需求来源。
- 需要实现计划、任务拆解或实现对照。

**识别信号（可按语义匹配）：**`按 Notion 规格实现`、`需求拆解`、`生成任务`、`规格变更影响`、`实施进展`。

**不触发与边界：**

- 只需周度概况时优先 weekly-project-digest。
- 纯计划不自动写任务或改状态。
- 不得把现有状态当作已完成验证。

**前置条件与实际可用性：**读当前规格和实际实现，保留范围、依赖、验收和未决项；写页面/任务须符合请求范围。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-database-designer](#skill-notion-notion-database-designer)：仅任务还需要 `notion:notion-database-designer` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [ue-gameplay-architecture-planning](#skill-ue-gameplay-architecture-planning)：仅任务还需要 `ue-gameplay-architecture-planning` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [ue-testing-debugging](#skill-ue-testing-debugging)：仅任务还需要 `ue-testing-debugging` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“把这份 Notion 技术规格转成实施任务并标注依赖。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 notion:notion-spec-to-implementation 完成：把这份 Notion 技术规格转成实施任务并标注依赖。”
- 不触发示例：“只告诉我这个任务的负责人。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**需求、任务、代码与测试证据可追溯；只更新确实发生的进展。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/notion/3.0.1/skills/notion-spec-to-implementation/SKILL.md)。

<a id="skill-notion-notion-weekly-project-digest"></a>

#### notion:notion-weekly-project-digest

**功能：**对比报告期内任务、决策、阻塞与上期基线，生成有证据的团队周报。

**适用场景：**用户要求周报、阶段汇总或连续报告期比较。

**自动触发条件（全部满足）：**

- 目标为项目/团队周期摘要。
- 有明确报告期或可从上下文确定。

**识别信号（可按语义匹配）：**`Notion 周报`、`本周进展`、`和上周比较`、`项目 digest`。

**不触发与边界：**

- 单个需求实现追踪走 spec-to-implementation。
- 当前 Done 不证明本周完成。
- 要求每周自动执行还需原生自动化工具，不能只写一次周报。

**前置条件与实际可用性：**定义时区和报告期，读取相关状态历史/证据；保留受众权限与不确定性。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [notion:notion-spec-to-implementation](#skill-notion-notion-spec-to-implementation)：仅任务还需要 `notion:notion-spec-to-implementation` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。
- [notion:notion-meeting-intelligence](#skill-notion-notion-meeting-intelligence)：仅任务还需要 `notion:notion-meeting-intelligence` 条目说明的独立职责时加入；先读其适用/排除条件，并保持请求的目标与写入范围。

**调用方式与正反例：**

- 自动选择示例：“依据 Notion 记录生成本周项目周报，并和上周对比。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 notion:notion-weekly-project-digest 完成：依据 Notion 记录生成本周项目周报，并和上周对比。”
- 不触发示例：“把这一个需求拆成实现任务。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**所有进展和数字有来源，不把编辑次数、状态快照或任务比例当作实际交付。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/notion/3.0.1/skills/notion-weekly-project-digest/SKILL.md)。


<a id="group-6"></a>

### 5.6 ChatGPT 动画宠物

<a id="skill-work-pets-create-pet"></a>

#### work-pets:create-pet

**功能：**从角色想法或参考图创建 ChatGPT Work v2 动画宠物，生成状态/方向图集并验证上传。

**适用场景：**目标明确是 ChatGPT Pets 的新动画伙伴或未绑定的新图集。

**自动触发条件（全部满足）：**

- 明确 ChatGPT/Work Pets 产品，或已有该产品上下文。
- 用户要求创建新宠物/新图集或相关创建流程。

**识别信号（可按语义匹配）：**`ChatGPT 动画宠物`、`Work pet`、`新建伙伴`、`v2 sprite sheet`。

**不触发与边界：**

- 真实宠物、UE 游戏宠物和其他应用宠物不适用。
- 仅静态头像走 imagegen。
- 更新已有自定义宠物走 update-pet，不新建替代其稳定 ID。

**前置条件与实际可用性：**先核实 Pets 工具与 Library 能力。新增视觉使用 imagegen，按 bundled scripts 完成九状态、十六方向及视觉验证；缺少工具不得伪造成功。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [imagegen](#skill-imagegen)：只有新绘制或修改视觉内容时调用 imagegen；元数据改名和原图只读验证不生成图像。
- [work-pets:update-pet](#skill-work-pets-update-pet)：按新建/更新/集合管理分别选主技能；仅用户同时要求多个生命周期操作时顺序交接，不重复解析目标。
- [work-pets:pets](#skill-work-pets-pets)：按新建/更新/集合管理分别选主技能；仅用户同时要求多个生命周期操作时顺序交接，不重复解析目标。

**调用方式与正反例：**

- 自动选择示例：“给 ChatGPT Work 创建一个会动的小狐狸伙伴。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 work-pets:create-pet 完成：给 ChatGPT Work 创建一个会动的小狐狸伙伴。”
- 不触发示例：“给我的 UE 游戏设计宠物跟随 AI。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**验证图集与动画、Library 产物、上传/创建结果；仅预览或校验请求不自动上传。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/work-pets/0.1.6/skills/create-pet/SKILL.md)。

<a id="skill-work-pets-pets"></a>

#### work-pets:pets

**功能：**列出、查看、选择、下载、关闭或删除 ChatGPT Work 动画宠物，使用稳定 pet ID。

**适用场景：**用户管理现有宠物集合或当前激活选择。

**自动触发条件（全部满足）：**

- 明确 ChatGPT Pets 或已有产品上下文。
- 请求是集合/选择/下载/删除操作。

**识别信号（可按语义匹配）：**`列出宠物`、`换一个 ChatGPT 伙伴`、`关闭宠物`、`下载图集`、`删除自定义宠物`。

**不触发与边界：**

- 真实动物护理不适用。
- 改现有宠物名字/图集走 update-pet。
- 隐藏、替换、清理不自动解释为删除，内置宠物不能删除。

**前置条件与实际可用性：**核实目标 ID 与工具连接；删除必须明确且目标唯一，关闭使用正常取消选择能力。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [work-pets:create-pet](#skill-work-pets-create-pet)：按新建/更新/集合管理分别选主技能；仅用户同时要求多个生命周期操作时顺序交接，不重复解析目标。
- [work-pets:update-pet](#skill-work-pets-update-pet)：按新建/更新/集合管理分别选主技能；仅用户同时要求多个生命周期操作时顺序交接，不重复解析目标。

**调用方式与正反例：**

- 自动选择示例：“把 ChatGPT 里的动画伙伴关掉。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 work-pets:pets 完成：把 ChatGPT 里的动画伙伴关掉。”
- 不触发示例：“把我的猫改名叫雪球。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**核对实际 active_pet_id 或删除结果；下载 URL 不是持久身份。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/work-pets/0.1.6/skills/pets/SKILL.md)。

<a id="skill-work-pets-update-pet"></a>

#### work-pets:update-pet

**功能：**在保持稳定 pet ID 的前提下修改名称、描述、图集，或只做校验/预览/修复。

**适用场景：**目标是已存在的自定义 ChatGPT 动画宠物。

**自动触发条件（全部满足）：**

- 明确现有自定义 ChatGPT 宠物。
- 需要修改元数据、图像或验证已有图集。

**识别信号（可按语义匹配）：**`修改伙伴名字`、`修复现有宠物`、`换图集`、`检查宠物动画`、`更新描述`。

**不触发与边界：**

- 创建新宠物走 create-pet。
- 只有改名不生成或上传图集。
- 只预览/验证不持久更新，更新不等于获准激活。

**前置条件与实际可用性：**由此 skill 自己定位稳定 ID，不为重复定位额外加载 pets；图像更改才调用 imagegen 与图集验证/上传。 当前会话清单可发现；文件/运行时/外部连接的实际执行能力需按前置条件核实。

**重叠选择：**按对象、动作、交付面选择；功能相近时使用本条列出的边界，不仅看关键词。

**组合规则：**

- [imagegen](#skill-imagegen)：只有新绘制或修改视觉内容时调用 imagegen；元数据改名和原图只读验证不生成图像。
- [work-pets:pets](#skill-work-pets-pets)：按新建/更新/集合管理分别选主技能；仅用户同时要求多个生命周期操作时顺序交接，不重复解析目标。

**调用方式与正反例：**

- 自动选择示例：“把 ChatGPT 现有小狐狸伙伴改名，其他保持不变。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 work-pets:update-pet 完成：把 ChatGPT 现有小狐狸伙伴改名，其他保持不变。”
- 不触发示例：“创建一个全新的动画伙伴。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**读回被修改字段与身份；仅在单独请求时改变激活选择。

**真实入口与来源：**[插件 SKILL.md](C:/Users/Kenwen/.codex/plugins/cache/openai-curated-remote/work-pets/0.1.6/skills/update-pet/SKILL.md)。


<a id="group-7"></a>

### 5.7 UE 基础 架构与构建

<a id="skill-ue-actor-component-architecture"></a>

#### ue-actor-component-architecture

**功能：**设计和排查 Actor/Component 的职责、组合、注册、初始化及销毁链。

**适用场景：**当前请求的核心问题是“设计和排查 Actor/Component 的职责、组合、注册、初始化及销毁链”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Actor`、`Component`、`OnRegister`、`BeginPlay`、`EndPlay`、`组件生命周期`。

**不触发与边界：**

- 一般编码不全量加载本条；只有生命周期或组合职责是问题核心时作为主 skill。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-game-features](#skill-ue-game-features)：组件由 GameFeature 注入或 Init State 协调时，追踪现有协议。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：动态组件对象回收或持有异常时，核查 GC。
- [ue-actors-and-components](#skill-ue-actors-and-components)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“角色换 Pawn 后组件重复初始化，帮我查生命周期。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-actor-component-architecture 完成：角色换 Pawn 后组件重复初始化，帮我查生命周期。”
- 不触发示例：“修改 IDE 的组件自动补全颜色。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-actor-component-architecture/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/actors.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-async-threading"></a>

#### ue-async-threading

**功能：**实现后台计算、UE::Tasks、异步回调及线程切换，管理取消与对象身份。

**适用场景：**当前请求的核心问题是“实现后台计算、UE::Tasks、异步回调及线程切换，管理取消与对象身份”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UE::Tasks`、`AsyncTask`、`FRunnable`、`线程`、`异步回调`。

**不触发与边界：**

- 一次性延迟不必建立线程；后台回调不能任意访问 UObject，依赖等待不使用无限轮询。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：后台结果保存 UObject 引用时，核查保活与失效。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：完成回调有重复绑定/未解绑证据时，核查委托。

**调用方式与正反例：**

- 自动选择示例：“异步加载完成后旧 Pawn 仍恢复状态，查一下回调身份。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-async-threading 完成：异步加载完成后旧 Pawn 仍恢复状态，查一下回调身份。”
- 不触发示例：“下周定时提醒我检查任务。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-async-threading/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/async.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-blueprint-cpp-interop"></a>

#### ue-blueprint-cpp-interop

**功能：**设计蓝图可见的 C++ 函数、属性、接口和异步节点契约。

**适用场景：**当前请求的核心问题是“设计蓝图可见的 C++ 函数、属性、接口和异步节点契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`BlueprintCallable`、`BlueprintNativeEvent`、`UPROPERTY`、`UFUNCTION`、`异步蓝图节点`。

**不触发与边界：**

- 改字段/签名先核对资产与重定向；不为所有内部函数添加 UFUNCTION。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：反射接口引起 UHT、导出或模块依赖问题时，补查构建。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：问题是 UObject 成员持有而非宏声明时，转入 GC。
- [ue-blueprint-cpp-integration](#skill-ue-blueprint-cpp-integration)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“把这个 C++ 查询作为纯函数暴露给角色蓝图。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-blueprint-cpp-interop 完成：把这个 C++ 查询作为纯函数暴露给角色蓝图。”
- 不触发示例：“画一张建筑蓝图示意图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-blueprint-cpp-interop/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/reflection.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-cpp-foundations"></a>

#### ue-cpp-foundations

**功能：**处理 UObject、反射、容器、基础生命周期和 UE C++ 声明方式。

**适用场景：**当前请求的核心问题是“处理 UObject、反射、容器、基础生命周期和 UE C++ 声明方式”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UCLASS`、`USTRUCT`、`UObject`、`GENERATED_BODY`、`UE C++基础`。

**不触发与边界：**

- 已知是 GC/模块/委托专项时选更窄 skill；不把基础规范当成每次全量重读理由。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：反射接口引起 UHT、导出或模块依赖问题时，补查构建。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：问题是 UObject 成员持有而非宏声明时，转入 GC。
- [ue-cpp-fundamentals](#skill-ue-cpp-fundamentals)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“这个 UObject 类的反射声明和指针持有是否正确？” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-cpp-foundations 完成：这个 UObject 类的反射声明和指针持有是否正确？”
- 不触发示例：“解释标准库 vector 的一般用法，不涉及 UE。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-cpp-foundations/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/reflection.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-framework"></a>

#### ue-gameplay-framework

**功能：**分析 GameMode/GameState/PlayerState/Controller/Pawn 职责、生成、占有与 Travel。

**适用场景：**当前请求的核心问题是“分析 GameMode/GameState/PlayerState/Controller/Pawn 职责、生成、占有与 Travel”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GameMode`、`PlayerState`、`PossessedBy`、`OnRep_PlayerState`、`重生`。

**不触发与边界：**

- 只有这些框架职责/生命周期是核心时选为主 skill，专项 GAS 问题通常先选 GAS。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-game-features](#skill-ue-game-features)：项目的生成/占有由 Experience 推进时，核查实际加载入口。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：PlayerState/Pawn 更换影响 ASC 时，核查 Owner/Avatar。
- [kb-ue-gameplay-framework](#skill-kb-ue-gameplay-framework)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“重生后 PlayerState 和 Pawn 的初始化顺序不对，沿调用链排查。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-framework 完成：重生后 PlayerState 和 Pawn 的初始化顺序不对，沿调用链排查。”
- 不触发示例：“生成一个 Notion 项目结构图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-framework/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/framework.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-module-build-system"></a>

#### ue-module-build-system

**功能：**修复 Build.cs、Target.cs、UHT、编译链接与模块依赖可见性。

**适用场景：**当前请求的核心问题是“修复 Build.cs、Target.cs、UHT、编译链接与模块依赖可见性”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Build.cs`、`Target.cs`、`UHT`、`链接错误`、`PublicDependencyModuleNames`。

**不触发与边界：**

- 保持 V5/Unreal5_5、单 Runtime 模块和常规构建；不为文档改动跑 UE 编译。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：符号来源、导出或 include 不确定时，查本机声明与模块。
- [ue-testing-debugging](#skill-ue-testing-debugging)：实际构建改动完成后，按风险选择回归和运行验证。
- [ue-module-and-build-system](#skill-ue-module-and-build-system)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“Editor 能编译但 Game 缺 UnrealEd，帮我修复依赖边界。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-module-build-system 完成：Editor 能编译但 Game 缺 UnrealEd，帮我修复依赖边界。”
- 不触发示例：“只排版一份使用文档。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-module-build-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/build.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-project-context"></a>

#### ue-project-context

**功能：**扫描并更新工程版本、模块、插件、初始化入口与验证状态的事实快照。

**适用场景：**当前请求的核心问题是“扫描并更新工程版本、模块、插件、初始化入口与验证状态的事实快照”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`项目上下文`、`工程现状`、`uproject`、`当前实现`、`接手项目`。

**不触发与边界：**

- 仅在需要整体事实基线或快照已过时使用；不能把历史文档当作运行证明。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：工程事实需要确认引擎 API/模块时，查本机源码。

**调用方式与正反例：**

- 自动选择示例：“先核实这个 UE 项目的模块、已启用插件和实际初始化入口。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-project-context 完成：先核实这个 UE 项目的模块、已启用插件和实际初始化入口。”
- 不触发示例：“只解释一行已有 C++ 代码。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-project-context/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/context.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-actors-and-components"></a>

#### ue-actors-and-components

**功能：**复核 Actor/组件构造、动态注册、附着、Tick 与生命周期细节。

**适用场景：**当前请求的核心问题是“复核 Actor/组件构造、动态注册、附着、Tick 与生命周期细节”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`动态组件`、`AddInstanceComponent`、`RegisterComponent`、`AttachToComponent`。

**不触发与边界：**

- 与主 actor-component skill 同职责时默认不双载；用于底层签名或特殊生命周期复核。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-actor-component-architecture`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-game-features](#skill-ue-game-features)：组件由 GameFeature 注入或 Init State 协调时，追踪现有协议。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：动态组件对象回收或持有异常时，核查 GC。
- [ue-actor-component-architecture](#skill-ue-actor-component-architecture)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实运行时添加组件的持有、注册与 EndPlay 清理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-actors-and-components 完成：核实运行时添加组件的持有、注册与 EndPlay 清理。”
- 不触发示例：“修改 UI 设计图的组件摆放。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-actors-and-components/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/actors.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-blueprint-cpp-integration"></a>

#### ue-blueprint-cpp-integration

**功能：**复核 UHT 生成的蓝图接口、事件、反射元数据和跨 C++ 调用细节。

**适用场景：**当前请求的核心问题是“复核 UHT 生成的蓝图接口、事件、反射元数据和跨 C++ 调用细节”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UHT接口`、`BlueprintImplementableEvent`、`元数据`、`蓝图C++边界`。

**不触发与边界：**

- 与主 interop 重叠时只选一条；反射变更仍须冷启动与资产验证。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-blueprint-cpp-interop`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：反射接口引起 UHT、导出或模块依赖问题时，补查构建。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：问题是 UObject 成员持有而非宏声明时，转入 GC。
- [ue-blueprint-cpp-interop](#skill-ue-blueprint-cpp-interop)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实 BlueprintNativeEvent 的声明和 _Implementation 签名。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-blueprint-cpp-integration 完成：核实 BlueprintNativeEvent 的声明和 _Implementation 签名。”
- 不触发示例：“把一个建筑规划草图转换为图片。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-blueprint-cpp-integration/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/reflection.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-coding-standards"></a>

#### ue-coding-standards

**功能：**针对明确的 UE 命名、反射、头文件、格式与注释审查需求核对规范。

**适用场景：**当前请求的核心问题是“针对明确的 UE 命名、反射、头文件、格式与注释审查需求核对规范”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UE编码规范`、`命名前缀`、`include规范`、`代码风格审查`。

**不触发与边界：**

- 全局项目规范始终生效，不因代码里出现 UCLASS 就加载全部风格指导或重排无关文件。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：反射接口引起 UHT、导出或模块依赖问题时，补查构建。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：问题是 UObject 成员持有而非宏声明时，转入 GC。

**调用方式与正反例：**

- 自动选择示例：“审查这几个 UE 类的命名和反射头文件顺序。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-coding-standards 完成：审查这几个 UE 类的命名和反射头文件顺序。”
- 不触发示例：“修一个明确的 GAS 激活失败，只需最小修复。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-coding-standards/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/reflection.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-core-types-and-containers"></a>

#### ue-core-types-and-containers

**功能：**选择 FName/FText/FString、UE 容器、数学和常用模板类型。

**适用场景：**当前请求的核心问题是“选择 FName/FText/FString、UE 容器、数学和常用模板类型”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`TArray`、`TMap`、`TSet`、`FText`、`FName`、`FString`。

**不触发与边界：**

- 纯字符串/容器问题不自动牵涉 GAS；有 UObject 保活问题再加 GC 专项。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：容器或字符串问题实际涉及 UObject 生命周期时，再核实持有关系。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：需要定义和绑定回调接口时，选择委托专项。

**调用方式与正反例：**

- 自动选择示例：“TArray 扩容后缓存指针崩溃，检查失效规则。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-core-types-and-containers 完成：TArray 扩容后缓存指针崩溃，检查失效规则。”
- 不触发示例：“修改数据库里的一列名称。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-core-types-and-containers/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/core.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-cpp-fundamentals"></a>

#### ue-cpp-fundamentals

**功能：**复核 UE C++ 反射、CDO、对象创建、容器和基础指针约定。

**适用场景：**当前请求的核心问题是“复核 UE C++ 反射、CDO、对象创建、容器和基础指针约定”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`CDO`、`NewObject`、`UObject基础`、`反射宏`。

**不触发与边界：**

- Kevin 来源基础复核入口；普通新实现与 ue-cpp-foundations 不必重复读取。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-cpp-foundations`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：反射接口引起 UHT、导出或模块依赖问题时，补查构建。
- [ue-memory-and-gc](#skill-ue-memory-and-gc)：问题是 UObject 成员持有而非宏声明时，转入 GC。
- [ue-cpp-foundations](#skill-ue-cpp-foundations)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“解释这个 UE 类的 CDO 与实例创建差异并核对声明。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-cpp-fundamentals 完成：解释这个 UE 类的 CDO 与实例创建差异并核对声明。”
- 不触发示例：“编写纯标准 C++ 命令行工具。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-cpp-fundamentals/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/reflection.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-delegates-and-events"></a>

#### ue-delegates-and-events

**功能：**选择普通/Dynamic/多播委托，处理签名、绑定、广播与句柄解绑。

**适用场景：**当前请求的核心问题是“选择普通/Dynamic/多播委托，处理签名、绑定、广播与句柄解绑”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`FDelegateHandle`、`AddUObject`、`AddDynamic`、`AddWeakLambda`、`委托`。

**不触发与边界：**

- 弱对象有效不代表动作身份有效；线程安全委托不使业务 UObject 自动线程安全。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：需要解释对象保活和弱引用行为时，核查持有关系。
- [ue-async-threading](#skill-ue-async-threading)：回调跨线程或来自异步操作时，核查线程切换。

**调用方式与正反例：**

- 自动选择示例：“重生后委托重复触发，检查注册、解绑和旧 Pawn 回调。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-delegates-and-events 完成：重生后委托重复触发，检查注册、解绑和旧 Pawn 回调。”
- 不触发示例：“每周定时发送项目报告。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-delegates-and-events/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/delegates.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-architecture-planning"></a>

#### ue-gameplay-architecture-planning

**功能：**核查 Lyra 与现有代码后规划职责、状态、权限、依赖、数据与验收方案。

**适用场景：**当前请求的核心问题是“核查 Lyra 与现有代码后规划职责、状态、权限、依赖、数据与验收方案”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`架构方案`、`玩法系统设计`、`职责边界`、`调用链设计`。

**不触发与边界：**

- 方案不等于实现；不能把 Lyra unimplemented/TODO 宣称成熟功能，也不增加重复系统。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-project-context](#skill-ue-project-context)：整体基线过时或不明确时先核查工程事实。
- [ue-game-features](#skill-ue-game-features)：方案涉及 Experience/注入/初始化时复用现有协议。

**调用方式与正反例：**

- 自动选择示例：“设计背包改动前查 Lyra 完成部分与占位，并给最小适配方案。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-architecture-planning 完成：设计背包改动前查 Lyra 完成部分与占位，并给最小适配方案。”
- 不触发示例：“直接修改一个注释错字。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-architecture-planning/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/architecture.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-kb-ue-gameplay-framework"></a>

#### kb-ue-gameplay-framework

**功能：**复核玩法框架的类关系、登录生成、Possession、PlayerState 与子系统生命周期。

**适用场景：**当前请求的核心问题是“复核玩法框架的类关系、登录生成、Possession、PlayerState 与子系统生命周期”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`框架关系复核`、`登录生成`、`Controller生命周期`。

**不触发与边界：**

- Kevin 重名入口；一般 Hodge 初始化排查优先 ue-gameplay-framework。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-gameplay-framework`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-game-features](#skill-ue-game-features)：项目的生成/占有由 Experience 推进时，核查实际加载入口。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：PlayerState/Pawn 更换影响 ASC 时，核查 Owner/Avatar。
- [ue-gameplay-framework](#skill-ue-gameplay-framework)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“复核服务器登录到 Pawn Possession 的框架调用顺序。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 kb-ue-gameplay-framework 完成：复核服务器登录到 Pawn Possession 的框架调用顺序。”
- 不触发示例：“把需求拆成多个 GitHub PR。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/kb-ue-gameplay-framework/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/framework.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-logging-and-assertions"></a>

#### ue-logging-and-assertions

**功能：**设计 UE_LOG 分类、verbosity、check/ensure 和可诊断失败路径。

**适用场景：**当前请求的核心问题是“设计 UE_LOG 分类、verbosity、check/ensure 和可诊断失败路径”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UE_LOG`、`LogCategory`、`ensure`、`check`、`日志分类`。

**不触发与边界：**

- 坏配置不以 check 崩溃替代诊断；日志不能输出令牌等敏感值。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：容器或字符串问题实际涉及 UObject 生命周期时，再核实持有关系。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：需要定义和绑定回调接口时，选择委托专项。

**调用方式与正反例：**

- 自动选择示例：“为资源缺失区分等待、配置错误和运行失败，并输出可定位日志。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-logging-and-assertions 完成：为资源缺失区分等待、配置错误和运行失败，并输出可定位日志。”
- 不触发示例：“查看我本次会话还有多少额度。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-logging-and-assertions/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/core.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-memory-and-gc"></a>

#### ue-memory-and-gc

**功能：**选择 UObject 保活、强弱软指针、FGCObject 与普通 C++ 智能指针并排查 GC 崩溃。

**适用场景：**当前请求的核心问题是“选择 UObject 保活、强弱软指针、FGCObject 与普通 C++ 智能指针并排查 GC 崩溃”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UPROPERTY`、`TObjectPtr`、`TWeakObjectPtr`、`TStrongObjectPtr`、`GC`、`FGCObject`。

**不触发与边界：**

- Outer 不等于保活证明；TSharedPtr 不管理 UObject，软路径不等于加载后强引用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-delegates-and-events](#skill-ue-delegates-and-events)：委托/回调持有或解绑引起崩溃时，核查对应注册与撤销。
- [ue-async-threading](#skill-ue-async-threading)：对象跨异步回调使用时，核查线程与请求身份。

**调用方式与正反例：**

- 自动选择示例：“异步操作完成时对象被 GC，分析持有关系而非简单 AddToRoot。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-memory-and-gc 完成：异步操作完成时对象被 GC，分析持有关系而非简单 AddToRoot。”
- 不触发示例：“优化一条 SQL 内存查询。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-memory-and-gc/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/gc.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-module-and-build-system"></a>

#### ue-module-and-build-system

**功能：**深入复核 UBT 模块、Build.cs、Target、导出与构建配置。

**适用场景：**当前请求的核心问题是“深入复核 UBT 模块、Build.cs、Target、导出与构建配置”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UBT规则`、`API导出宏`、`TargetRules`、`ModuleRules`。

**不触发与边界：**

- 保持 Unreal5_5 和 V5；与主 module-build-system 不在同阶段重复读取。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-module-build-system`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：符号来源、导出或 include 不确定时，查本机声明与模块。
- [ue-testing-debugging](#skill-ue-testing-debugging)：实际构建改动完成后，按风险选择回归和运行验证。
- [ue-module-build-system](#skill-ue-module-build-system)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“查公开头文件类型的模块依赖为何只在 Editor 编译正常。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-module-and-build-system 完成：查公开头文件类型的模块依赖为何只在 Editor 编译正常。”
- 不触发示例：“将代码格式化为两空格缩进。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-module-and-build-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/build.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-navigating-engine-source"></a>

#### ue-navigating-engine-source

**功能：**定位本机精确声明、定义、调用者、模块、include 与弃用标记。

**适用场景：**当前请求的核心问题是“定位本机精确声明、定义、调用者、模块、include 与弃用标记”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`查引擎源码`、`精确签名`、`头文件在哪`、`模块来源`、`API版本`。

**不触发与边界：**

- 源码定位是核实手段，不把同名符号或文件存在当作完整行为证明。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：定位结果涉及依赖可见性或链接错误时，修改相应 Build.cs 边界。

**调用方式与正反例：**

- 自动选择示例：“GetDynamicSpecSourceTags 在 5.5 可用吗，查本机声明和调用。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-navigating-engine-source 完成：GetDynamicSpecSourceTags 在 5.5 可用吗，查本机声明和调用。”
- 不触发示例：“直接按 5.8 文档升级项目。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-navigating-engine-source/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/source.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-plugins-and-modules"></a>

#### ue-plugins-and-modules

**功能：**处理 .uplugin 描述、模块类型、加载阶段、可选依赖与项目启用关系。

**适用场景：**当前请求的核心问题是“处理 .uplugin 描述、模块类型、加载阶段、可选依赖与项目启用关系”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`.uplugin`、`LoadingPhase`、`插件模块`、`EditorNoCommandlet`。

**不触发与边界：**

- UE 插件与应用连接器不同；不顺手升级 ALS/RiderLink，依赖变更仍需 Game 构建。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：符号来源、导出或 include 不确定时，查本机声明与模块。
- [ue-testing-debugging](#skill-ue-testing-debugging)：实际构建改动完成后，按风险选择回归和运行验证。

**调用方式与正反例：**

- 自动选择示例：“核实这个项目插件的 Runtime/Editor 依赖和加载阶段。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-plugins-and-modules 完成：核实这个项目插件的 Runtime/Editor 依赖和加载阶段。”
- 不触发示例：“给 Codex 安装 Gmail 插件。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-plugins-and-modules/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/build.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-project-structure"></a>

#### ue-project-structure

**功能：**核实 .uproject、Source/Config/Content、挂载路径和版本控制边界。

**适用场景：**当前请求的核心问题是“核实 .uproject、Source/Config/Content、挂载路径和版本控制边界”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`工程目录`、`挂载路径`、`uproject模块`、`源码组织`。

**不触发与边界：**

- 本项目目录约定优先，不为整理引入模块或重命名资产。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：符号来源、导出或 include 不确定时，查本机声明与模块。
- [ue-testing-debugging](#skill-ue-testing-debugging)：实际构建改动完成后，按风险选择回归和运行验证。

**调用方式与正反例：**

- 自动选择示例：“解释现有模块和 Public/Private 目录，不增加业务模块。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-project-structure 完成：解释现有模块和 Public/Private 目录，不增加业务模块。”
- 不触发示例：“把现有单类实现按功能拆成多个 cpp。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-project-structure/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/build.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-subsystems"></a>

#### ue-subsystems

**功能：**按 Engine/GameInstance/World/LocalPlayer 生命周期选择与扩展 Subsystem。

**适用场景：**当前请求的核心问题是“按 Engine/GameInstance/World/LocalPlayer 生命周期选择与扩展 Subsystem”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Subsystem`、`InitializeDependency`、`Deinitialize`、`世界子系统`。

**不触发与边界：**

- 已有系统能承担时优先扩展；不要仅为持久化状态引入全局静态对象。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-actor-component-architecture](#skill-ue-actor-component-architecture)：比较对象/组件与子系统的生命周期职责时，结合现有持有者选择。
- [ue-async-threading](#skill-ue-async-threading)：子系统发起异步工作时，核查 Deinitialize 后的回调与世界身份。

**调用方式与正反例：**

- 自动选择示例：“多 PIE 世界共享了错误缓存，检查 Subsystem 所有权。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-subsystems 完成：多 PIE 世界共享了错误缓存，检查 Subsystem 所有权。”
- 不触发示例：“创建一个额外 Runtime 业务模块来管理缓存。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-subsystems/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/subsystems.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-timers-and-async"></a>

#### ue-timers-and-async

**功能：**复核 FTimerManager、Ticker、UE::Tasks、异步线程及配对取消。

**适用场景：**当前请求的核心问题是“复核 FTimerManager、Ticker、UE::Tasks、异步线程及配对取消”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`FTimerHandle`、`SetTimer`、`ClearTimer`、`Ticker`、`延迟回调`。

**不触发与边界：**

- 这是游戏运行时 Timer，不是 Codex 定时任务；不要通过无限轮询等初始化依赖。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：后台结果保存 UObject 引用时，核查保活与失效。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：完成回调有重复绑定/未解绑证据时，核查委托。

**调用方式与正反例：**

- 自动选择示例：“攻击取消后 Timer 仍触发，按执行身份清理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-timers-and-async 完成：攻击取消后 Timer 仍触发，按执行身份清理。”
- 不触发示例：“明天提醒我检查攻击动画。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-timers-and-async/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/async.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-8"></a>

### 5.8 UE 玩法 输入 网络与相机

<a id="skill-ue-character-movement"></a>

#### ue-character-movement

**功能：**实现和排查 CharacterMovement、SavedMove、根运动、预测与纠正。

**适用场景：**当前请求的核心问题是“实现和排查 CharacterMovement、SavedMove、根运动、预测与纠正”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`CharacterMovement`、`SavedMove`、`PhysCustom`、`网络移动`、`地面移动`。

**不触发与边界：**

- 优先现有 CMC；客户端位移正常不能证明服务器和模拟代理正确。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-networking-replication](#skill-ue-networking-replication)：出现多端纠正/拥有者差异时，补充网络验证。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：移动由 Dash/Sprint 等能力持有时，核查取消与参数恢复。
- [ue-character-and-movement](#skill-ue-character-and-movement)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“自定义冲刺在客户端被服务器拉回，检查 CMC 预测。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-character-movement 完成：自定义冲刺在客户端被服务器拉回，检查 CMC 预测。”
- 不触发示例：“修改网页滚动动画。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-character-movement/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/movement.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-game-features"></a>

#### ue-game-features

**功能：**实现 GameFeature Action、Experience、组件注入、Init State 与停用清理。

**适用场景：**当前请求的核心问题是“实现 GameFeature Action、Experience、组件注入、Init State 与停用清理”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GameFeature`、`Experience`、`PawnData`、`Init State`、`组件注入`。

**不触发与边界：**

- Lyra Experience/AddAbilities 是样例/项目实现，不是同名 Engine 现成功能；先核对本地调用链。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：Action 实际授予能力/效果/属性时，核查句柄与 ASC 所有权。
- [ue-data-assets-tables](#skill-ue-data-assets-tables)：Experience/PawnData 加载、软引用或 Bundle 是问题部分时，核查资源契约。

**调用方式与正反例：**

- 自动选择示例：“Experience 加载完成后为角色授予组件，停用时正确撤销。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-game-features 完成：Experience 加载完成后为角色授予组件，停用时正确撤销。”
- 不触发示例：“给 AI 助手安装一个新 skill。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-game-features/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/gamefeatures.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-abilities"></a>

#### ue-gameplay-abilities

**功能：**实现 GAS 能力、属性、效果、AbilityTask、授予撤销、预测和取消。

**适用场景：**当前请求的核心问题是“实现 GAS 能力、属性、效果、AbilityTask、授予撤销、预测和取消”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GAS`、`GameplayAbility`、`ASC`、`GameplayEffect`、`冷却`、`技能激活`。

**不触发与边界：**

- 玩家 Owner 与 Pawn Avatar 分开；沿 Init State 初始化，不照搬通用 BeginPlay 示例。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-animation-system](#skill-ue-animation-system)：能力实际播放 Montage、使用 Notify 或根运动时，辅助处理动画生命周期。
- [ue-networking-replication](#skill-ue-networking-replication)：问题涉及 RPC、预测、属性复制或多端权限时，核查网络边界。
- [ue-game-features](#skill-ue-game-features)：授予来自 Experience/GameFeature 时，追踪激活与撤销来源。
- [ue-gameplay-ability-system](#skill-ue-gameplay-ability-system)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“给角色增加可取消的攻击能力，保留 PlayerState ASC。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-abilities 完成：给角色增加可取消的攻击能力，保留 PlayerState ASC。”
- 不触发示例：“为 Codex 编写一个 SKILL.md。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-abilities/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/gas.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-cameras"></a>

#### ue-gameplay-cameras

**功能：**沿现有 CameraMode 栈设计技能、锁定和过场相机，统一应用视图。

**适用场景：**当前请求的核心问题是“沿现有 CameraMode 栈设计技能、锁定和过场相机，统一应用视图”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`CameraMode`、`CameraModeStack`、`PlayerCameraManager`、`技能镜头`。

**不触发与边界：**

- GameplayCameras 插件存在不代表本项目使用它；不替换现有 Hodge 相机栈。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：技能持有相机模式请求时，核查句柄和执行身份。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：过场接管视图时，核查序列中断与恢复。

**调用方式与正反例：**

- 自动选择示例：“技能结束后恢复默认相机，防止旧回调覆盖新镜头。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-cameras 完成：技能结束后恢复默认相机，防止旧回调覆盖新镜头。”
- 不触发示例：“给网页添加摄像头输入。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-cameras/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/cameras.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-tags-messaging"></a>

#### ue-gameplay-tags-messaging

**功能：**设计和排查 Native Tags、INI 标签、层级匹配、查询、通知与复制。

**适用场景：**当前请求的核心问题是“设计和排查 Native Tags、INI 标签、层级匹配、查询、通知与复制”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GameplayTag`、`NativeGameplayTags`、`MatchesTag`、`TagQuery`、`标签消息`。

**不触发与边界：**

- 标签事实由对应系统维护；不把 Lyra GameplayMessageRuntime 当作本项目已启用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：标签表达技能激活、阻塞、取消或效果状态时，追踪 ASC 标签来源。
- [ue-gameplay-tags](#skill-ue-gameplay-tags)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“取消技能的标签层级匹配错误，检查定义与查询。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-tags-messaging 完成：取消技能的标签层级匹配错误，检查定义与查询。”
- 不触发示例：“给 GitHub issue 增加一个标签。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-tags-messaging/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/tags.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-input-system"></a>

#### ue-input-system

**功能：**沿 Hero/InputConfig 接入 Enhanced Input 的动作、上下文、触发与重绑定。

**适用场景：**当前请求的核心问题是“沿 Hero/InputConfig 接入 Enhanced Input 的动作、上下文、触发与重绑定”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Enhanced Input`、`IA_`、`IMC_`、`InputAction`、`InputTag`、`按键映射`。

**不触发与边界：**

- 只给本地玩家安装映射，重复初始化需解绑；5.8 Input Mode Filtering 不作为 5.5 配置前提。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：输入通过 InputTag 激活能力时，核对按下/释放/取消链。
- [ue-ui-umg-slate](#skill-ue-ui-umg-slate)：UI 焦点或 CommonUI 输入路由截获输入时，核查各自职责。
- [ue-enhanced-input](#skill-ue-enhanced-input)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“长按冲刺释放后仍持续，检查 Completed/Canceled 与 GAS 输入。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-input-system 完成：长按冲刺释放后仍持续，检查 Completed/Canceled 与 GAS 输入。”
- 不触发示例：“修改 CLI 的 Vim 输入模式。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-input-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/input.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-mover"></a>

#### ue-mover

**功能：**核查实验性 Mover 的本机 API、输入/同步数据和网络后端适用性。

**适用场景：**当前请求的核心问题是“核查实验性 Mover 的本机 API、输入/同步数据和网络后端适用性”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Mover`、`UMoverComponent`、`移动后端`、`同步状态`。

**不触发与边界：**

- 依赖/架构评估优先；安装 skill 不启用 Mover，也不把 5.8 移动 API 当作现有接口。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-character-movement](#skill-ue-character-movement)：评估结果缺少本机能力时说明现有 CMC 的替代路径，不自动迁移。
- [ue-mover-movement-system](#skill-ue-mover-movement-system)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“评估 UE5.5 的 Mover 能否支持这个明确需求，不迁移现有 CMC。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-mover 完成：评估 UE5.5 的 Mover 能否支持这个明确需求，不迁移现有 CMC。”
- 不触发示例：“修复当前 CharacterMovement 的普通跳跃参数。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-mover/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/mover.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-networking-replication"></a>

#### ue-networking-replication

**功能：**实现属性复制、OnRep、RPC、子对象、FastArray、Push Model 与权限校验。

**适用场景：**当前请求的核心问题是“实现属性复制、OnRep、RPC、子对象、FastArray、Push Model 与权限校验”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`OnRep`、`RPC`、`DOREPLIFETIME`、`FastArray`、`Iris`、`服务器权限`。

**不触发与边界：**

- SetupIrisSupport 不证明运行时启用 Iris；PIE Dedicated 不证明已有 Server Target。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：复制对象是 ASC、属性或能力预测时，保留 GAS 协议。
- [ue-character-movement](#skill-ue-character-movement)：问题是移动纠正/SavedMove 时，由移动专项处理。
- [ue-networking-and-replication](#skill-ue-networking-and-replication)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“装备列表复制丢更新，检查 FastArray 脏标记和拥有关系。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-networking-replication 完成：装备列表复制丢更新，检查 FastArray 脏标记和拥有关系。”
- 不触发示例：“将本地文件复制到另一个目录。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-networking-replication/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/network.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-character-and-movement"></a>

#### ue-character-and-movement

**功能：**复核 ACharacter 的跳跃、蹲伏、旋转、RootMotion 和运动组件契约。

**适用场景：**当前请求的核心问题是“复核 ACharacter 的跳跃、蹲伏、旋转、RootMotion 和运动组件契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`ACharacter`、`Jump`、`Crouch`、`RotationRate`、`RootMotionSource`。

**不触发与边界：**

- 当前 CMC 行为细节使用本条；不要转到实验性 Mover。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-character-movement`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-networking-replication](#skill-ue-networking-replication)：出现多端纠正/拥有者差异时，补充网络验证。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：移动由 Dash/Sprint 等能力持有时，核查取消与参数恢复。
- [ue-character-movement](#skill-ue-character-movement)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实蹲伏与胶囊变化在服务器、客户端的现有处理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-character-and-movement 完成：核实蹲伏与胶囊变化在服务器、客户端的现有处理。”
- 不触发示例：“在网页上让卡片移动。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-character-and-movement/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/movement.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-enhanced-input"></a>

#### ue-enhanced-input

**功能：**复核 EnhancedInput 触发、Modifier、输入值、映射优先级和本地玩家安装细节。

**适用场景：**当前请求的核心问题是“复核 EnhancedInput 触发、Modifier、输入值、映射优先级和本地玩家安装细节”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`InputModifier`、`FInputActionValue`、`ETriggerEvent`、`IMC优先级`。

**不触发与边界：**

- 一般玩家输入接入主用 ue-input-system；同阶段不重复读取两套完整基础说明。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-input-system`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：输入通过 InputTag 激活能力时，核对按下/释放/取消链。
- [ue-ui-umg-slate](#skill-ue-ui-umg-slate)：UI 焦点或 CommonUI 输入路由截获输入时，核查各自职责。
- [ue-input-system](#skill-ue-input-system)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实二维 WASD 的 Modifier 和映射上下文优先级。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-enhanced-input 完成：核实二维 WASD 的 Modifier 和映射上下文优先级。”
- 不触发示例：“让终端支持 Vim 快捷键。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-enhanced-input/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/input.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-ability-system"></a>

#### ue-gameplay-ability-system

**功能：**复核 ASC、Ability、AttributeSet、Effect、Task 与复制的 GAS 具体接口。

**适用场景：**当前请求的核心问题是“复核 ASC、Ability、AttributeSet、Effect、Task 与复制的 GAS 具体接口”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GAS接口复核`、`属性复制`、`AbilityTask签名`、`ASC模式`。

**不触发与边界：**

- 普通实现默认 ue-gameplay-abilities；本条用于深度复核或用户指定。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-gameplay-abilities`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-animation-system](#skill-ue-animation-system)：能力实际播放 Montage、使用 Notify 或根运动时，辅助处理动画生命周期。
- [ue-networking-replication](#skill-ue-networking-replication)：问题涉及 RPC、预测、属性复制或多端权限时，核查网络边界。
- [ue-game-features](#skill-ue-game-features)：授予来自 Experience/GameFeature 时，追踪激活与撤销来源。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“复核 AttributeSet 复制回调与能力任务清理的 5.5 契约。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-ability-system 完成：复核 AttributeSet 复制回调与能力任务清理的 5.5 契约。”
- 不触发示例：“创建一个 Agent 能力目录。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-ability-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/gas.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-gameplay-tags"></a>

#### ue-gameplay-tags

**功能：**复核 GameplayTag 的定义、容器、匹配、查询、接口和标签分类。

**适用场景：**当前请求的核心问题是“复核 GameplayTag 的定义、容器、匹配、查询、接口和标签分类”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Native Tag`、`MatchesTagExact`、`GameplayTagContainer`、`标签分类`。

**不触发与边界：**

- 保留项目既有 Tag 名称与资产重定向，不为分类美观批量重命名。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-gameplay-tags-messaging`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：标签表达技能激活、阻塞、取消或效果状态时，追踪 ASC 标签来源。
- [ue-gameplay-tags-messaging](#skill-ue-gameplay-tags-messaging)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“检查标签分类和 MatchesTagExact 是否导致子标签不匹配。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-gameplay-tags 完成：检查标签分类和 MatchesTagExact 是否导致子标签不匹配。”
- 不触发示例：“给 Notion 页面打标签。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-gameplay-tags/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/tags.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-mover-movement-system"></a>

#### ue-mover-movement-system

**功能：**复核本机实验 Mover 的组件、模式、输入和同步数据结构。

**适用场景：**当前请求的核心问题是“复核本机实验 Mover 的组件、模式、输入和同步数据结构”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Mover模式`、`UMoverComponent接口`、`Mover网络后端`。

**不触发与边界：**

- 仅做存在性/适用性核查不启用或迁移框架；不存在 API 要明确说明。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-mover`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-character-movement](#skill-ue-character-movement)：评估结果缺少本机能力时说明现有 CMC 的替代路径，不自动迁移。
- [ue-mover](#skill-ue-mover)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“查 5.8 Mover 示例在本机 5.5 缺哪些结构，给现有 CMC 替代路径。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-mover-movement-system 完成：查 5.8 Mover 示例在本机 5.5 缺哪些结构，给现有 CMC 替代路径。”
- 不触发示例：“微调当前 CMC 的步高。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-mover-movement-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/mover.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-networking-and-replication"></a>

#### ue-networking-and-replication

**功能：**深入复核网络角色、拥有链、RPC、相关性、休眠与复制机制。

**适用场景：**当前请求的核心问题是“深入复核网络角色、拥有链、RPC、相关性、休眠与复制机制”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`网络角色`、`拥有链`、`Relevancy`、`Dormancy`、`子对象注册`。

**不触发与边界：**

- 主实现默认 networking-replication；需要底层机制复核时才补充本条。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-networking-replication`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：复制对象是 ASC、属性或能力预测时，保留 GAS 协议。
- [ue-character-movement](#skill-ue-character-movement)：问题是移动纠正/SavedMove 时，由移动专项处理。
- [ue-networking-replication](#skill-ue-networking-replication)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“复核拥有者 RPC 为什么被拒绝，区分 Owner 与 Authority。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-networking-and-replication 完成：复核拥有者 RPC 为什么被拒绝，区分 Owner 与 Authority。”
- 不触发示例：“复制一份 PNG 图片。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-networking-and-replication/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/network.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-9"></a>

### 5.9 UE 动画 骨架与网格

<a id="skill-ue-animation-system"></a>

#### ue-animation-system

**功能：**处理 AnimInstance、Montage、Notify、动画层、根运动和 GAS 动画集成。

**适用场景：**当前请求的核心问题是“处理 AnimInstance、Montage、Notify、动画层、根运动和 GAS 动画集成”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`AnimInstance`、`Montage`、`Notify`、`AnimGraph`、`根运动`、`动画层`。

**不触发与边界：**

- 动画图和 EventGraph 工具能力不同；不因同名技能把 5.8 动画插件迁入 5.5。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：动画由能力驱动时，核实激活、取消和任务结束。
- [ue-character-movement](#skill-ue-character-movement)：涉及 RootMotion/运动预测时，核查运动执行者。
- [unreal-mcp](#skill-unreal-mcp)：确需读取或编辑具体动画资产时，先发现工具实际支持的图类型。
- [kb-ue-animation-system](#skill-kb-ue-animation-system)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“攻击 Montage 取消后仍触发结束回调，修复动作身份判断。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-animation-system 完成：攻击 Montage 取消后仍触发结束回调，修复动作身份判断。”
- 不触发示例：“给 Word 文档增加入场动画。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-animation-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/animation.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-kb-ue-animation-system"></a>

#### kb-ue-animation-system

**功能：**复核动画状态机、AnimInstance 更新、曲线、Montage 与通知的底层契约。

**适用场景：**当前请求的核心问题是“复核动画状态机、AnimInstance 更新、曲线、Montage 与通知的底层契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`动画线程`、`BlendSpace`、`状态机`、`曲线`、`Montage接口`。

**不触发与边界：**

- 这是 Kevin 来源的重名保留入口；普通 GAS 动画接入默认优先 ue-animation-system。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-animation-system`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：动画由能力驱动时，核实激活、取消和任务结束。
- [ue-character-movement](#skill-ue-character-movement)：涉及 RootMotion/运动预测时，核查运动执行者。
- [unreal-mcp](#skill-unreal-mcp)：确需读取或编辑具体动画资产时，先发现工具实际支持的图类型。
- [ue-animation-system](#skill-ue-animation-system)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“复核 AnimInstance 更新线程能否安全读取这个字段。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 kb-ue-animation-system 完成：复核 AnimInstance 更新线程能否安全读取这个字段。”
- 不触发示例：“生成一个 ChatGPT 动画宠物。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/kb-ue-animation-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/animation.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-control-rig-and-ik"></a>

#### ue-control-rig-and-ik

**功能：**处理 Control Rig、IK Rig、IK Retargeter、骨架映射和运行时控制空间。

**适用场景：**当前请求的核心问题是“处理 Control Rig、IK Rig、IK Retargeter、骨架映射和运行时控制空间”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`ControlRig`、`IK Rig`、`IK Retargeter`、`重定向`、`足部IK`。

**不触发与边界：**

- 5.8 操作栈/API 不自动适用于 5.5；不批量重存 ALS 资产。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-animation-system](#skill-ue-animation-system)：Rig/重定向需要接入 AnimInstance/AnimGraph 时，核查动画更新。
- [ue-meshes-static-and-skeletal](#skill-ue-meshes-static-and-skeletal)：涉及 Skeleton/Socket/参考姿势时，核查网格资源。

**调用方式与正反例：**

- 自动选择示例：“检查两套骨架的 IK Retargeter 与参考姿势，按 5.5 实际接口操作。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-control-rig-and-ik 完成：检查两套骨架的 IK Retargeter 与参考姿势，按 5.5 实际接口操作。”
- 不触发示例：“在图片里画出一个角色骨架。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-control-rig-and-ik/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/rig.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-meshes-static-and-skeletal"></a>

#### ue-meshes-static-and-skeletal

**功能：**检查静态/骨骼网格、Skeleton、Socket、LOD、材质槽和物理资产。

**适用场景：**当前请求的核心问题是“检查静态/骨骼网格、Skeleton、Socket、LOD、材质槽和物理资产”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`SkeletalMesh`、`StaticMesh`、`Socket`、`LOD`、`Skeleton`。

**不触发与边界：**

- 资源存在不等于骨架/动画兼容；只保存任务关联资产。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-importing-content](#skill-ue-importing-content)：需要导入或重导入资源时，保持明确覆盖范围。
- [ue-control-rig-and-ik](#skill-ue-control-rig-and-ik)：问题在骨架映射/IK 时，补充专项。

**调用方式与正反例：**

- 自动选择示例：“装备附着到 Socket 后位置错误，核对网格和骨架变换。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-meshes-static-and-skeletal 完成：装备附着到 Socket 后位置错误，核对网格和骨架变换。”
- 不触发示例：“画一个低多边形角色概念图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-meshes-static-and-skeletal/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/meshes.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-10"></a>

### 5.10 UE 数据 资源 世界与存档

<a id="skill-ue-data-assets-tables"></a>

#### ue-data-assets-tables

**功能：**设计 DataAsset/DataTable、软引用、资产管理、异步加载和 Cook 可达性。

**适用场景：**当前请求的核心问题是“设计 DataAsset/DataTable、软引用、资产管理、异步加载和 Cook 可达性”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`DataAsset`、`DataTable`、`PrimaryAsset`、`软引用`、`StreamableHandle`。

**不触发与边界：**

- 资产内部值需真实读取；文件存在不说明字段已配置或打包包含。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：资源加载后持有与回收是问题时，补查 GC。
- [ue-packaging-and-deployment](#skill-ue-packaging-and-deployment)：问题在 Cook/包内可达性时，核查打包规则；不自动执行发布。

**调用方式与正反例：**

- 自动选择示例：“把技能可调参数放入现有 Profile，并核对异步加载与 Cook。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-data-assets-tables 完成：把技能可调参数放入现有 Profile，并核对异步加载与 Cook。”
- 不触发示例：“在 Notion 设计一个任务数据库。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-data-assets-tables/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/assets.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-serialization-savegames"></a>

#### ue-serialization-savegames

**功能：**设计 USaveGame、槽位、异步保存、二进制归档、版本迁移及恢复。

**适用场景：**当前请求的核心问题是“设计 USaveGame、槽位、异步保存、二进制归档、版本迁移及恢复”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`USaveGame`、`AsyncSaveGameToSlot`、`FArchive`、`存档`、`槽位`。

**不触发与边界：**

- SaveGame 属性标记不自动保存整个 Actor；持久 ID 与临时对象引用要区分。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-data-assets-tables](#skill-ue-data-assets-tables)：恢复流程需要加载资源时，明确异步和驻留契约。
- [ue-networking-replication](#skill-ue-networking-replication)：多人存档恢复影响服务器事实时，核查权威入口。
- [ue-save-and-load](#skill-ue-save-and-load)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“增加带版本号的存档并防止迟到读取覆盖当前游戏。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-serialization-savegames 完成：增加带版本号的存档并防止迟到读取覆盖当前游戏。”
- 不触发示例：“把编辑器当前蓝图保存到磁盘。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-serialization-savegames/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/save.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-world-level-streaming"></a>

#### ue-world-level-streaming

**功能：**处理 WorldPartition、DataLayer、LevelInstance、流送就绪与跨关卡状态。

**适用场景：**当前请求的核心问题是“处理 WorldPartition、DataLayer、LevelInstance、流送就绪与跨关卡状态”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`WorldPartition`、`DataLayer`、`LevelStreaming`、`HLOD`、`关卡流送`。

**不触发与边界：**

- 加载/可见/Actor 初始化/玩法就绪是不同阶段；Cook 与编辑器加载分别验证。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-serialization-savegames](#skill-ue-serialization-savegames)：跨关卡持久状态需要保存/恢复时，核查稳定 ID 与版本。
- [ue-data-assets-tables](#skill-ue-data-assets-tables)：加载/包内资源可达性是问题时，核查软引用与扫描规则。
- [ue-levels-and-world-partition](#skill-ue-levels-and-world-partition)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“关卡卸载后异步回调还修改旧世界，检查清理与请求身份。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-world-level-streaming 完成：关卡卸载后异步回调还修改旧世界，检查清理与请求身份。”
- 不触发示例：“给网站分页组件增加懒加载。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-world-level-streaming/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/world.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-asset-management"></a>

#### ue-asset-management

**功能：**深入处理 AssetRegistry、StreamableManager、PrimaryAsset 与硬软引用。

**适用场景：**当前请求的核心问题是“深入处理 AssetRegistry、StreamableManager、PrimaryAsset 与硬软引用”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`AssetRegistry`、`FStreamableHandle`、`PrimaryAssetId`、`资源驻留`。

**不触发与边界：**

- 数据结构选择优先 data-assets；资源定位、异步加载与持有是本条重点。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：资源加载后持有与回收是问题时，补查 GC。
- [ue-packaging-and-deployment](#skill-ue-packaging-and-deployment)：问题在 Cook/包内可达性时，核查打包规则；不自动执行发布。

**调用方式与正反例：**

- 自动选择示例：“查软引用资源加载成功后为什么仍被回收，以及句柄何时释放。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-asset-management 完成：查软引用资源加载成功后为什么仍被回收，以及句柄何时释放。”
- 不触发示例：“给 GitHub PR 附加附件。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-asset-management/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/assets.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-data-driven-design"></a>

#### ue-data-driven-design

**功能：**按记录/结构/曲线/配置需求选择 DataTable、DataAsset、Curve 和 DeveloperSettings。

**适用场景：**当前请求的核心问题是“按记录/结构/曲线/配置需求选择 DataTable、DataAsset、Curve 和 DeveloperSettings”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`数据驱动设计`、`CurveTable`、`DeveloperSettings`、`可调参数`。

**不触发与边界：**

- 选择容器后仍需实际字段和加载契约；不为本任务另造配置框架。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-memory-and-gc](#skill-ue-memory-and-gc)：资源加载后持有与回收是问题时，补查 GC。
- [ue-packaging-and-deployment](#skill-ue-packaging-and-deployment)：问题在 Cook/包内可达性时，核查打包规则；不自动执行发布。

**调用方式与正反例：**

- 自动选择示例：“决定角色成长参数该放 DataAsset 还是 CurveTable，并沿用现有配置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-data-driven-design 完成：决定角色成长参数该放 DataAsset 还是 CurveTable，并沿用现有配置。”
- 不触发示例：“在 Notion 创建内容计划库。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-data-driven-design/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/assets.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-importing-content"></a>

#### ue-importing-content

**功能：**核实 FBX/Interchange 导入、重导入、单位、路径、骨架和覆盖范围。

**适用场景：**当前请求的核心问题是“核实 FBX/Interchange 导入、重导入、单位、路径、骨架和覆盖范围”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`FBX`、`Interchange`、`AssetImportTask`、`重导入`。

**不触发与边界：**

- 导入成功不等于骨架/材质/引用正确；不批量覆盖其他资产。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-meshes-static-and-skeletal](#skill-ue-meshes-static-and-skeletal)：导入网格后核对骨架、LOD、Socket 和材质槽。
- [ue-data-assets-tables](#skill-ue-data-assets-tables)：导入产物需要运行时引用/加载时，核查资产路径。

**调用方式与正反例：**

- 自动选择示例：“重导入这把武器网格，保留原材质槽与引用。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-importing-content 完成：重导入这把武器网格，保留原材质槽与引用。”
- 不触发示例：“将 Google Docs 导入到 Word。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-importing-content/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/import.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-levels-and-world-partition"></a>

#### ue-levels-and-world-partition

**功能：**复核关卡、WorldPartition、DataLayer、HLOD、流送源和卸载边界。

**适用场景：**当前请求的核心问题是“复核关卡、WorldPartition、DataLayer、HLOD、流送源和卸载边界”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`流送源`、`WorldPartition接口`、`DataLayer状态`、`子关卡`。

**不触发与边界：**

- 与 ue-world-level-streaming 同职责时择一，细查本机 API 才用本条补充。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-world-level-streaming`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-serialization-savegames](#skill-ue-serialization-savegames)：跨关卡持久状态需要保存/恢复时，核查稳定 ID 与版本。
- [ue-data-assets-tables](#skill-ue-data-assets-tables)：加载/包内资源可达性是问题时，核查软引用与扫描规则。
- [ue-world-level-streaming](#skill-ue-world-level-streaming)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“检查 DataLayer 激活与 Actor 实际就绪为何不同步。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-levels-and-world-partition 完成：检查 DataLayer 激活与 Actor 实际就绪为何不同步。”
- 不触发示例：“拆分一个大 Markdown 文档。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-levels-and-world-partition/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/world.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-save-and-load"></a>

#### ue-save-and-load

**功能：**复核 SaveGame、归档标志、异步槽位和稳定 ID 的序列化细节。

**适用场景：**当前请求的核心问题是“复核 SaveGame、归档标志、异步槽位和稳定 ID 的序列化细节”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`存档归档`、`SaveGame标志`、`异步存档回调`、`稳定ID`。

**不触发与边界：**

- 主存档实现默认 ue-serialization-savegames，细查存储/归档契约再用本条。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-serialization-savegames`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-data-assets-tables](#skill-ue-data-assets-tables)：恢复流程需要加载资源时，明确异步和驻留契约。
- [ue-networking-replication](#skill-ue-networking-replication)：多人存档恢复影响服务器事实时，核查权威入口。
- [ue-serialization-savegames](#skill-ue-serialization-savegames)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实为什么 UPROPERTY(SaveGame) 没有自动保存 Actor 状态。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-save-and-load 完成：核实为什么 UPROPERTY(SaveGame) 没有自动保存 Actor 状态。”
- 不触发示例：“将 Notion 会议笔记保存。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-save-and-load/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/save.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-11"></a>

### 5.11 UE 蓝图 编辑器与 UI

<a id="skill-ue-editor-tools"></a>

#### ue-editor-tools

**功能：**在现有 Editor 模块中扩展细节面板、资产工具、菜单及编辑器子系统。

**适用场景：**当前请求的核心问题是“在现有 Editor 模块中扩展细节面板、资产工具、菜单及编辑器子系统”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Editor Module`、`DetailCustomization`、`UToolMenus`、`编辑器工具`。

**不触发与边界：**

- Runtime 不无条件依赖 UnrealEd；优先现有 AuthoringLibrary，不为了工具另拆业务模块。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [create-toolset](#skill-create-toolset)：需要将现有编辑接口暴露为实际桥接能力时，设计对应工具契约。
- [ue-module-build-system](#skill-ue-module-build-system)：添加 C++ 编辑器工具时，核查 Editor 模块依赖和构建。

**调用方式与正反例：**

- 自动选择示例：“在 HodgeAbilityEditor 中加一个数据资产校验工具。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-editor-tools 完成：在 HodgeAbilityEditor 中加一个数据资产校验工具。”
- 不触发示例：“调整 IDE 字体大小。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-editor-tools/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/editor.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-ui-umg-slate"></a>

#### ue-ui-umg-slate

**功能：**实现 UMG/Slate/CommonUI 层、焦点、输入、属性事件和控件生命周期。

**适用场景：**当前请求的核心问题是“实现 UMG/Slate/CommonUI 层、焦点、输入、属性事件和控件生命周期”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UMG`、`Slate`、`CommonUI`、`WBP_`、`HUD`、`控件`。

**不触发与边界：**

- 项目已有 UI 依赖不等于全部资产接好；不要用 Widget Tick 轮询替代事件。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：控件消费 ASC 属性/标签事件时，核查绑定来源及重生恢复。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：Construct/Destruct 导致重复回调时，检查委托配对。
- [unreal-mcp](#skill-unreal-mcp)：需要操作 WBP 资产时，核实控件树/图表工具能力。
- [ue-umg-and-slate](#skill-ue-umg-and-slate)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“做角色生命条，按 ASC 事件更新并处理重生解绑。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-ui-umg-slate 完成：做角色生命条，按 ASC 事件更新并处理重生解绑。”
- 不触发示例：“制作一个项目宣传网站。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-ui-umg-slate/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/ui.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-blueprint-fundamentals"></a>

#### ue-blueprint-fundamentals

**功能：**解释并检查蓝图类、生成类/CDO、图类型、变量、组件和 Construction Script。

**适用场景：**当前请求的核心问题是“解释并检查蓝图类、生成类/CDO、图类型、变量、组件和 Construction Script”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`蓝图CDO`、`ConstructionScript`、`EventGraph`、`蓝图父类`。

**不触发与边界：**

- 读图能力不等于所有图可写；需要实际操作时再叠加 unreal-mcp。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-blueprint-cpp-interop](#skill-ue-blueprint-cpp-interop)：图错误源自 C++ 暴露接口时，核查反射声明。
- [unreal-mcp](#skill-unreal-mcp)：确需在编辑器读取/编辑蓝图时，先核实图类型与 schema。

**调用方式与正反例：**

- 自动选择示例：“蓝图默认值与实例值不同，帮我查 CDO 和构造脚本。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-blueprint-fundamentals 完成：蓝图默认值与实例值不同，帮我查 CDO 和构造脚本。”
- 不触发示例：“给服务器写部署蓝图计划。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-blueprint-fundamentals/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/blueprint.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-editor-scripting-and-python"></a>

#### ue-editor-scripting-and-python

**功能：**用 UE Python 与 Editor 子系统批量检查/编辑明确的资产，保持事务和保存范围。

**适用场景：**当前请求的核心问题是“用 UE Python 与 Editor 子系统批量检查/编辑明确的资产，保持事务和保存范围”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`unreal Python`、`EditorAssetSubsystem`、`EditorUtility`、`编辑器脚本`。

**不触发与边界：**

- 普通系统 Python 没有 unreal 编辑器对象；不可默认开启 Remote Execution。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [create-toolset](#skill-create-toolset)：需要将现有编辑接口暴露为实际桥接能力时，设计对应工具契约。
- [ue-module-build-system](#skill-ue-module-build-system)：添加 C++ 编辑器工具时，核查 Editor 模块依赖和构建。

**调用方式与正反例：**

- 自动选择示例：“用已有 UE Python 接口核查这一组资产的引用，不保存其他资产。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-editor-scripting-and-python 完成：用已有 UE Python 接口核查这一组资产的引用，不保存其他资产。”
- 不触发示例：“用系统 Python 解析一个 CSV。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-editor-scripting-and-python/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/editor.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-umg-and-slate"></a>

#### ue-umg-and-slate

**功能：**复核 UUserWidget/Slate 生命周期、BindWidget、焦点、事件和性能。

**适用场景：**当前请求的核心问题是“复核 UUserWidget/Slate 生命周期、BindWidget、焦点、事件和性能”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`BindWidget`、`NativeDestruct`、`Slate控件`、`Widget生命周期`。

**不触发与边界：**

- 通用 UI 实现默认 ue-ui-umg-slate；仅特定控件底层问题补充此条。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-ui-umg-slate`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：控件消费 ASC 属性/标签事件时，核查绑定来源及重生恢复。
- [ue-delegates-and-events](#skill-ue-delegates-and-events)：Construct/Destruct 导致重复回调时，检查委托配对。
- [unreal-mcp](#skill-unreal-mcp)：需要操作 WBP 资产时，核实控件树/图表工具能力。
- [ue-ui-umg-slate](#skill-ue-ui-umg-slate)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“反复打开 Widget 后委托重复，核实 Construct/Destruct。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-umg-and-slate 完成：反复打开 Widget 后委托重复，核实 Construct/Destruct。”
- 不触发示例：“制作一个网站登录页。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-umg-and-slate/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/ui.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-create-toolset"></a>

#### create-toolset

**功能：**为 UE5.5 项目扩展可组合的编辑器/MCP 工具，复用 AuthoringLibrary 与现有网关。

**适用场景：**当前请求的核心问题是“为 UE5.5 项目扩展可组合的编辑器/MCP 工具，复用 AuthoringLibrary 与现有网关”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`扩展UE工具`、`MCP工具接口`、`HodgeAbilityEditor`、`toolset`。

**不触发与边界：**

- 不使用缺失的 5.8 ToolsetRegistry/ToolsetDefinition，第三方网关改动需明确任务关联。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 本地 UE5.5 工作流适配；实时执行依赖当前编辑器/桥接，不提供缺失的 5.8 官方引擎设施。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-editor-tools](#skill-ue-editor-tools)：实现项目编辑器函数时使用已有 Editor 模块。
- [ue-module-build-system](#skill-ue-module-build-system)：新增/修改 C++ 接口或依赖后执行常规验证。

**调用方式与正反例：**

- 自动选择示例：“把现有编辑器校验函数通过当前桥接暴露，先检查已有 schema。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 create-toolset 完成：把现有编辑器校验函数通过当前桥接暴露，先检查已有 schema。”
- 不触发示例：“仅调用一个现成的读取资产工具。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/create-toolset/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/epic-toolset.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-unreal-mcp"></a>

#### unreal-mcp

**功能：**通过真实连接核对工程身份、发现 schema 并查询/操作 UE 编辑器。

**适用场景：**当前请求的核心问题是“通过真实连接核对工程身份、发现 schema 并查询/操作 UE 编辑器”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UE编辑器实时操作`、`资产查询`、`hodge_unreal`、`bridge_status`。

**不触发与边界：**

- 现有 search→describe→execute 不等于官方 5.8 meta-tool 协议；连通与写入范围需逐次核实。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 本地 UE5.5 工作流适配；实时执行依赖当前编辑器/桥接，不提供缺失的 5.8 官方引擎设施。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-blueprint-fundamentals](#skill-ue-blueprint-fundamentals)：实际请求针对蓝图结构/图类型时，先明确定义。
- [ue-ui-umg-slate](#skill-ue-ui-umg-slate)：实际请求针对 Widget/控件树时，使用其业务与生命周期规则。

**调用方式与正反例：**

- 自动选择示例：“读取当前 UE 角色蓝图默认值并核对工程身份。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 unreal-mcp 完成：读取当前 UE 角色蓝图默认值并核对工程身份。”
- 不触发示例：“只解释 GAS 的基本原理。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/unreal-mcp/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/epic-mcp.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-unreal-skill"></a>

#### unreal-skill

**功能：**在 UE5.5 仓库维护标准文件型 Agent Skill 工作流，替代缺失的引擎 AgentSkill 设施。

**适用场景：**当前请求的核心问题是“在 UE5.5 仓库维护标准文件型 Agent Skill 工作流，替代缺失的引擎 AgentSkill 设施”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UE项目工作流skill`、`编辑器流程SKILL.md`、`文件型AgentSkill`。

**不触发与边界：**

- 本机没有 UAgentSkill/AgentSkillToolset；通用创建过程由 skill-creator 主导，本条只补 UE 路径适配。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 本地 UE5.5 工作流适配；实时执行依赖当前编辑器/桥接，不提供缺失的 5.8 官方引擎设施。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [skill-creator](#skill-skill-creator)：通用 Agent Skill 编写由该入口主导，本条只补 UE5.5 的路径和能力边界。
- [create-rule](#skill-create-rule)：只有需要长期约束自动生效时增加规则入口。

**调用方式与正反例：**

- 自动选择示例：“为当前 UE 编辑器资产校验流程写一个文件型 SKILL.md。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 unreal-skill 完成：为当前 UE 编辑器资产校验流程写一个文件型 SKILL.md。”
- 不触发示例：“创建一个 UAgentSkill 蓝图资产。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/unreal-skill/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/epic-skill.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-12"></a>

### 5.12 UE AI Mass 与 StateTree

<a id="skill-ue-ai-navigation"></a>

#### ue-ai-navigation

**功能：**实现 AIController、行为树、感知、EQS 和寻路任务，并与现有战斗执行入口对接。

**适用场景：**当前请求的核心问题是“实现 AIController、行为树、感知、EQS 和寻路任务，并与现有战斗执行入口对接”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`AIController`、`Behavior Tree`、`Blackboard`、`EQS`、`NavMesh`、`怪物AI`。

**不触发与边界：**

- AI 决策与服务器执行分开；不让 AI 伪造本地玩家输入，不为普通怪物增加 PlayerState。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-character-movement](#skill-ue-character-movement)：任务需要移动、取消或 RootMotion 时，核查执行者。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：AI 请求使用现有战斗能力时，核查统一请求与权限。
- [ue-state-trees](#skill-ue-state-trees)：明确选择 StateTree 行为实现时，核查其任务签名。
- [ue-ai-and-navigation](#skill-ue-ai-and-navigation)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“怪物看见玩家后寻路追击，丢失目标时取消移动。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-ai-navigation 完成：怪物看见玩家后寻路追击，丢失目标时取消移动。”
- 不触发示例：“创建一个审核代码的 Agent。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-ai-navigation/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/ai.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-mass-entity"></a>

#### ue-mass-entity

**功能：**使用本机 Mass Fragment、Query、Processor 和 Deferred 命令处理 ECS 工作流。

**适用场景：**当前请求的核心问题是“使用本机 Mass Fragment、Query、Processor 和 Deferred 命令处理 ECS 工作流”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MassEntity`、`FMassEntityQuery`、`Fragment`、`Processor`、`ECS`。

**不触发与边界：**

- 不把现有 Actor/GAS 框架自动迁到 Mass；本机模块位置与 API 由 5.5 源码核实。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：5.8 示例或 Query/Processor 签名不确定时，以本机源码核实。

**调用方式与正反例：**

- 自动选择示例：“核实本机 Mass Query 的读写权限与 Processor 注册签名。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-mass-entity 完成：核实本机 Mass Query 的读写权限与 Processor 注册签名。”
- 不触发示例：“让 Code Review agent 并行审查三个文件。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-mass-entity/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/mass.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-state-trees"></a>

#### ue-state-trees

**功能：**使用本机 StateTree 的任务、条件、评估器、事件与进出状态契约。

**适用场景：**当前请求的核心问题是“使用本机 StateTree 的任务、条件、评估器、事件与进出状态契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`StateTree`、`EnterState`、`ExitState`、`InstanceData`、`状态树`。

**不触发与边界：**

- 行为状态与 Init State 分工不同；5.8 Scheduled Tick/Considerations 需核实本机是否存在。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-ai-navigation](#skill-ue-ai-navigation)：StateTree 驱动 AI 感知/移动时，明确决策与执行边界。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：任务提交 GAS 动作时，取消时撤销同一执行请求。

**调用方式与正反例：**

- 自动选择示例：“StateTree 任务 Abort 后撤销移动请求，核实 5.5 签名。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-state-trees 完成：StateTree 任务 Abort 后撤销移动请求，核实 5.5 签名。”
- 不触发示例：“把 Pawn 的初始化阶段倒退来表达眩晕。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-state-trees/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/statetree.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-ai-and-navigation"></a>

#### ue-ai-and-navigation

**功能：**复核 AIController、路径跟随、行为树、感知与 EQS 的具体框架关系。

**适用场景：**当前请求的核心问题是“复核 AIController、路径跟随、行为树、感知与 EQS 的具体框架关系”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`路径跟随`、`感知`、`BTTask`、`EQS`、`AIController`。

**不触发与边界：**

- 与主 AI skill 重合时选择其一；本条适合接口/框架关系复核。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-ai-navigation`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-character-movement](#skill-ue-character-movement)：任务需要移动、取消或 RootMotion 时，核查执行者。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：AI 请求使用现有战斗能力时，核查统一请求与权限。
- [ue-state-trees](#skill-ue-state-trees)：明确选择 StateTree 行为实现时，核查其任务签名。
- [ue-ai-navigation](#skill-ue-ai-navigation)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“检查行为树任务结束后 AIController 的移动取消是否完整。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-ai-and-navigation 完成：检查行为树任务结束后 AIController 的移动取消是否完整。”
- 不触发示例：“创建一个 Codex 调试 agent 类型。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-ai-and-navigation/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/ai.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-13"></a>

### 5.13 UE 渲染 特效 音频与几何

<a id="skill-ue-audio-system"></a>

#### ue-audio-system

**功能：**控制 AudioComponent、MetaSound、衰减、并发、混音和事件驱动音效。

**适用场景：**当前请求的核心问题是“控制 AudioComponent、MetaSound、衰减、并发、混音和事件驱动音效”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`AudioComponent`、`MetaSound`、`Quartz`、`音效`、`空间音频`。

**不触发与边界：**

- 普通音频播放可用原生显示；只有 UE 音频资源和运行逻辑才进入本条。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：持续音效由技能占用时，核查取消/结束清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：音频由时间线/过场驱动时，核查播放与中断。
- [ue-audio-and-metasounds](#skill-ue-audio-and-metasounds)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“角色冲刺时播放可停止的附着音效，取消时释放。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-audio-system 完成：角色冲刺时播放可停止的附着音效，取消时释放。”
- 不触发示例：“播放我上传的 mp3。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-audio-system/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/audio.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-materials-rendering"></a>

#### ue-materials-rendering

**功能：**处理 MID、参数集合、渲染目标、后处理、材质与平台渲染设置。

**适用场景：**当前请求的核心问题是“处理 MID、参数集合、渲染目标、后处理、材质与平台渲染设置”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MID`、`MaterialParameterCollection`、`RenderTarget`、`后处理材质`。

**不触发与边界：**

- 运行时参数与编辑器材质图编辑分开；不自动切换 Substrate/RHI。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [imagegen](#skill-imagegen)：确需新的纹理位图时才生成图像；材质图仍由 UE 工作流修改。
- [unreal-mcp](#skill-unreal-mcp)：需要实际编辑材质资源时，核实图表接口，不假定通用蓝图桥支持。
- [ue-materials-and-shaders](#skill-ue-materials-and-shaders)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“技能命中时修改现有 MID 参数，取消时恢复占用。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-materials-rendering 完成：技能命中时修改现有 MID 参数，取消时恢复占用。”
- 不触发示例：“修改 PNG 图像的背景颜色。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-materials-rendering/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/materials.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-effects"></a>

#### ue-niagara-effects

**功能：**生成/控制 Niagara System 与组件、User 参数、池化、结束回调及资源清理。

**适用场景：**当前请求的核心问题是“生成/控制 Niagara System 与组件、User 参数、池化、结束回调及资源清理”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Niagara`、`NS_`、`Emitter`、`User参数`、`特效池`。

**不触发与边界：**

- 视觉特效不负责服务器伤害事实；DataChannel/LightweightEmitter 按 5.5 实际能力核查。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。
- [ue-niagara-vfx](#skill-ue-niagara-vfx)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“技能取消时停止附着 Niagara 并避免池化组件被旧回调清理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-effects 完成：技能取消时停止附着 Niagara 并避免池化组件被旧回调清理。”
- 不触发示例：“把一张静态技能图标改成蓝色。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-effects/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-physics-collision"></a>

#### ue-physics-collision

**功能：**实现 Trace/Sweep/Overlap、碰撞通道、命中结果、刚体与 Chaos 交互。

**适用场景：**当前请求的核心问题是“实现 Trace/Sweep/Overlap、碰撞通道、命中结果、刚体与 Chaos 交互”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Sweep`、`Overlap`、`Trace`、`FHitResult`、`Collision`、`Chaos`。

**不触发与边界：**

- 服务端伤害判定与客户端特效分离；不通过直接位移绕过运动复制。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：查询结果进入伤害/能力 TargetData 时，验证权限与动作身份。
- [ue-networking-replication](#skill-ue-networking-replication)：物理结果需要多端表现时，明确权威事实与表现复制。
- [ue-physics-and-chaos](#skill-ue-physics-and-chaos)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“近战一次挥击重复命中同一目标，检查碰撞查询和去重范围。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-physics-collision 完成：近战一次挥击重复命中同一目标，检查碰撞查询和去重范围。”
- 不触发示例：“解释网页按钮的 hover 事件。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-physics-collision/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/physics.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-procedural-generation"></a>

#### ue-procedural-generation

**功能：**使用本机 PCG、程序化网格、实例化和几何工具生成内容。

**适用场景：**当前请求的核心问题是“使用本机 PCG、程序化网格、实例化和几何工具生成内容”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`PCG`、`ProceduralMesh`、`GeometryScript`、`ISM`、`HISM`。

**不触发与边界：**

- 插件启用状态先检查；5.8 PointArrayData/GPU 功能不作为 5.5 前提。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-world-level-streaming](#skill-ue-world-level-streaming)：生成发生在 WorldPartition/流送世界时，核查世界与取消身份。
- [ue-meshes-static-and-skeletal](#skill-ue-meshes-static-and-skeletal)：输出包含网格/实例时，核对资源和组件生命周期。

**调用方式与正反例：**

- 自动选择示例：“为已有 PCG CPU 节点核实 5.5 的输入、种子和输出契约。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-procedural-generation 完成：为已有 PCG CPU 节点核实 5.5 的输入、种子和输出契约。”
- 不触发示例：“用图片模型生成一张地形贴图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-procedural-generation/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/pcg.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-sequencer-cinematics"></a>

#### ue-sequencer-cinematics

**功能：**控制 LevelSequence 播放、绑定、事件轨、相机及电影渲染队列。

**适用场景：**当前请求的核心问题是“控制 LevelSequence 播放、绑定、事件轨、相机及电影渲染队列”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`LevelSequence`、`Sequencer`、`MovieScene`、`MRQ`、`过场`。

**不触发与边界：**

- 序列生命周期与技能动作身份配对；启动 MRQ 不表示视频产物已验证。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-gameplay-cameras](#skill-ue-gameplay-cameras)：序列接管角色视图时，核查相机栈恢复。
- [ue-niagara-effects](#skill-ue-niagara-effects)：序列控制特效激活/参数时，核查资源生命周期。
- [ue-sequencer-and-cinematics](#skill-ue-sequencer-and-cinematics)：同职责复核候选：本条默认承担一般实现；只有具体底层核查需要或用户指定另一来源时择一/分阶段使用。

**调用方式与正反例：**

- 自动选择示例：“过场序列中断后释放相机和输入占用。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-sequencer-cinematics 完成：过场序列中断后释放相机和输入占用。”
- 不触发示例：“为 PPT 增加入场动画。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-sequencer-cinematics/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/sequencer.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-audio-and-metasounds"></a>

#### ue-audio-and-metasounds

**功能：**复核音频组件、MetaSound 参数、Quartz 与混音的接口和生命周期。

**适用场景：**当前请求的核心问题是“复核音频组件、MetaSound 参数、Quartz 与混音的接口和生命周期”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MetaSound参数`、`Quartz时钟`、`AudioComponent接口`。

**不触发与边界：**

- 常规音频实现主用 ue-audio-system；仅在专项接口复核时补充。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-audio-system`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：持续音效由技能占用时，核查取消/结束清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：音频由时间线/过场驱动时，核查播放与中断。
- [ue-audio-system](#skill-ue-audio-system)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实 MetaSound 参数的实际名称、类型与 AudioComponent API。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-audio-and-metasounds 完成：核实 MetaSound 参数的实际名称、类型与 AudioComponent API。”
- 不触发示例：“生成一张声音波形示意图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-audio-and-metasounds/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/audio.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-auto-landscape-materials"></a>

#### ue-auto-landscape-materials

**功能：**制作和检查基于坡度、高度、纹理混合及重复抑制的自动地形材质。

**适用场景：**当前请求的核心问题是“制作和检查基于坡度、高度、纹理混合及重复抑制的自动地形材质”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`自动地形材质`、`坡度混合`、`高度混合`、`平铺纹理`。

**不触发与边界：**

- 材质参数方案不等同 PCG 几何生成；不要覆盖无关第三方材质。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-materials-rendering](#skill-ue-materials-rendering)：地形问题实际来自材质图或参数时，分离处理。
- [ue-world-level-streaming](#skill-ue-world-level-streaming)：地形和植被跨 WorldPartition 流送时，核查加载与 Cook。

**调用方式与正反例：**

- 自动选择示例：“在已有地形材质中调整坡度混合，保持实例参数可调。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-auto-landscape-materials 完成：在已有地形材质中调整坡度混合，保持实例参数可调。”
- 不触发示例：“用 PCG 放置沿路的树木。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-auto-landscape-materials/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/landscape.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-landscape-and-foliage"></a>

#### ue-landscape-and-foliage

**功能：**处理 Landscape 尺寸、图层、雕刻、植被实例、碰撞与世界分区关系。

**适用场景：**当前请求的核心问题是“处理 Landscape 尺寸、图层、雕刻、植被实例、碰撞与世界分区关系”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Landscape`、`Foliage`、`地形图层`、`植被实例`。

**不触发与边界：**

- 空间内容编辑与材质、RVT 分别核查；写入前明确目标与恢复点。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-materials-rendering](#skill-ue-materials-rendering)：地形问题实际来自材质图或参数时，分离处理。
- [ue-world-level-streaming](#skill-ue-world-level-streaming)：地形和植被跨 WorldPartition 流送时，核查加载与 Cook。

**调用方式与正反例：**

- 自动选择示例：“核查地形 Edit Layers 与植被碰撞，不改无关关卡。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-landscape-and-foliage 完成：核查地形 Edit Layers 与植被碰撞，不改无关关卡。”
- 不触发示例：“制作一张自然风景照片。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-landscape-and-foliage/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/landscape.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-landscape-rvt-and-grass"></a>

#### ue-landscape-rvt-and-grass

**功能：**配置和排查 Landscape RVT、地表融合与 Landscape Grass 输出。

**适用场景：**当前请求的核心问题是“配置和排查 Landscape RVT、地表融合与 Landscape Grass 输出”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`RVT`、`LandscapeGrass`、`地表融合`、`虚拟纹理`。

**不触发与边界：**

- 原生 RVT/Grass 不代表 UDS/UDW 已安装；按目标平台验证表现与成本。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-materials-rendering](#skill-ue-materials-rendering)：地形问题实际来自材质图或参数时，分离处理。
- [ue-world-level-streaming](#skill-ue-world-level-streaming)：地形和植被跨 WorldPartition 流送时，核查加载与 Cook。

**调用方式与正反例：**

- 自动选择示例：“地形草和石头接缝不一致，检查 RVT 与 Grass 配置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-landscape-rvt-and-grass 完成：地形草和石头接缝不一致，检查 RVT 与 Grass 配置。”
- 不触发示例：“用普通 Niagara 发射草叶粒子。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-landscape-rvt-and-grass/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/landscape.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-lighting-and-lumen"></a>

#### ue-lighting-and-lumen

**功能：**核实光源、曝光、Lumen、阴影、雾和当前 RHI/平台支持。

**适用场景：**当前请求的核心问题是“核实光源、曝光、Lumen、阴影、雾和当前 RHI/平台支持”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Lumen`、`曝光`、`光照`、`阴影`、`SkyAtmosphere`。

**不触发与边界：**

- 不为照搬 5.8 方案自动切换渲染管线或声称实验功能已成熟可用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-profiling-and-optimization](#skill-ue-profiling-and-optimization)：有性能目标时采集目标场景的 CPU/GPU 数据。
- [ue-materials-rendering](#skill-ue-materials-rendering)：问题归于具体材质/参数/后处理资源时，转入更窄对象。

**调用方式与正反例：**

- 自动选择示例：“室内曝光和 Lumen 反射异常，先核对项目设置与场景。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-lighting-and-lumen 完成：室内曝光和 Lumen 反射异常，先核对项目设置与场景。”
- 不触发示例：“把已有照片整体提亮。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-lighting-and-lumen/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/rendering.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-master-material-authoring"></a>

#### ue-master-material-authoring

**功能：**构建可复用主材质、实例、开关、打包贴图通道和参数契约。

**适用场景：**当前请求的核心问题是“构建可复用主材质、实例、开关、打包贴图通道和参数契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`主材质`、`MaterialInstance`、`StaticSwitch`、`ORM`、`PBR`。

**不触发与边界：**

- 位图生成可叠加 imagegen，但主材质图和资源引用仍使用实际 UE 工具/API。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [imagegen](#skill-imagegen)：确需新的纹理位图时才生成图像；材质图仍由 UE 工作流修改。
- [unreal-mcp](#skill-unreal-mcp)：需要实际编辑材质资源时，核实图表接口，不假定通用蓝图桥支持。

**调用方式与正反例：**

- 自动选择示例：“整理现有主材质的可调参数与打包纹理通道，保留实例引用。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-master-material-authoring 完成：整理现有主材质的可调参数与打包纹理通道，保留实例引用。”
- 不触发示例：“生成一张新的金属贴图。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-master-material-authoring/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/materials.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-material-graph-effects"></a>

#### ue-material-graph-effects

**功能：**实现材质图中的 UV 动画、顶点形变、溶解、发光和交互参数。

**适用场景：**当前请求的核心问题是“实现材质图中的 UV 动画、顶点形变、溶解、发光和交互参数”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`材质溶解`、`UV流动`、`WPO`、`顶点动画`、`发光材质`。

**不触发与边界：**

- 需要实际材质图能力；不要把普通蓝图节点工具当作材质图编辑器。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [imagegen](#skill-imagegen)：确需新的纹理位图时才生成图像；材质图仍由 UE 工作流修改。
- [unreal-mcp](#skill-unreal-mcp)：需要实际编辑材质资源时，核实图表接口，不假定通用蓝图桥支持。

**调用方式与正反例：**

- 自动选择示例：“用现有材质做角色受击溶解并暴露可调阈值。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-material-graph-effects 完成：用现有材质做角色受击溶解并暴露可调阈值。”
- 不触发示例：“用图片模型给人物换背景。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-material-graph-effects/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/materials.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-materials-and-shaders"></a>

#### ue-materials-and-shaders

**功能：**复核材质/Shader、MID、参数集合、后处理和渲染接口。

**适用场景：**当前请求的核心问题是“复核材质/Shader、MID、参数集合、后处理和渲染接口”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Shader`、`UMaterial`、`MID接口`、`后处理接口`。

**不触发与边界：**

- 与主 materials-rendering 重合时选择其一；Runtime 不能依赖 MaterialEditingLibrary。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-materials-rendering`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [imagegen](#skill-imagegen)：确需新的纹理位图时才生成图像；材质图仍由 UE 工作流修改。
- [unreal-mcp](#skill-unreal-mcp)：需要实际编辑材质资源时，核实图表接口，不假定通用蓝图桥支持。
- [ue-materials-rendering](#skill-ue-materials-rendering)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实动态材质参数修改和自定义 Shader 的模块边界。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-materials-and-shaders 完成：核实动态材质参数修改和自定义 Shader 的模块边界。”
- 不触发示例：“修改网页 CSS 渐变。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-materials-and-shaders/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/materials.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-nanite-and-rendering"></a>

#### ue-nanite-and-rendering

**功能：**检查 Nanite、渲染特性、后处理、平台/材质限制和性能代价。

**适用场景：**当前请求的核心问题是“检查 Nanite、渲染特性、后处理、平台/材质限制和性能代价”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Nanite`、`GPU渲染`、`VSM`、`渲染特性`、`RHI`。

**不触发与边界：**

- 视觉开关要按实际版本/硬件验证；不擅自提高画质或切换 RHI。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-profiling-and-optimization](#skill-ue-profiling-and-optimization)：有性能目标时采集目标场景的 CPU/GPU 数据。
- [ue-materials-rendering](#skill-ue-materials-rendering)：问题归于具体材质/参数/后处理资源时，转入更窄对象。

**调用方式与正反例：**

- 自动选择示例：“这个网格启用 Nanite 后异常，核实材质、平台与渲染设置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-nanite-and-rendering 完成：这个网格启用 Nanite 后异常，核实材质、平台与渲染设置。”
- 不触发示例：“用 SVG 画一个三角形。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-nanite-and-rendering/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/rendering.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-custom-modules"></a>

#### ue-niagara-custom-modules

**功能：**创建或排查 Niagara Module Script、输入、执行阶段与可复用函数。

**适用场景：**当前请求的核心问题是“创建或排查 Niagara Module Script、输入、执行阶段与可复用函数”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Niagara Module Script`、`自定义特效模块`、`Scratch Pad`。

**不触发与边界：**

- Niagara 脚本模块不同于 UBT 模块；必须核实实际图表编辑能力。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。

**调用方式与正反例：**

- 自动选择示例：“为 Niagara 加一个可复用速度模块，明确粒子更新阶段。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-custom-modules 完成：为 Niagara 加一个可复用速度模块，明确粒子更新阶段。”
- 不触发示例：“给 UE 新建一个 Runtime C++ 模块。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-custom-modules/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-flamethrower"></a>

#### ue-niagara-flamethrower

**功能：**组织喷火视觉的发射器、流动、碰撞表现、光照和生命周期。

**适用场景：**当前请求的核心问题是“组织喷火视觉的发射器、流动、碰撞表现、光照和生命周期”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`喷火特效`、`flamethrower`、`火焰发射器`。

**不触发与边界：**

- 伤害范围/判定由现有战斗/GAS负责，Niagara 只消费视觉参数。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。

**调用方式与正反例：**

- 自动选择示例：“做持续喷火的 Niagara 视觉，松开输入时停止并释放组件。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-flamethrower 完成：做持续喷火的 Niagara 视觉，松开输入时停止并释放组件。”
- 不触发示例：“设计火焰技能的服务器伤害公式，不涉及表现。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-flamethrower/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-mesh-disintegration"></a>

#### ue-niagara-mesh-disintegration

**功能：**制作基于网格采样、材质溶解和粒子的消散特效。

**适用场景：**当前请求的核心问题是“制作基于网格采样、材质溶解和粒子的消散特效”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`mesh disintegration`、`网格消散`、`粒子溶解`。

**不触发与边界：**

- 死亡事实与表现完成分开，取消/卸载时清理对应组件，不销毁无关资源。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。

**调用方式与正反例：**

- 自动选择示例：“角色死亡时同步材质溶解和 Niagara 网格粒子。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-mesh-disintegration 完成：角色死亡时同步材质溶解和 Niagara 网格粒子。”
- 不触发示例：“删除一份网格资产。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-mesh-disintegration/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-renderers-and-materials"></a>

#### ue-niagara-renderers-and-materials

**功能：**配置 Sprite/Mesh/Ribbon 渲染器、材质与参数绑定。

**适用场景：**当前请求的核心问题是“配置 Sprite/Mesh/Ribbon 渲染器、材质与参数绑定”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Niagara Renderer`、`Ribbon`、`粒子材质`、`Sprite Renderer`。

**不触发与边界：**

- 渲染器输出、材质参数和平台支持分别读取，不把缺图表工具当作配置成功。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。

**调用方式与正反例：**

- 自动选择示例：“Niagara Ribbon 材质不正确，检查渲染器绑定与透明模式。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-renderers-and-materials 完成：Niagara Ribbon 材质不正确，检查渲染器绑定与透明模式。”
- 不触发示例：“修改 UMG 文字颜色。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-renderers-and-materials/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-sequenced-effects"></a>

#### ue-niagara-sequenced-effects

**功能：**通过 Sequencer/事件轨控制 Niagara 生命周期、参数和时序。

**适用场景：**当前请求的核心问题是“通过 Sequencer/事件轨控制 Niagara 生命周期、参数和时序”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Sequencer Niagara`、`特效时序`、`Niagara轨道`。

**不触发与边界：**

- 时序轨与运行时技能清理是两条责任链；仅后者有问题时优先普通 Niagara/GAS。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。

**调用方式与正反例：**

- 自动选择示例：“在过场中精确控制 Niagara 的激活和结束。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-sequenced-effects 完成：在过场中精确控制 Niagara 的激活和结束。”
- 不触发示例：“制作一个命令执行的工作流序列。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-sequenced-effects/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-niagara-vfx"></a>

#### ue-niagara-vfx

**功能：**复核 Niagara System/Emitter、User 参数、附着、池化与结束回调。

**适用场景：**当前请求的核心问题是“复核 Niagara System/Emitter、User 参数、附着、池化与结束回调”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`VFX接口`、`NiagaraComponent`、`粒子池`、`发射器`。

**不触发与边界：**

- 通用特效实现默认 ue-niagara-effects，本条用于接口复核或明确指定。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-niagara-effects`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-material-graph-effects](#skill-ue-material-graph-effects)：视觉效果同时需要材质图变化时，明确共享参数。
- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：特效由能力开始/取消驱动时，核查执行身份和组件清理。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：特效由序列控制时，核查时序轨和中断。
- [ue-niagara-effects](#skill-ue-niagara-effects)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“核实池化 Niagara 组件的自动销毁参数和结束回调。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-niagara-vfx 完成：核实池化 Niagara 组件的自动销毁参数和结束回调。”
- 不触发示例：“生成一张静态爆炸图片。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-niagara-vfx/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/niagara.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-physics-and-chaos"></a>

#### ue-physics-and-chaos

**功能：**复核碰撞模型、刚体、ragdoll、约束和 Chaos 模拟契约。

**适用场景：**当前请求的核心问题是“复核碰撞模型、刚体、ragdoll、约束和 Chaos 模拟契约”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`ragdoll`、`PhysicsAsset`、`刚体约束`、`Chaos接口`。

**不触发与边界：**

- 与主 physics-collision 重合时择一；网络物理与当前 CMC/GAS 适配需单独验证。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-physics-collision`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-abilities](#skill-ue-gameplay-abilities)：查询结果进入伤害/能力 TargetData 时，验证权限与动作身份。
- [ue-networking-replication](#skill-ue-networking-replication)：物理结果需要多端表现时，明确权威事实与表现复制。
- [ue-physics-collision](#skill-ue-physics-collision)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“角色 ragdoll 恢复后运动状态不对，检查参数所有者和清理。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-physics-and-chaos 完成：角色 ragdoll 恢复后运动状态不对，检查参数所有者和清理。”
- 不触发示例：“解释普通物理学中的抛物线。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-physics-and-chaos/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/physics.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-sequencer-and-cinematics"></a>

#### ue-sequencer-and-cinematics

**功能：**复核 SequencePlayer、绑定、相机切换、事件轨和 Movie Render Queue。

**适用场景：**当前请求的核心问题是“复核 SequencePlayer、绑定、相机切换、事件轨和 Movie Render Queue”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`SequencePlayer接口`、`Possessable`、`Spawnable`、`MRQ接口`。

**不触发与边界：**

- 主过场实现默认 ue-sequencer-cinematics；运行时和编辑器渲染依赖要分开。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**普通实现默认 `ue-sequencer-cinematics`；本条在具体接口/底层复核、已有上下文或用户指定时可主导，不自动双载。

**组合规则：**

- [ue-gameplay-cameras](#skill-ue-gameplay-cameras)：序列接管角色视图时，核查相机栈恢复。
- [ue-niagara-effects](#skill-ue-niagara-effects)：序列控制特效激活/参数时，核查资源生命周期。
- [ue-sequencer-cinematics](#skill-ue-sequencer-cinematics)：同职责主入口：按本次意图择一，不能简单叠加两套完整基础流程。

**调用方式与正反例：**

- 自动选择示例：“复核运行时绑定到 SequencePlayer 的 Actor 身份和生命周期。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-sequencer-and-cinematics 完成：复核运行时绑定到 SequencePlayer 的 Actor 身份和生命周期。”
- 不触发示例：“给生成图片添加电影风格。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-sequencer-and-cinematics/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/sequencer.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-14"></a>

### 5.14 UE 调试 测试 性能与打包

<a id="skill-ue-testing-debugging"></a>

#### ue-testing-debugging

**功能：**选择实际自动化断言、日志、调试与运行验证，报告真实结果。

**适用场景：**当前请求的核心问题是“选择实际自动化断言、日志、调试与运行验证，报告真实结果”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Automation`、`BEGIN_DEFINE_SPEC`、`回归测试`、`PIE验证`、`调试`。

**不触发与边界：**

- 不为低价值实现镜像写测试；编译、蓝图、PIE、联机、打包分别验收。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：实际 C++ 变更要求常规构建时执行相应 Editor/Game 目标。
- [ue-debugging-techniques](#skill-ue-debugging-techniques)：测试失败需要定位现场时，选择有针对性的日志/断点。

**调用方式与正反例：**

- 自动选择示例：“为这次 Pawn 更换修复增加覆盖旧回调的回归用例。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-testing-debugging 完成：为这次 Pawn 更换修复增加覆盖旧回调的回归用例。”
- 不触发示例：“为纯 Markdown 改动执行全部 Editor/Game 构建。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-testing-debugging/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/testing.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-automation-and-testing"></a>

#### ue-automation-and-testing

**功能：**深入使用 Simple/Complex Automation、Spec、Functional Test 和测试运行入口。

**适用场景：**当前请求的核心问题是“深入使用 Simple/Complex Automation、Spec、Functional Test 和测试运行入口”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`IMPLEMENT_SIMPLE_AUTOMATION_TEST`、`Spec`、`FunctionalTest`、`自动化框架`。

**不触发与边界：**

- 测试框架专题选择本条；整体改动验收主用 ue-testing-debugging，不重复全量加载。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：实际 C++ 变更要求常规构建时执行相应 Editor/Game 目标。
- [ue-debugging-techniques](#skill-ue-debugging-techniques)：测试失败需要定位现场时，选择有针对性的日志/断点。

**调用方式与正反例：**

- 自动选择示例：“把这个初始化幂等回归场景写成 UE Automation Spec。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-automation-and-testing 完成：把这个初始化幂等回归场景写成 UE Automation Spec。”
- 不触发示例：“仅检查一份文档的错别字。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-automation-and-testing/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/testing.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-debugging-techniques"></a>

#### ue-debugging-techniques

**功能：**用断点、日志、断言、DrawDebug、VisualLogger 和 GameplayDebugger 定位问题。

**适用场景：**当前请求的核心问题是“用断点、日志、断言、DrawDebug、VisualLogger 和 GameplayDebugger 定位问题”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`VisualLogger`、`GameplayDebugger`、`DrawDebug`、`断点`、`复现`。

**不触发与边界：**

- 定位问题主用本条；正式性能测量/优化使用 profiling，而非凭日志计数推断性能。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：实际 C++ 变更要求常规构建时执行相应 Editor/Game 目标。

**调用方式与正反例：**

- 自动选择示例：“复现攻击方向异常，用日志和调试绘制定位最后执行者。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-debugging-techniques 完成：复现攻击方向异常，用日志和调试绘制定位最后执行者。”
- 不触发示例：“为原理说明生成一个交互模拟。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-debugging-techniques/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/testing.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-game-thread-performance"></a>

#### ue-game-thread-performance

**功能：**定位游戏线程的重复查询、Tick、同步加载和任务量开销，并建立有效缓存。

**适用场景：**当前请求的核心问题是“定位游戏线程的重复查询、Tick、同步加载和任务量开销，并建立有效缓存”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`GameThread`、`Tick开销`、`同步加载卡顿`、`帧预算`。

**不触发与边界：**

- 仅在游戏线程已有证据时作为窄化分析；不凭主观感觉修改算法或降低正确性。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [canvas](#skill-canvas)：用户需要独立分析面板时才呈现分析，既有文件报告不额外建 Canvas。

**调用方式与正反例：**

- 自动选择示例：“主线程在大量敌人 Tick 时卡顿，测量热点再优化。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-game-thread-performance 完成：主线程在大量敌人 Tick 时卡顿，测量热点再优化。”
- 不触发示例：“让图片更有速度感。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-game-thread-performance/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/profiling.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-packaging-and-deployment"></a>

#### ue-packaging-and-deployment

**功能：**准备并诊断 Cook、Stage、Package、UAT、目标平台与产物验证。

**适用场景：**当前请求的核心问题是“准备并诊断 Cook、Stage、Package、UAT、目标平台与产物验证”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`BuildCookRun`、`Cook`、`打包`、`Stage`、`UAT`。

**不触发与边界：**

- 安装 skill 不构成打包/发布请求；无 Server Target 时不得臆造独立专服构建命令。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-module-build-system](#skill-ue-module-build-system)：先确保已有目标和依赖在普通构建中成立。
- [ue-data-assets-tables](#skill-ue-data-assets-tables)：Cook 缺资源时追踪软引用和 AssetManager 可达性。

**调用方式与正反例：**

- 自动选择示例：“为当前已有 Game Target 准备 Win64 打包并核对软资源可达性。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-packaging-and-deployment 完成：为当前已有 Game Target 准备 Win64 打包并核对软资源可达性。”
- 不触发示例：“只做 Editor 编译，不需要 Cook。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-packaging-and-deployment/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/packaging.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-profiling-and-optimization"></a>

#### ue-profiling-and-optimization

**功能：**使用 Unreal Insights、stat、CPU/GPU/内存工具建立测量与优化证据。

**适用场景：**当前请求的核心问题是“使用 Unreal Insights、stat、CPU/GPU/内存工具建立测量与优化证据”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Unreal Insights`、`stat unit`、`性能分析`、`GPU耗时`、`内存分配`。

**不触发与边界：**

- 先区分 CPU/GPU/GC 等瓶颈；具体主线程热点可转 game-thread-performance。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 当前会话可发现的项目入口；具体插件、资产和工具能力按任务核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-game-thread-performance](#skill-ue-game-thread-performance)：证据定位到游戏线程时，再做更窄热点分析。
- [canvas](#skill-canvas)：用户需要独立分析面板时才呈现分析，既有文件报告不额外建 Canvas。

**调用方式与正反例：**

- 自动选择示例：“采集同一场景的改前改后数据，定位卡顿并验证收益。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-profiling-and-optimization 完成：采集同一场景的改前改后数据，定位卡顿并验证收益。”
- 不触发示例：“在没有测量时承诺帧率提高一倍。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-profiling-and-optimization/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/profiling.md)；完整上游代码示例未整体移植/编译认证。


<a id="group-15"></a>

### 5.15 UE 外部内容依赖核查

<a id="skill-ue-metahuman-animator"></a>

#### ue-metahuman-animator

**功能：**核查旧版 MetaHuman Animator 的插件、性能资产、导出和本机可用流程。

**适用场景：**当前请求的核心问题是“核查旧版 MetaHuman Animator 的插件、性能资产、导出和本机可用流程”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MetaHuman Animator`、`面部表演`、`MetaHumanPerformance`。

**不触发与边界：**

- 当前未安装所需新能力；本条提供前置诊断，不宣称已移植完整 Animator。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：先核查本机符号、插件和资产，明确 5.8 与 5.5 差异。
- [ue-animation-system](#skill-ue-animation-system)：本机依赖确实成立后，才核查现有面部/身体动画绑定。

**调用方式与正反例：**

- 自动选择示例：“先核查我的 5.5 是否有处理面部表演所需插件和资产。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-metahuman-animator 完成：先核查我的 5.5 是否有处理面部表演所需插件和资产。”
- 不触发示例：“直接调用 5.8 新版音频生成人脸 API。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-metahuman-animator/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/metahuman.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-metahuman-creator"></a>

#### ue-metahuman-creator

**功能：**核查 MetaHuman Creator 外部服务、角色资产和旧版导入工作流。

**适用场景：**当前请求的核心问题是“核查 MetaHuman Creator 外部服务、角色资产和旧版导入工作流”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MetaHuman Creator`、`数字人创建`、`角色组装`。

**不触发与边界：**

- 5.8 内置 Creator 编辑 API 不存在于当前本机环境；不能靠文档提供该接口。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：先核查本机符号、插件和资产，明确 5.8 与 5.5 差异。
- [ue-animation-system](#skill-ue-animation-system)：本机依赖确实成立后，才核查现有面部/身体动画绑定。

**调用方式与正反例：**

- 自动选择示例：“核实已有 MetaHuman 资产与旧版 Creator/Bridge 依赖。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-metahuman-creator 完成：核实已有 MetaHuman 资产与旧版 Creator/Bridge 依赖。”
- 不触发示例：“在 5.5 中直接调用 MetaHumanCharacterEditorSubsystem。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-metahuman-creator/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/metahuman.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-metahuman-live-link"></a>

#### ue-metahuman-live-link

**功能：**核查 MetaHuman Live Link 源、Subject、面部 AnimBP、控制器和录制。

**适用场景：**当前请求的核心问题是“核查 MetaHuman Live Link 源、Subject、面部 AnimBP、控制器和录制”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`MetaHuman Live Link`、`面捕Subject`、`LiveLinkController`。

**不触发与边界：**

- LiveLink 引擎能力与 MetaHuman 角色/源配置分别检查；不能仅凭模块存在称面捕可用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：先核查本机符号、插件和资产，明确 5.8 与 5.5 差异。
- [ue-animation-system](#skill-ue-animation-system)：本机依赖确实成立后，才核查现有面部/身体动画绑定。

**调用方式与正反例：**

- 自动选择示例：“已有 MetaHuman 的面捕 Subject 不驱动动画，检查控制器和绑定。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-metahuman-live-link 完成：已有 MetaHuman 的面捕 Subject 不驱动动画，检查控制器和绑定。”
- 不触发示例：“给标准角色添加身体 IK。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-metahuman-live-link/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/metahuman.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-cinematics-rendering"></a>

#### ue-uds-cinematics-rendering

**功能：**核查已安装 UDS 在 Sequencer、MRQ、Path Tracer 中的时间和渲染行为。

**适用场景：**当前请求的核心问题是“核查已安装 UDS 在 Sequencer、MRQ、Path Tracer 中的时间和渲染行为”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS cinematics`、`UDS MRQ`、`天空序列`。

**不触发与边界：**

- 先确认商业资产及版本，未安装时只诊断，不要求配置不存在的 UDS 字段。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“若项目已有 UDS，核实拍摄序列中天空时间如何驱动。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-cinematics-rendering 完成：若项目已有 UDS，核实拍摄序列中天空时间如何驱动。”
- 不触发示例：“使用原生 SkyAtmosphere 制作天空，不涉及 UDS。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-cinematics-rendering/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-clouds"></a>

#### ue-uds-clouds

**功能：**核查 UDS 云层模式、运动、体积云和性能参数。

**适用场景：**当前请求的核心问题是“核查 UDS 云层模式、运动、体积云和性能参数”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS clouds`、`UDS体积云`、`云层模式`。

**不触发与边界：**

- UDS 云资产不等于原生 VolumetricCloud 组件；当前依赖未核实。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“核查已有 UDS 的云层运动和体积模式参数。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-clouds 完成：核查已有 UDS 的云层运动和体积模式参数。”
- 不触发示例：“在图像中画几朵云。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-clouds/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-fog-and-atmosphere"></a>

#### ue-uds-fog-and-atmosphere

**功能：**核查 UDS 的雾、颜色、体积雾与大气耦合。

**适用场景：**当前请求的核心问题是“核查 UDS 的雾、颜色、体积雾与大气耦合”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS fog`、`UDS atmosphere`、`天空雾`。

**不触发与边界：**

- 按实际资产属性和版本读取，不能照抄 5.8 教程配置。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“在已有 UDS 中排查雾色与天空不一致。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-fog-and-atmosphere 完成：在已有 UDS 中排查雾色与天空不一致。”
- 不触发示例：“调整普通 ExponentialHeightFog，不涉及 UDS。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-fog-and-atmosphere/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-lighting-and-shadows"></a>

#### ue-uds-lighting-and-shadows

**功能：**核查 UDS 太阳/月亮灯、SkyLight、曝光和云影。

**适用场景：**当前请求的核心问题是“核查 UDS 太阳/月亮灯、SkyLight、曝光和云影”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS lighting`、`UDS shadows`、`UDS云影`。

**不触发与边界：**

- 未发现 UDS 时使用 lighting-and-lumen 说明原生机制，不伪装为 UDS 接入。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“核实已安装 UDS 的云影和 SkyLight 更新方式。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-lighting-and-shadows 完成：核实已安装 UDS 的云影和 SkyLight 更新方式。”
- 不触发示例：“给一张图片增加阴影。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-lighting-and-shadows/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-modifiers-configs-state"></a>

#### ue-uds-modifiers-configs-state

**功能：**核查 UDS Modifier、Configuration Manager 与保存恢复状态。

**适用场景：**当前请求的核心问题是“核查 UDS Modifier、Configuration Manager 与保存恢复状态”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS modifier`、`UDS config`、`UDS state`。

**不触发与边界：**

- 商业资产字段先读回；运行时状态保存和插件配置是不同来源。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“已有 UDS 的配置切换后无法恢复，检查状态所有者。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-modifiers-configs-state 完成：已有 UDS 的配置切换后无法恢复，检查状态所有者。”
- 不触发示例：“创建一个 Codex 配置规则。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-modifiers-configs-state/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-performance-mobile-troubleshooting"></a>

#### ue-uds-performance-mobile-troubleshooting

**功能：**核查 UDS 性能、移动端特性级、更新/缓存和常见故障。

**适用场景：**当前请求的核心问题是“核查 UDS 性能、移动端特性级、更新/缓存和常见故障”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS mobile`、`UDS performance`、`UDS cache`。

**不触发与边界：**

- 不自动升级 UDS 或调用未验证缓存重建接口；性能结论需实测。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“测量已有 UDS 对移动平台的云层开销。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-performance-mobile-troubleshooting 完成：测量已有 UDS 对移动平台的云层开销。”
- 不触发示例：“测量普通 UE 主线程开销，不涉及 UDS。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-performance-mobile-troubleshooting/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-setup-and-modes"></a>

#### ue-uds-setup-and-modes

**功能：**核查 UDS 放置、Sky/Color/Project/Feature Mode 的实际资产配置。

**适用场景：**当前请求的核心问题是“核查 UDS 放置、Sky/Color/Project/Feature Mode 的实际资产配置”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS setup`、`Ultra Dynamic Sky`、`UDS模式`。

**不触发与边界：**

- skill 包不含 UDS 资产，不自动购买、安装或声称已启用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“先检查工程是否已有 Ultra Dynamic Sky，再解释可用设置。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-setup-and-modes 完成：先检查工程是否已有 Ultra Dynamic Sky，再解释可用设置。”
- 不触发示例：“安装 Codex 的新 Agent Skill。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-setup-and-modes/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-simulation"></a>

#### ue-uds-simulation

**功能：**核查 UDS 地理位置、日期、时区和真实太阳/月亮模拟。

**适用场景：**当前请求的核心问题是“核查 UDS 地理位置、日期、时区和真实太阳/月亮模拟”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS simulation`、`UDS经纬度`、`天空日期时区`。

**不触发与边界：**

- 真实天气/日历工具不是 UDS；地理和日期模型先从实际资产读取。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“核实已有 UDS 的经纬度与日照模拟是否采用正确时区。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-simulation 完成：核实已有 UDS 的经纬度与日照模拟是否采用正确时区。”
- 不触发示例：“查询明天当地天气。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-simulation/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-sun-moon-stars"></a>

#### ue-uds-sun-moon-stars

**功能：**核查 UDS 日月、月相、星空、极光和空间层。

**适用场景：**当前请求的核心问题是“核查 UDS 日月、月相、星空、极光和空间层”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS sun`、`UDS moon`、`UDS stars`、`UDS极光`。

**不触发与边界：**

- 这是 UDS 专项而非图像生成或原生天空的通用规则。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“调整已安装 UDS 的月相和星空，并核对实际字段。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-sun-moon-stars 完成：调整已安装 UDS 的月相和星空，并核对实际字段。”
- 不触发示例：“生成一张星空壁纸。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-sun-moon-stars/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-uds-time"></a>

#### ue-uds-time

**功能：**核查 UDS 时间推进、昼夜速度、事件与多人同步。

**适用场景：**当前请求的核心问题是“核查 UDS 时间推进、昼夜速度、事件与多人同步”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDS time`、`UDS昼夜`、`UDS TimeOfDay`。

**不触发与边界：**

- 日夜运行时间与外部自动化不同；多人时间事实需权威来源。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“已有 UDS 的昼夜时间在客户端不同步，定位事实来源。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-uds-time 完成：已有 UDS 的昼夜时间在客户端不同步，定位事实来源。”
- 不触发示例：“让 Codex 每天晚上发一条提醒。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-uds-time/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-udw-material-and-screen-effects"></a>

#### ue-udw-material-and-screen-effects

**功能：**核查 UDW 地表湿润、雪、雨滴、屏幕效果和材质联动。

**适用场景：**当前请求的核心问题是“核查 UDW 地表湿润、雪、雨滴、屏幕效果和材质联动”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDW material`、`UDW screen`、`湿润积雪`、`DLWE`。

**不触发与边界：**

- 依赖 UDW 资产和版本，不把原生材质支持视为完整天气集成。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“已有 UDW 的湿润参数没有影响地表，核实材质函数接入。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-udw-material-and-screen-effects 完成：已有 UDW 的湿润参数没有影响地表，核实材质函数接入。”
- 不触发示例：“普通材质做溶解，不涉及天气。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-udw-material-and-screen-effects/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-udw-particles-lightning-wind-sounds"></a>

#### ue-udw-particles-lightning-wind-sounds

**功能：**核查 UDW 雨雪粒子、闪电、风、碰撞和环境音。

**适用场景：**当前请求的核心问题是“核查 UDW 雨雪粒子、闪电、风、碰撞和环境音”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDW rain`、`UDW lightning`、`UDW wind`、`UDW weather audio`。

**不触发与边界：**

- 视效与实际天气状态分离，资产未安装时不臆造工具调用。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“排查已安装 UDW 的雨粒子与风声生命周期。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-udw-particles-lightning-wind-sounds 完成：排查已安装 UDW 的雨粒子与风声生命周期。”
- 不触发示例：“做一个普通 Niagara 火焰，不涉及 UDW。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-udw-particles-lightning-wind-sounds/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-udw-random-seasons-temperature"></a>

#### ue-udw-random-seasons-temperature

**功能：**核查 UDW 随机天气、季节、温度和气候预设。

**适用场景：**当前请求的核心问题是“核查 UDW 随机天气、季节、温度和气候预设”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDW random`、`UDW seasons`、`UDW temperature`。

**不触发与边界：**

- 这是游戏天气模型，不是天气预报；随机和复制状态需明确所有权。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“已有 UDW 的季节与温度变化不一致，核对预设与采样。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-udw-random-seasons-temperature 完成：已有 UDW 的季节与温度变化不一致，核对预设与采样。”
- 不触发示例：“查看真实城市当前气温。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-udw-random-seasons-temperature/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-udw-setup-and-state"></a>

#### ue-udw-setup-and-state

**功能：**核查 UDW 安装状态、预设、手动天气状态和事件接口。

**适用场景：**当前请求的核心问题是“核查 UDW 安装状态、预设、手动天气状态和事件接口”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`Ultra Dynamic Weather`、`UDW setup`、`UDW state`。

**不触发与边界：**

- 安装本条不安装商业插件；字段和事件名必须从实际资产核实。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“先检查是否有 UDW，再说明当前能用的天气状态接口。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-udw-setup-and-state 完成：先检查是否有 UDW，再说明当前能用的天气状态接口。”
- 不触发示例：“仅创建一个 Skill 文档目录。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-udw-setup-and-state/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

<a id="skill-ue-udw-spatial-weather"></a>

#### ue-udw-spatial-weather

**功能：**核查 UDW 天气覆盖体积、径向风暴、天气 Mask 和空间采样。

**适用场景：**当前请求的核心问题是“核查 UDW 天气覆盖体积、径向风暴、天气 Mask 和空间采样”；可从当前 UE 工程上下文识别，不要求用户说出接口名。

**自动触发条件（全部满足）：**

- 对象是当前 UE 工程、其资产/代码/配置，或明确的 UE 工作流请求。
- 请求的实际动作和核心问题属于本条功能；引用、否定、举例或同词异义不触发。

**识别信号（可按语义匹配）：**`UDW spatial`、`Weather Override Volume`、`UDW mask`、`径向风暴`。

**不触发与边界：**

- Weather Mask 能力依赖真实 UDW 资源，不可从原生 Volume 类推断。
- 只读解释不升级为资产写入；纯文档修改不触发 Editor/Game 构建。

**前置条件与实际可用性：**遵守 AGENTS.md 与 UE5.5.4 契约，具体方案前查本机 Lyra；采用上游示例前核实本机精确 API、模块与资产。 可识别并进入依赖核查；所需商业/新版能力没有因安装此 skill 而安装，实际可用性需核实。

**重叠选择：**按最窄职责和当前意图选择；本条不是所有 UE 请求的统一前置。

**组合规则：**

- [ue-navigating-engine-source](#skill-ue-navigating-engine-source)：确认依赖和实际 API；没有商业资产时不执行插件专用操作。
- [ue-lighting-and-lumen](#skill-ue-lighting-and-lumen)：仅讨论引擎原生天空/光照时切换至此入口，明确不是 UDS/UDW 接入。

**调用方式与正反例：**

- 自动选择示例：“已有 UDW 的室内天气遮罩无效，检查空间覆盖与材质路径。” → 识别本条，不要求手动命名。
- 可选的显式调用：“请使用 ue-udw-spatial-weather 完成：已有 UDW 的室内天气遮罩无效，检查空间覆盖与材质路径。”
- 不触发示例：“调整原生碰撞体积的大小。” → 按真实对象、动作和排除条件转交或直接处理。

**验收：**按实际变更执行对应源码/资产/构建/运行验证，分别报告成功、失败和未执行项；源码锚点不等于全部上游示例已移植。

**真实入口与来源：**[项目 UE5.5.4 SKILL.md](E:/Project/Git/Hodgepodge/.agents/skills/ue-udw-spatial-weather/SKILL.md)。
本地适配细节见 [领域参考](../../.agents/ue55-reference/weather.md)；完整上游代码示例未整体移植/编译认证。

## 6 能力缺口和清单维护

- 这份清单只描述本次会话的 143 项。新安装、移除或更新插件后重新盘点当前会话清单；不因磁盘上新增一个同名目录就断言其可调用。
- 插件路径和个人路径属于本机快照。迁移机器先重新定位真实入口，保留完整标识、来源和边界；项目的 UE skill 可随仓库迁移，外部缓存不因此一起安装。
- `disable-model-invocation` 和 `allow_implicit_invocation` 是不同来源/宿主的字段；是否生效必须按实际宿主核实。手册和规则都不是权限系统或自动执行 Hook。
- 不可用的原生 UI、尚未连接的外部服务、没有安装的商业资产、缺失的新版引擎 API 均单独诊断。不要以换工具、绕过权限或伪造操作掩盖缺口。
- 新的相近 skill 应补充正例、反例、首选关系与组合条件；不要只追加关键词。新自然语言说法无需逐字登记，助手仍按实际意图判断。当前会话新增的条目即使尚未收录，仍可按真实 description/SKILL.md 选择；记录目录待同步，不因此中断任务或顺手改写无关手册。
- 修改 `.agents/skill-catalog.json` 或短规则后，在项目根目录执行 `python -X utf8 .agents/scripts/build_skill_manual.py` 更新本文件；执行同命令加 `--check` 只读检查生成一致性、143 项覆盖、来源摘要、锚点和组合引用。
- 外部 SKILL.md 更新导致摘要变化时，先重读变更并同步逐项规则/来源摘要，再生成；不能直接刷新摘要掩盖未审查的触发或权限变更。
- 本次文档验证不执行所有 skill，不产生平台自动选择准确率结论，也不代表 C++、蓝图、PIE、联机或打包测试通过。
