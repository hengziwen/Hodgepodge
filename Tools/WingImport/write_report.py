"""Write a durable asset inventory and validation record from actual import results."""
import json,collections,subprocess
from pathlib import Path
PROJECT=Path('E:/Project/Git/Hodgepodge')
REPORT=PROJECT/'Saved/WingImport'
DOC=PROJECT/'Docs/Validation/anzhu-wing-import-2026-10-12.md'
results=json.loads((REPORT/'import-results.json').read_text(encoding='utf-8'))
validation=json.loads((REPORT/'validation-fresh.json').read_text(encoding='utf-8'))
state=json.loads((REPORT/'gameplay-state.json').read_text(encoding='utf-8'))
assert validation['passed'] and state['wing_gameplay_disabled']
assets=results['imported']+results['materials']
counts=collections.Counter(a['type'] for a in assets)
sources=json.loads((REPORT/'source-files.json').read_text(encoding='utf-8'))
text=[
 '# Anzhu 翅膀资产导入与材质配置记录（2026-10-12）',
 '',
 '## 交付范围',
 '',
 '- 源目录：`D:/下载/BaiduwangpanDownloads/Anzhu`。',
 '- 导入目录：`/Game/Wuwa/Wing`；源文件的目录结构、名称和大小写保持原样。',
 f'- 原始文件 {len(sources)} 个，复制后 SHA256 全部一致，原始目录未修改。',
 '- 导入主翅膀、`Model/test` 变体和附带镰刀；变体与主翅膀共用骨架，镰刀使用独立骨架。',
 '- 仅制作资产与导入工具；未修改 C++、角色攻击、装备、Experience 或已有翅膀配置。',
 '- 已核对旧 `BP_PoverWingComponent` 的客户端/服务端注入仍均为 false。',
 '',
 '## 单位与骨架',
 '',
 'UE 5.5.4 默认以厘米为长度单位，并非把项目改成米。所有源 FBX 的 `UnitScaleFactor=100`，表示声明的长度单位是米；但模型顶点仍是百量级。实际导入采用 `Convert Scene=true`、`Convert Scene Unit=true` 和用户指定的 `Import Uniform Scale=0.01`，模型和动画使用完全相同的设置。转换与缩放组合后，主翅膀包围盒为约 `163.004 × 6.552 × 162.956 cm`；测试变体约相同，镰刀约 `286.845 × 14.941 × 120.673 cm`。',
 '',
 '- 主翅膀骨架保留源文件全部 139 根骨骼，包括 `Root` 和 `Bone_Wing000` 等翼骨。',
 '- 35 段 `BaseAnim`、10 段 `RibbonAnim` 使用主翅膀骨架。',
 '- 镰刀保留独立的 4 根骨骼，2 段镰刀动画只绑定这套骨架。',
 '- 未改写现有 `SK_Pover_LyraLab` 或上一套 PMX 翅膀骨架，也没有进行角色动画重定向。',
 '- 动画按源文件时间范围以 30 fps 导入，全部关闭 Root Motion，避免意外驱动角色位移。',
 '- 动画预览网格已设置为自己的翅膀/镰刀网格。模型 FBX 自带的动画不另外生成重复 AnimSequence。',
 '',
 '模型参考姿势的 `Bone_Wing000` 在根节点上方约 98.4 cm，而部分攻击动画会把该骨骼移动到另一位置。这些变换来自源 FBX。本轮保留原动画，不把模型的原点改成背部插槽；后续挂载角色时要单独校准组件与角色动作，不能直接套用旧 PMX 翅膀的挂载偏移。',
 '',
 '## 三个材质槽',
 '',
 '主模型槽位顺序保持源 FBX 的顺序：',
 '',
 '1. `MI_Trans_Anzhu_Wing_80008_CM`：半透明特效层，1200 个三角形，使用独立铺满 0～1 的 UV。',
 '2. `MI_EDarkNanzhuWingMd1001_Wing`：实体羽翼，3684 个三角形，连接原始 D 贴图。',
 '3. `MI_Mask_Anzhu_Wing_80014_CM`：遮罩羽片，2162 个三角形，连接原始 D 贴图。',
 '',
 '主模型材质实例放在 `/Game/Wuwa/Wing/EDarkNanzhuWingMd10011/Model/Materials`；测试变体的实例放在其 `test/Materials` 下，使用自己的贴图。四个可编辑父材质放在 `/Game/Wuwa/Wing/Materials`。没有把默认材质留在任何模型槽位中。',
 '',
 '### 特效槽的处理',
 '',
 '源文件只提供羽翼 D 贴图，FBX 中没有贴图引用，也没有原始游戏的特效材质图、独立特效遮罩或 Niagara 资源。因此本次为第一个槽位制作了 UE 原生可调的替代材质，并非原游戏特效的精确还原。',
 '',
 '- 父材质：`M_Wing_FX`；`Translucent + Unlit + Two Sided`，启用 Skeletal Mesh 用途。',
 '- 使用 UV0 的 U/V 双向软边和纵向渐变生成透明遮罩，避免把整张羽翼图集铺到特效几何上。',
 '- 紫蓝色自发光，带轻微正弦明暗变化；不会产生伤害、碰撞或额外粒子。',
 '- `EffectOpacity=0.16`：控制透明强度，设为 0 可隐藏特效层。',
 '- `EffectColor=(0.12,0.035,0.65)`：线性空间发光色。',
 '- `EmissiveStrength=2.5`：自发光强度。',
 '- `EdgeFalloff=1.5`：软边衰减；`PulseFrequency=0.5`、`PulseAmplitude=0.1`：轻微呼吸效果。',
 '',
 '调整方法：打开主模型第一个材质槽对应的 `MI_Trans_Anzhu_Wing_80008_CM`，勾选需要覆盖的参数，再调上述值并保存。不必修改父材质，也不需要实现新的 GA。未来有原始特效遮罩或材质参数时，可以替换父材质图并保留槽位与实例引用。',
 '',
 '### 实体与遮罩槽的处理',
 '',
 '- `M_Wing_Surface` 使用 Default Lit、Opaque、Two Sided，并以 `BaseTexture` 参数连接正确 D 贴图。',
 '- `M_Wing_Mask` 使用 Default Lit、Masked、Two Sided。源 D 贴图 Alpha 几乎恒定为 254～255，不能把它当有效透明遮罩；本次用贴图亮度排除黑色图集空白，裁剪阈值为 0.01。',
 '- 原始色图以 sRGB 导入；镰刀 `_N` 纹理保留为线性 Masks。源文件未提供其通道约定，没有将这张含混合通道的纹理误接成普通 RGB Normal。镰刀材质已连接 D 贴图。',
 '',
 '## 验证结果',
 '',
 f'- 当前编辑器验证与独立渲染 commandlet 从磁盘加载验证均通过；最终 {len(validation["checks"])} 项检查全部通过。',
 '- 检查覆盖：66 个资产可加载、网格尺寸、骨骼名、材质槽数量/顺序、模型/动画缩放、动画时长、骨架绑定、五个采样时刻的有限骨骼变换、实际动态/保持姿势、材质父级与贴图引用、材质 shader 指令统计、全部源文件 SHA256。',
 '- 原生网格编辑器目视检查通过：黑色羽毛、蓝紫羽片和半透明发光层可见。',
 '- 42 段动画有实际骨骼运动，5 段源动画为保持姿势的片段；没有把静态源片段视为导入丢失动画。',
 '- 临时预览关卡已还原为 `/Game/CodexText/L_MainMenu`，本轮结束时无未保存地图或资产。',
 '- 未进行角色挂载、完整动画视觉逐段验收、攻击判定、PIE、联机、Cook 或打包测试。未修改 C++，未重复构建。',
 '',
 '### 源文件中的保持姿势片段',
 '',
 ]
for animation in validation['animations']:
    if not animation['authored_motion']:
        text.append(f'- `{animation["path"].split(".")[0]}`：{animation["seconds"]:.3f} 秒，源曲线只有小于 0.001 的浮点变化。')
text += [
 '',
 '### 实际命令',
 '',
 '```powershell',
 '& E:/Python/python.exe Tools/WingImport/inspect_sources.py',
 '& E:/Python/python.exe Tools/WingImport/inspect_animation_channels.py',
 '& E:/Python/python.exe Tools/WingImport/copy_sources.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingImport/import_assets.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingImport/configure_materials.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingImport/validate_assets.py',
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingImport/finalize_assets.py',
 "$env:HODGE_WING_FRESH='1'",
 "& E:/UE/UE_5.5/Engine/Binaries/Win64/UnrealEditor-Cmd.exe E:/Project/Git/Hodgepodge/Hodgepodge.uproject -run=pythonscript '-script=E:/Project/Git/Hodgepodge/Tools/WingImport/validate_assets.py' -AllowCommandletRendering -NoShaderWorker -unattended -nosplash -NoSound '-abslog=E:/Project/Git/Hodgepodge/Saved/WingImport/validation-fresh-render.log'",
 '& E:/Python/python.exe Tools/HitReaction/mcp_execute.py Tools/WingImport/check_gameplay_state.py',
 '```',
 '',
 '最终导入、材质配置、编辑器校验和启用渲染的磁盘校验命令均退出 0。早期工具尝试已修正 UE Python 受保护属性/引脚名的调用问题；未开启 commandlet 渲染的尝试无法读取 shader 统计。后一次并行 shader worker 校验虽然资产检查全部通过，但引擎云材质的 shader worker 意外退出，commandlet 因此退出 1；同时本机产生了内存不足的 JVM 诊断文件。最终用 UE 已支持的 `-NoShaderWorker` 在进程内编译并复核，不修改引擎、项目或插件设置。相关失败日志和 JVM 诊断保留在 Saved/WingImport，不纳入源码。临时 SceneCapture 尝试不能驱动普通编辑器 Actor 的动画刷新，未将其静态截图用于动画验收；目视依据使用原生网格编辑器。',
 '',
 '证据：`Saved/WingImport/import-results.json`、`validation-editor.json`、`validation-fresh.json`、`source-files.json`、`gameplay-state.json` 和对应日志；网格截图位于 `Saved/Screenshots/wing-editor-preview.png`。',
 '',
 '## 完整资产清单',
 '',
 f'共 {len(assets)} 个 UE 资产。',
 ''
 ]
for cls,count in sorted(counts.items()):
    text += [f'### {cls}（{count}）','']
    for item in assets:
        if item['type']==cls:text.append('- `'+item['path'].split('.')[0]+'`')
    text.append('')
text += ['## 材质引用修复清单','']
for row in results['repairs']:
    text += [f'- 网格 `{row["mesh"].split(".")[0]}`，槽位 {row["slot_index"]} `{row["slot"]}`：',
             f'  使用 `{row["material"].split(".")[0]}`；'+('独立 UV 程序化特效，无外部贴图依赖。' if row['effect'] else f'`BaseTexture` 指向 `{row["base_texture"].split(".")[0]}`。')]
DOC.parent.mkdir(parents=True,exist_ok=True)
DOC.write_text('\n'.join(text)+'\n',encoding='utf-8')
durable={'assets':assets,'repairs':results['repairs'],'scale':results['scale'],'validation':validation,'gameplay_state':state,'source_files':sources}
DOC.with_name('anzhu-wing-import-2026-10-12-results.json').write_text(json.dumps(durable,ensure_ascii=False,indent=2),encoding='utf-8')
(REPORT/'status-after.txt').write_bytes(subprocess.check_output(['git','status','--short'],cwd=PROJECT))
print(json.dumps({'document':str(DOC),'assets':len(assets),'checks':len(validation['checks'])},ensure_ascii=False))
