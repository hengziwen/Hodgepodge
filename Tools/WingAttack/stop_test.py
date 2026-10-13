"""停止本次 PIE，保持保存资产不变。"""
import unreal
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
print('请求停止翅膀测试 PIE')
