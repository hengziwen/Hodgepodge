# dev-Combat 合并验证 · 2026-10-24

## 范围与状态

用户要求先将 `dev-Combat` 合入 `main`，再实施 Hero 自动重生。工作区外部操作已创建并同步合并提交 `e738342`，父提交为 `ab360d3` 与 `5f2d342`。本记录只描述合并修复与实际验证，不把两侧历史测试计为本次通过。

## 冲突处理

- 保留 main 的库存装备切换检查与 Combat 的独立命中检测组件，移除已迁入命中组件的旧局部查找函数。
- PawnData 同时保留库存校验和削韧校验；公开头文件同时包含两类配置所需类型。
- Editor 验证库同时保留 `QueueNumberPopProbe` 与 `InspectReactionMotion`，各自声明、函数体和闭括号独立。
- 知识库保留伤害跳字、背包和战斗改造记录，标注历史“受击暂缓”已被本次合并替代。

合并提交误将正式 `DA_Dafult_PawnData.uasset` 的 LFS 冲突文本作为 255 字节资产提交，无法加载。已从两侧 LFS 原始对象恢复并只读导出比较，再在 main 的原始资产上合入 Combat 的完整 `Poise`：

| 配置 | 合并结果 |
| --- | --- |
| bUseInventory | 保留 main 的 true |
| InventoryMaxEntries | 两侧同为 40 |
| DefaultInventoryItems | 保留 main 的 ID_Sword ×1，等级 1，默认装备 |
| Poise | 保留 Combat 的启用状态与完整参数 |
| PawnClass、AbilitySets、StatProfile、HitReactionProfile、输入、相机、连段等 | 两侧一致，保留原值 |

写入前已保留错误文件、main 原件、Combat 原件及原有 Guardian 修改的二进制备份，位于 `Saved/MergeDevCombat_20261024_002357/`。该生成目录不入库。临时比较资产已删除。Guardian 内容的 SHA256 仍为 `73a268436808145d5ac0ee8e2973f0a25216b1ac5320bf720336a861c823c0f1`，与开始时一致；保护用 stash 保留，未重复应用。

## 实际验证

- `git diff --check` 与源码／文档冲突标记检查通过。
- Editor：原常规构建在 UBA 内存调度等待后中止；改用本机 UBT 已支持的 `-NoUBA -NoUBALocal -MaxParallelActions=1`，退出码 0。
- 命令：`Build.bat HodgepodgeEditor Win64 Development -Project=E:/Project/Git/Hodgepodge/Hodgepodge.uproject -WaitMutex -architecture=x64 -NoUBA -NoUBALocal -MaxParallelActions=1`。
- UE Python commandlet：两侧资产只读导出退出码 0；目标保存与 `EditorValidatorSubsystem.is_asset_valid` 返回 `VALID`，无数据校验警告。正式资产保存后 SHA256 为 `99398dd8ddba8dc95557a7c5551b6c1006d123bb5a7dd018db3bff00ae1dab5f`。
- Game：同参数运行 `Build.bat Hodgepodge Win64 Development ... -NoUBA -NoUBALocal -MaxParallelActions=1`，退出码 0。
- 原生自动化：`UnrealEditor-Cmd.exe ... -unattended -NullRHI -ExecCmds="Automation RunTests Hodge." -TestExit="Automation Test Queue Empty" -ReportExportPath=.../Automation`，退出码 0，247 项全部成功（138 项无警告、109 项带警告，失败／未运行均为 0）。报告保留于同一 Saved 目录，不能视为 PIE／联机玩法验收。
- 未验证：本次合并后的完整战斗演出、所有网络时序、Cook／打包。既有 Montage 分支通知重叠等加载警告仍保留，未扩大本次合并修复范围。
