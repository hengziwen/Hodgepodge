"""停止锁定验证 PIE，保持生产资产不变。"""
import unreal
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
