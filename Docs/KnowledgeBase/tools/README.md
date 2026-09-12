# 知识库维护工具

[返回知识库](../README.md)

`kb.py` 只使用 Python 标准库。建议 Python 3.10 或更新版本；本次在本机 Python 3.11 执行。它根据脚本位置定位仓库，与当前工作目录无关；下面命令假设终端位于仓库根目录。

## 查询

```powershell
python Docs/KnowledgeBase/tools/kb.py search "PawnData"
python Docs/KnowledgeBase/tools/kb.py search "InputTag.Ability.Melee" --limit 60
```

大小写不敏感，逐行匹配，优先人工章节再查 Reference。输出仓库相对路径、行号和短上下文，便于 IDE 跳转。它不解析中文分词，也不是向量检索；检索类名、函数、Tag 或完整短语最可靠。

## 检查

```powershell
python Docs/KnowledgeBase/tools/kb.py check
```

检查知识库 Markdown 中本地文件链接的存在性，以及快照范围内文件是否新增、删除或改变 SHA-256。任一链接错误或漂移返回退出码 1，全部通过返回 0。它不验证 Markdown 页内锚点、源码行号仍指向原符号或 UE 资源引用完整性。

漂移范围是 Source、Config、Content/Main、uproject 和四份原始根目录文档。第三方插件、Saved、Intermediate、Binaries、Content 其他目录内容不做哈希跟踪。全 Content 只在资产参考页按顶层目录统计文件数。

## 刷新

```powershell
python Docs/KnowledgeBase/tools/kb.py refresh
```

刷新会覆盖工具管理的 Reference Markdown 和 snapshot.json。不要直接编辑自动生成页；源数据说明修改脚本中的 PURPOSE，架构结论写入人工章节。新增源码子目录会自动生成对应参考页；旧子目录消失时，旧生成页不会自动删除，维护者需核对后清理不再链接的旧页。

刷新不修改 Source、Config、Content，也不改变人工章节。默认配置摘录隐藏 Token/Password/Secret 等凭据类字段值。源配置本身不被更改。

## 推荐更新流程

1. 先运行 check，查看有哪些源码/配置/资产发生漂移。
2. 打开对应源码核对有效函数体与调用者；蓝图字段在 UE 编辑器确认。
3. 更新受影响的人工章节、接通清单和验收结果，标注新的核对日期。
4. 运行 refresh 重建参考与快照。
5. 再运行 check，检查没有断链；手工抽查更改相关的源符号和说明。

不要直接 refresh 后就声称结论同步：快照只证明“已扫描”，人工章节仍可能过期。

## 索引解析边界

脚本去掉 C++ 行注释和块注释，保留字符串和行号，提取常见顶层限定函数定义、头文件有效声明、项目内 include 和原生 Tag 宏。它不会展开宏、求值 WITH_EDITOR/UE_WITH_IRIS、处理完整 C++ AST 或读取 uasset。

“定义候选”不是调用图。项目 include 链也不是运行时调用链。复杂模板、特殊宏、非标准排版可能漏检；跨源码和蓝图的行为必须人工核对。

## 本次实际验证

已执行 refresh、check 和包含中文/英文类名的 search，检查了注释 Hero 文件不会产生有效定义，校正了 Super 调用误入定义索引的问题，并确认配置摘录隐藏凭据类字段。UE 编译、PIE、网络与资产内部值不在此工具验证范围内。
