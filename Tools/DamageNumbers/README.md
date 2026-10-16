# Lyra 跳字开发与验收工具

本目录只服务本地跳字功能；正式伤害不调用这些工具。设计与配置见 [方案](../../Docs/Design/damage-numbers-lyra.md)，结果见 [验证记录](../../Docs/Validation/damage-numbers-lyra.md)。

1. 编辑器关闭时运行 `python Tools/DamageNumbers/migrate_lyra_assets.py`，只迁入六个已列明的 Lyra 资源，拒绝覆盖不同内容。
2. 完成 Editor/Game 常规构建，启动本项目编辑器，再通过现有鉴权网关运行 `setup_style.py`，创建缺失的 Hodge 样式并核查真实依赖。已存在样式只读取，不自动覆盖。
3. 用 `editor_state.py` 记录地图和性能设置；UE 5.5 的 PIE 模式枚举无法由本机 Python 读取，结束时明确恢复单人 PIE 基线，不声称完整还原原始 PIE 设置。
4. `python Tools/DamageNumbers/run_batch.py` 顺序执行 single、listen-host、listen、dedicated、listen-lag。复用正式验证地图，隔离 AI，在 PIE 实例上补满测试生命，不修改角色或攻击资产。Combo 输入在实际取消窗口开启后注入，避免把提前注入的测试输入丢弃当作连段错误。
5. `python Tools/DamageNumbers/run_batch.py --capture single` 逐帧截图：仅这次单人取证使用 `t.OverrideFPS 60`，完成时恢复。属于固定仿真步长的离线 60fps 视频素材，不是实时 60fps 性能证明。普通网络测试使用实际帧时间。

`Saved/DamageNumbers` 保存报告、历史失败、依赖清单、构建日志和图片，不提交生成数据。`run_e2e.py` 区分正式五段/E/R 命中、独立 GE 压力夹具与最后一击夹具。高频部分同帧发出 40 次真实 Damage GE，每个目标扣 5 HP；最后一击将一个测试实例降至 10 HP，再请求 100，预期本地收到 10。

完成后停止 PIE，在网关执行 `editor_state.py` 并传入 `RESTORE_NUMBERS_EDITOR=True`，恢复记录的地图与性能设置；任何脏资产都应先明确处理，不强制关闭或自动保存用户资产。
