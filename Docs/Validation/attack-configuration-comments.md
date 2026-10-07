# 攻击配置手册与字段注释验证

本轮只新增／更新说明文档并插入中文注释，不改变已有字段、默认值、结构、函数声明／实现、声明顺序或 include。没有修改资产、GameplayTag 注册或构建设置，没有提交代码。

## 注释与文档范围

- HodgeAbilityDefinition.h：Definition、执行／混合／时间轴引用配置。
- HodgeAbilityTimeline.h：Window／Point 和时间轴配置；已有完整注释保留，只补源时间与边界限制。
- HodgeHitDetection.h：来源、Profile、Volume、绑定及运行请求／结果。
- HodgeComboDefinition.h：连段 DataTable 行、跳转边、输入映射与记忆。
- HodgeAbilitySet.h：Definition 授予字段及能力包。
- HodgePawnData.h：攻击关联的角色配置。
- HodgeInputConfig.h：输入标签匹配说明。

7 个头文件共核对 145 个反射字段，补充 114 行中文注释。已有准确说明没有为了增加数量而重复；新注释尽量一行解释一件事，没有给 include 新增注释。

[攻击能力配置手册](../Guides/attack-ability-configuration.md)共 510 行，涵盖每个配置字段的意义、默认值、适用条件、关联引用、8 类配置配方、排查以及运行期字段。README、文档目录、本地知识库和 AI_DEVELOPMENT 已加入统一入口，后续配置问答优先引用对应章节。

## 验证方法与结果

修改前保存七个头文件的原始字节，路径 `Saved/AttackConfigDocs/Before`。对修改前后逐行比较：只允许 insert，新增行必须是中文 // 注释，不允许原行删除／替换／移动。去除注释后的代码等价，include 列表完全相同，原编码／换行保留。145 个字段名称均在手册中可检索。

脚本：`Saved/AttackConfigDocs/verify.py`；结果：`verification.json`，通过。手册 12 个目录锚点、本地文档链接和 git diff --check 通过。知识库刷新／漂移检查结果另存本轮证据。

UE 5.5.4 Editor／Game Win64 Development 常规构建均退出 0；UHT 正常处理注释元数据，生成文件含 ToolTip 元数据。日志 `Saved/AttackConfigDocs/Editor-build.log`、`Game-build.log`。保留既有 IncludeOrder 提示、UMG IsFocusable 弃用等警告，未改变相关设置。

实际命令：

```powershell
python -X utf8 Saved/AttackConfigDocs/verify.py
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' HodgepodgeEditor Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoHotReloadFromIDE -NoUBA -NoUBALocal -MaxParallelActions=1
& 'E:/UE/UE_5.5/Engine/Build/BatchFiles/Build.bat' Hodgepodge Win64 Development '-Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject' -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1
python -X utf8 Docs/KnowledgeBase/tools/kb.py refresh
python -X utf8 Docs/KnowledgeBase/tools/kb.py check
git diff --check
```

本次没有重新执行原生玩法测试、蓝图编译、PIE、联机或打包。上轮多段攻击的验收记录是上轮证据，不当成本轮复跑。

开始时编辑器在运行但 MCP 连接拒绝；没有通过编辑器修改资产。用户保存关闭后完成常规构建。用户已有未提交代码和资产变更保留。
