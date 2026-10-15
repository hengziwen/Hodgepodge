# 性能分析：UE 5.5.4 适配

先读 [项目契约](../ue55-policy.md)。以下是本地执行流程；上游 5.8 文档仅用于按需比较。

## 开发流程

1. 先固定硬件、地图、角色数量、网络模式和复现步骤，采集 stat/Unreal Insights 数据再决定优化。
2. 区分游戏线程、渲染线程、GPU、分配和 GC 开销，找到调用者与触发频率后再修改。
3. 优先事件驱动、减少重复查询和合理缓存；缓存要有明确失效来源，不能用错误状态换取帧率。
4. 改前改后使用相同场景对比；没有测量结果时描述预期影响并明确未验证，不虚构提升比例。

## 本机源码依据

引擎根目录：`E:/UE/UE_5.5/Engine`。行号供定位，调用前重新读取完整声明、实现及调用者。

- `Source/Runtime/Core/Public/ProfilingDebugging/CpuProfilerTrace.h:352`：`TRACE_CPUPROFILER_EVENT_SCOPE`。
- `Source/Runtime/Core/Public/Stats/Stats.h:285`：`DECLARE_CYCLE_STAT`。

## 验证范围

本次核实了以上本机文件、列出的符号及领域工作流入口。未对上游全部代码块做 5.5 移植或编译；具体玩法/工具变更仍按项目规范执行对应构建和运行验证。
