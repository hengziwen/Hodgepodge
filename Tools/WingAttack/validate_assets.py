"""只读验证现行通知蒙太奇资产，保留原工具路径供既有调用使用。"""
from pathlib import Path
# 正式验证只有一个入口，避免同时维护映射表和通知两套契约。
exec(Path(__file__).with_name('validate_montages.py').read_text(encoding='utf-8-sig'))
