# 扫描快照

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

## 生成信息

- UTC：2026-09-09T16:52:46.761936+00:00
- Git HEAD：`aefbd66a15743dfb9eea2887b9697d22aa298182`
- 源文件（h/cpp/cs）：132
- Content/Main 文件：36
- 原生标签注册条目：144
- 漂移跟踪文件：177

## 生成时已有的受 Git 跟踪修改

```text
M Config/DefaultEngine.ini
AM Source/Hodgepodge/Private/Component/HodgeHeroComponent.cpp
 M Source/Hodgepodge/Private/Data/HodgePawnData.cpp
AM Source/Hodgepodge/Public/Component/HodgeHeroComponent.h
 M Source/Hodgepodge/Public/Data/HodgePawnData.h
```

这里只记录已跟踪路径状态，未跟踪知识库自身不包含在此列表。HEAD 不足以还原 dirty 工作区；[snapshot.json](snapshot.json) 保存扫描范围的 SHA-256。

## 验证边界

没有运行 Unreal 编译、PIE、打包或蓝图数据解析。Content/Main 之外只统计顶层文件数量，不做引用结论。函数与宏索引是导航候选，不做 C++ 语义解析。人工章节核对日期不会由 refresh 自动更新。

运行 `python Docs/KnowledgeBase/tools/kb.py check` 检查相对链接和跟踪文件漂移。发现变化后，先复核人工章节，再刷新快照。
