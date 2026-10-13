# 残象之翼暂停启用记录

2026-10-11，按用户要求暂时屏蔽翅膀。

- `/Game/Main/Experiences/Exp_HodgeDefaultExperience` 中，Pover 的 `BP_PoverWingComponent` 注入条目保留，客户端与服务器选项均关闭。
- `/Game/Main/Character/Hero/Anim/Montages/AM_Attack04_Montage` 移除唯一的 `NamedComponent/WingMesh` 命中通知，剩余五条原有通知保留。
- 模型、独立骨架、WingAttach、三个检测插槽、动画蓝图、Profile、组件蓝图和代码保留；通用 GAS 取消权限修复保留。
- 制作脚本默认禁止重新启用，后续需要明确设置 ENABLE_POVER_WING=True 并恢复 Experience 注入选项。

实际执行：

```powershell
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/disable_assets.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Saved/WingImplementation/start_listen.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/verify_disabled.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/EnemyAI/stop_validation.py
& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/Wing/restore_validation.py
git diff --check
```

Experience 蓝图编译、相关资产保存和禁用配置查询通过。Listen Server 双玩家 PIE 中，服务器和客户端的四个玩家实例均没有 HodgeWingComponent 或 WingMesh，真实输入能走完五段普攻，结果 error=null。

资产备份及原始结果位于 `Saved/WingDisable`。第一次保存后写 JSON 时遇到 UE Array 序列化问题，已转为 Python list；资产变更已保存，重跑脚本验证为幂等。

本次未修改 C++，未运行构建、Cook 或打包；原地图与性能选项已恢复，没有提交 Git。
