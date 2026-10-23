"""真实受击补转采集及离线验收；完整保留 Actor、Mesh 和原生移动证据，不保存资产。"""
from pathlib import Path
from decimal import Decimal
import argparse
import json
import math
import re
import sys
import time
import traceback

# 原生结算Time用%.6f序列化；仅关联诊断时应用半个末位单位，不改变玩法验收容差。
RESOLUTION_TIME_HALF_QUANTUM = Decimal('0.0000005')
HERO_RECOVERY_PROFILE = '/Game/Main/Character/Hero/Anim/HitReactions/DA_Hero_HitReaction.DA_Hero_HitReaction'


def restore_ground_recovery_scope(scope, reason, restored_at=None):
    """恢复获批专项的同一Profile；读回失败时保留令牌，供异常清理再次恢复。"""
    if scope is None:
        return
    profile, record = scope['profile'], scope['record']
    profile.set_editor_property('bEnableGroundRecovery', record['before_enabled'])
    actual = profile.get_editor_property('bEnableGroundRecovery')
    if actual is not record['before_enabled']:
        raise RuntimeError('专项结束后无法恢复原有地面恢复开关: ' + record['profile'])
    record.update(restored=True, restored_enabled=actual, restore_reason=reason, restored_at=restored_at)


def recovery_scope_failures(report):
    """仅允许Hero两项专项隔离，逐帧确认其余正式场景与预热保持原恢复配置。"""
    baseline = report.get('ground_recovery_baseline', {})
    scopes = report.get('ground_recovery_scopes', [])
    failures = []
    player = report.get('player_target') is True
    if type(baseline.get('enabled')) is not bool or not baseline.get('profile') or \
            player and (baseline['profile'] != HERO_RECOVERY_PROFILE or baseline['enabled'] is not True):
        return ['正式恢复配置基线缺失，或玩家未使用批准的Hero恢复配置']
    expected = [name for name in report.get('requested_cases', []) if player and name in ('SameLevel', 'Upgrade')]
    if [scope.get('case') for scope in scopes] != expected:
        failures.append('专项覆盖缺失、重复、错序或扩展到未获批场景')
    for scope in scopes:
        if scope.get('profile') != HERO_RECOVERY_PROFILE or scope.get('before_enabled') is not True or \
                scope.get('override_enabled') is not False or scope.get('restored') is not True or \
                scope.get('restored_enabled') is not True or scope.get('restore_reason') != 'case_end' or \
                not isinstance(scope.get('entered_at'), (int, float)) or \
                not isinstance(scope.get('restored_at'), (int, float)) or \
                not math.isfinite(scope['entered_at']) or not math.isfinite(scope['restored_at']) or \
                scope['restored_at'] < scope['entered_at']:
            failures.append({'case': scope.get('case'), 'reason': '专项没有完整配对进入、还原与原值读回证据'})
    rows = report.get('warmup', {}).get('samples', []) + report.get('samples', [])
    for row in rows:
        expected_enabled = False if player and row['case'] in expected else baseline['enabled']
        if row.get('recovery_profile') != baseline['profile'] or row.get('ground_recovery_enabled') is not expected_enabled:
            failures.append({'case': row['case'], 'sample': row.get('sample_index'),
                             'reason': '专项外配置被覆盖，或专项内没有使用获批隔离设置'})
    return failures


def yaw_delta(start, end):
    """返回最短水平角差，正负跨界不构成额外转身。"""
    return (float(end) - float(start) + 180.0) % 360.0 - 180.0


def guid_key(value):
    """统一反射导出与日志 GUID，仅规范格式，不替换执行身份。"""
    text = str(value)
    fields = dict(re.findall(r'([ABCD])=(-?\d+)', text))
    if len(fields) == 4:
        return ''.join(f'{int(fields[key]) & 0xffffffff:08X}' for key in 'ABCD')
    result = re.sub(r'[-{}\s]', '', text).upper()
    if not re.fullmatch(r'[0-9A-F]{32}', result):
        raise ValueError('无法识别 GUID: ' + text)
    return result


def facing_identity(row):
    """段身份不使用每刀 Sequence，也不把不同 Avatar 合并。"""
    facing = row['facing']
    return row['avatar'], guid_key(facing['ChainId']), int(facing['Segment'])


def facing_preset_matches(facing, max_angle, window):
    """真实采样验证可观察的角度上限、时窗和转速，不把最大补角误当作必转角。"""
    values = [facing[key] for key in ['StartYaw', 'TargetYaw', 'StartedAt', 'EndsAt', 'MaxYawSpeed']]
    return all(math.isfinite(value) for value in values) and \
        abs(yaw_delta(facing['StartYaw'], facing['TargetYaw'])) <= max_angle + .05 and \
        abs(facing['EndsAt'] - facing['StartedAt'] - window) <= .0001 and abs(facing['MaxYawSpeed'] - 720.) <= .0001


def enum_is(value, expected):
    """仅统一原生CamelCase与Python枚举下划线表示，不替换实际结算结果。"""
    return re.sub(r'[^A-Za-z]', '', str(value)).upper().endswith(expected.replace('_', '').upper())


def followup_proof_failures(name, events):
    """复核同步真实结算证据，区分普通接触、成功同级提交与真正空中追加。"""
    failures = []
    if len(events) != 2 or any(event['damage'] <= 0. or event.get('collected_by') != 'NativeHitFacingResolved' for event in events):
        return ['需要两个与原生HitFacingResolved唯一关联的真实正伤害结算']
    first, second = events
    if not enum_is(first['outcome'], 'Applied'):
        failures.append('首击未成功提交受击计划')
    first_id = (guid_key(first['facing_chain']), first['facing_segment'])
    second_id = (guid_key(second['facing_chain']), second['facing_segment'])
    if first_id[0] == '0' * 32 or first_id[1] <= 0:
        failures.append('首击未建立有效补转身份')
    if name == 'ContactNoImpact':
        if not enum_is(second['outcome'], 'NoImpact'):
            failures.append('普通接触未产生预期NoImpact结算')
        if second_id[1] > 0 and second_id != first_id:
            failures.append('普通接触意外建立新补转段')
        return failures
    if second_id != first_id:
        failures.append('追加命中没有保持同一控制链补转段')
    if name == 'SameLevel':
        if not enum_is(second['outcome'], 'Applied') or not all(event['controlled_at_resolution'] and
                'CONTROLLED' in event['phase_at_resolution'].upper() for event in events):
            failures.append('同级追加未在连续地面控制中成功提交')
        if second['sequence_at_resolution'] <= first['sequence_at_resolution'] or \
                second['chain_started_at_resolution'] != first['chain_started_at_resolution']:
            failures.append('同级追加缺少新计划与原控制链的同步证据')
        if first['montage_at_resolution'] is None or second['montage_at_resolution'] != first['montage_at_resolution'] or \
                first['montage_instance_at_resolution'] < 0 or \
                second['montage_instance_at_resolution'] != first['montage_instance_at_resolution']:
            failures.append('成功同级提交没有延续原受击动画实例')
    elif name == 'AirFollow':
        if not enum_is(second['outcome'], 'NoImpact'):
            failures.append('非续飞空中追加没有保持原有NoImpact裁决')
        if not second['controlled_at_resolution'] or not second['falling_at_resolution'] or \
                'AIRBORNE' not in second['phase_at_resolution'].upper():
            failures.append('第二次真实GE结算时未同时处于Airborne与CMC离地状态')
        if second['probe_kind'] != 'AirHit' or second['can_launch']:
            failures.append('空中追加不是已禁止新击飞的真实AirHit探针')
    return failures


def full_chain_proof_failures(events):
    """正式控制与削韧参数不变，同一目标必须按真实结算完成接触到击飞。"""
    if len(events) != 4 or any(event['damage'] <= 0. or event.get('collected_by') != 'NativeHitFacingResolved' for event in events):
        return ['完整流程需要四个同步真实正伤害结算']
    contact, light, heavy, launch = events
    failures = []
    if not enum_is(contact['outcome'], 'NoImpact') or contact['controlled_at_resolution'] or contact['facing_segment']:
        failures.append('普通接触意外建立控制或补转段')
    if not all(enum_is(event['outcome'], 'Applied') for event in [light, heavy, launch]) or \
            'LIGHT' not in light['level'].upper() or 'HEAVY' not in heavy['level'].upper() or \
            launch['probe_kind'] != 'Launch' or not launch['can_launch'] or 'AIRBORNE' not in launch['phase_at_resolution'].upper():
        failures.append('真实结算未依次成为Light、Heavy和首次Launch')
    return failures


def proxy_convergence_proof(observed, group):
    """以前帧作为接收下界，在身份有效期内证明0.25秒内收敛，打断单独记边界。"""
    first = group[0]
    index = observed.index(first)
    result = {'verdict': 'unproven', 'receive_lower_bound': None, 'receive_upper_bound': first['world_time'],
              'deadline': None, 'interrupted_at': None, 'converged_at': None, 'reason': None}
    if index == 0:
        result['reason'] = '缺少首个冻结观察前的逐帧样本，无法给出OnRep接收下界'
        return result, []
    previous = observed[index - 1]
    gap = first['world_time'] - previous['world_time']
    if not 0. <= gap < .25 or first['sample_index'] != previous['sample_index'] + 1 or \
            first['frame_index'] != previous['frame_index'] + 1:
        result['reason'] = '首个冻结观察与前帧不连续或间隔达到0.25秒'
        return result, []
    result.update(receive_lower_bound=previous['world_time'], deadline=previous['world_time'] + .25)
    identity = facing_identity(first) + (int(first['facing']['Revision']),)
    active = []
    for row in observed[index:]:
        if not row['controlled'] or facing_identity(row) + (int(row['facing']['Revision']),) != identity:
            result['interrupted_at'] = row['world_time']
            break
        active.append(row)
    timely = [row for row in active if row['world_time'] <= result['deadline'] and
              abs(yaw_delta(row['actor_yaw'], row['facing']['FinalYaw'])) <= 2.]
    late = [row for row in active if row['world_time'] >= result['deadline']]
    if timely and all(abs(yaw_delta(row['actor_yaw'], row['facing']['FinalYaw'])) <= 2. for row in late):
        result.update(verdict='converged', converged_at=timely[0]['world_time'])
    elif result['interrupted_at'] is not None and result['interrupted_at'] < result['deadline']:
        result.update(verdict='interrupted', reason='旧身份在保守0.25秒截止前被新段或清空打断，不追旧段收敛')
    else:
        result['reason'] = '有效身份内不能严格证明保守接收下界起0.25秒内收敛并保持'
    return result, late


def parse_facing_uint32_fields(exported):
    """从结构完整导出读取受保护的uint32身份，缺失或歧义均停止，绝不补零。"""
    if not isinstance(exported, str):
        raise TypeError('Facing导出必须是字符串')
    text = exported.strip()
    if not (text.startswith('(') and text.endswith(')')):
        raise ValueError('Facing导出缺少完整结构边界: ' + text)
    body = text[1:-1]
    parts, start, depth, quoted, escaped = [], 0, 0, False, False
    for index, char in enumerate(body):
        if quoted:
            if escaped:
                escaped = False
            elif char == '\\':
                escaped = True
            elif char == '"':
                quoted = False
            continue
        if char == '"':
            quoted = True
        elif char == '(':
            depth += 1
        elif char == ')':
            depth -= 1
            if depth < 0:
                raise ValueError('Facing导出括号不匹配: ' + text)
        elif char == ',' and depth == 0:
            parts.append(body[start:index])
            start = index + 1
    if quoted or depth != 0:
        raise ValueError('Facing导出字符串或嵌套结构未结束: ' + text)
    parts.append(body[start:])
    result = {}
    for part in parts:
        key, separator, value = part.partition('=')
        if not separator or not key.strip():
            raise ValueError('Facing导出包含非法字段: ' + text)
        key, value = key.strip(), value.strip()
        if key not in ('Segment', 'Revision'):
            continue
        if key in result:
            raise ValueError('Facing导出身份字段重复: ' + key + ': ' + text)
        if not re.fullmatch(r'[0-9]+', value) or not 0 <= int(value) <= 0xffffffff:
            raise ValueError('Facing导出身份不是合法uint32: ' + key + ': ' + text)
        result[key] = int(value)
    if set(result) != {'Segment', 'Revision'}:
        raise ValueError('Facing导出缺少Segment或Revision: ' + text)
    return result


def unique_npc_intent_export(rows, parse_transition):
    """遍历UE实际导出的结构文本，唯一意图保留完整导出值，空标签由反射解析器过滤。"""
    intents = set()
    for row in rows:
        for serialized in row.get('Transitions', []):
            if not isinstance(serialized, str):
                raise TypeError('NPC转换必须是UE ExportText字符串')
            exported = parse_transition(serialized)
            if exported is None:
                continue
            if not isinstance(exported, str) or not exported:
                raise TypeError('NPC意图解析器必须返回完整标签文本或None')
            intents.add(exported)
    if len(intents) != 1:
        raise ValueError('成熟NPC夹具需要唯一合法意图，实际数量: ' + str(len(intents)))
    return next(iter(intents))


def parse_native_log(text):
    """只解析带明确事件名的原生记录，保留行号及全部原文供失败追查。"""
    events = []
    for number, line in enumerate(text.splitlines(), 1):
        if 'LogHodgeHitReactionFacing:' not in line or 'Event=' not in line:
            continue
        values = dict(re.findall(r'(\w+)=([^\s]+)', line))
        if values.get('Event') in ('NativeCorrection', 'NativeRejectedRestore') and len(values) != len(re.findall(r'(\w+)=([^\s]+)', line)):
            raise ValueError(f'原生朝向响应第{number}行存在重复字段: {line}')
        event = {'line': number, 'raw': line, **values}
        if values['Event'] in ('Step', 'Correction', 'FrozenRestore', 'NativeCorrection', 'NativeRejectedRestore', 'CurrentRestore', 'HistoricalRestore', 'ReplayBegin', 'ReplayEnd'):
            required = ['Avatar', 'Chain', 'Segment', 'Revision', 'WorldTime', 'Role']
            if values['Event'] == 'Step':
                required += ['BeforeYaw', 'AfterYaw', 'DeltaTime', 'EffectiveDeltaTime', 'MaxYawSpeed', 'TargetYaw', 'Replay']
            elif values['Event'] == 'NativeCorrection':
                required += ['BeforeYaw', 'AfterYaw', 'NativeYaw', 'NativeType', 'MoveTimeStamp']
            elif values['Event'] == 'NativeRejectedRestore':
                required += ['BeforeYaw', 'AfterYaw', 'RejectedYaw', 'NativeType', 'MoveTimeStamp']
            elif values['Event'] in ('Correction', 'FrozenRestore', 'CurrentRestore', 'HistoricalRestore'):
                endpoint = 'AnchorYaw' if values['Event'] == 'CurrentRestore' else 'RecordedYaw' if values['Event'] == 'HistoricalRestore' else 'FinalYaw'
                required += ['BeforeYaw', 'AfterYaw', endpoint]
            else:
                required += ['BeforeYaw' if values['Event'] == 'ReplayBegin' else 'AfterYaw']
            missing = [key for key in required if key not in values]
            if missing:
                raise ValueError(f'原生第 {number} 行缺少 {missing}: {line}')
            event['chain_key'] = guid_key(values['Chain'])
            for key in ['Segment', 'Revision', 'Role', 'Replay', 'Paused', 'Hard', 'Final']:
                if key in event:
                    event[key] = int(event[key])
            for key in ['WorldTime', 'ServerTime', 'BeforeYaw', 'AfterYaw', 'DeltaTime',
                        'EffectiveDeltaTime', 'TargetYaw', 'FinalYaw', 'AnchorYaw', 'RecordedYaw', 'MaxYawSpeed',
                        'NativeYaw', 'MoveTimeStamp', 'ServerYaw', 'RejectedYaw']:
                if key in event:
                    event[key] = float(event[key])
                    if not math.isfinite(event[key]):
                        raise ValueError(f'原生第 {number} 行 {key} 非有限值')
        events.append(event)
    return events


def parse_native_resolutions(text):
    """解析结算同步点的只读诊断，缺字段、重复字段及非法身份均拒绝。"""
    result = []
    required = {'Avatar', 'Hit', 'Time', 'Outcome', 'Level', 'Phase', 'Falling', 'Controlled',
                'Sequence', 'ChainStartedAt', 'Chain', 'Segment', 'Revision', 'Montage', 'MontageInstance', 'CanLaunch'}
    for number, line in enumerate(text.splitlines(), 1):
        if 'LogHodgeHitPresentation:' not in line or 'HitFacingResolved ' not in line:
            continue
        pairs = re.findall(r'(\w+)=([^\s]+)', line.split('HitFacingResolved ', 1)[1])
        values = dict(pairs)
        if len(values) != len(pairs) or not required <= values.keys():
            raise ValueError(f'原生结算第{number}行字段重复或缺失: {line}')
        event = {'line': number, 'raw': line, **values}
        event['hit_key'], event['chain_key'] = guid_key(values['Hit']), guid_key(values['Chain'])
        if event['hit_key'] == '0' * 32:
            raise ValueError(f'原生结算第{number}行命中身份为空')
        for key in ['Time', 'ChainStartedAt']:
            event[key] = float(values[key])
            if not math.isfinite(event[key]):
                raise ValueError(f'原生结算第{number}行{key}非有限')
        for key in ['Falling', 'Controlled', 'CanLaunch', 'Sequence', 'Segment', 'Revision', 'MontageInstance']:
            if not re.fullmatch(r'-?[0-9]+', values[key]):
                raise ValueError(f'原生结算第{number}行{key}不是整数')
            event[key] = int(values[key])
        if any(event[key] not in (0, 1) for key in ['Falling', 'Controlled', 'CanLaunch']) or \
                any(not 0 <= event[key] <= 0xffffffff for key in ['Segment', 'Revision']) or event['Sequence'] < 0:
            raise ValueError(f'原生结算第{number}行布尔值或执行身份越界')
        if values['Outcome'] not in {'NoRequest', 'NoDamageOutput', 'Rejected', 'Dead', 'InvalidRequest',
                'LowJudgement', 'NoImpact', 'Unsupported', 'NotReady', 'CannotInterrupt', 'Busy', 'Applied'} or \
                values['Level'] not in {'None', 'Light', 'Heavy', 'Break'} or \
                values['Phase'] not in {'None', 'Controlled', 'Airborne', 'Downed', 'GettingUp'}:
            raise ValueError(f'原生结算第{number}行枚举值未知')
        result.append(event)
    return result


def correlate_resolution_events(case, rows, polled, native_resolutions):
    """按目标和Hit唯一关联原生同步点，保留轮询原件，遗漏或歧义不能冒充同步证据。"""
    authority = [row for row in rows if row['peer'] == 'authority']
    if not authority:
        return list(polled), ['没有权威逐帧样本，无法界定本场景原生结算范围']
    avatars = {row['avatar'] for row in authority}
    if len(avatars) != 1:
        return list(polled), ['权威目标不唯一，无法关联原生结算']
    start = case.get('observed_from_world_time', authority[0]['world_time'])
    end = case.get('observed_until_world_time', authority[-1]['world_time'])
    if not math.isfinite(start) or not math.isfinite(end):
        return list(polled), ['场景时间边界非有限，无法关联原生结算']
    # 比较序列化证据的十进制区间，避免二进制浮点在恰好半微秒处再引入舍入误差。
    start_decimal, end_decimal = Decimal(str(start)), Decimal(str(end))
    candidates = [event for event in native_resolutions if event['Avatar'] in avatars and
                  Decimal(str(event['Time'])) + RESOLUTION_TIME_HALF_QUANTUM >= start_decimal and
                  Decimal(str(event['Time'])) - RESOLUTION_TIME_HALF_QUANTUM <= end_decimal]
    failures, linked, used = [], [], set()
    previous_native_time = -math.inf
    for event in polled:
        matches = [item for item in candidates if item['Avatar'] == event.get('avatar') and
                   item['hit_key'] == guid_key(event['hit_id'])]
        if len(matches) != 1:
            failures.append({'hit_id': event['hit_id'], 'reason': '同Avatar与Hit的原生结算不是唯一一条',
                             'native_lines': [item['line'] for item in matches]})
            linked.append(dict(event))
            continue
        native = matches[0]
        if native['line'] in used or native['Time'] < previous_native_time or \
                Decimal(str(native['Time'])) - RESOLUTION_TIME_HALF_QUANTUM > Decimal(str(event['time'])) or \
                not enum_is(event['outcome'], native['Outcome']) or not enum_is(event['level'], native['Level']):
            failures.append({'hit_id': event['hit_id'], 'reason': '重复消费、原生时点晚于轮询或结果与轮询不一致',
                             'native_line': native['line']})
        used.add(native['line'])
        previous_native_time = native['Time']
        linked.append({**event, 'poll_observation': dict(event), 'native_resolution': native,
                       'collected_by': 'NativeHitFacingResolved', 'time': native['Time'],
                       'outcome': native['Outcome'], 'level': native['Level'],
                       'phase_at_resolution': native['Phase'], 'controlled_at_resolution': bool(native['Controlled']),
                       'falling_at_resolution': bool(native['Falling']), 'sequence_at_resolution': native['Sequence'],
                       'chain_started_at_resolution': native['ChainStartedAt'], 'facing_chain': native['chain_key'],
                       'facing_segment': native['Segment'], 'facing_revision': native['Revision'],
                       'montage_at_resolution': None if native['Montage'] == 'None' else native['Montage'],
                       'montage_instance_at_resolution': native['MontageInstance'], 'can_launch': bool(native['CanLaunch'])})
    missing = [event['line'] for event in candidates if event['line'] not in used]
    if missing:
        failures.append({'reason': '原生真实结算没有逐一对应的轮询命中，不能遗漏同帧结果', 'native_lines': missing})
    return linked, failures


def cancel_identity_proof(observed, canceled_identity, authority, snapshot):
    """初始空状态不等于收到Clear，只在见过旧身份并离开后拒绝可见重入。"""
    seen, left, failures = False, False, []
    for row in observed:
        current = facing_identity(row)[1:] == canceled_identity and row['controlled']
        if authority and row['frame_index'] > snapshot['frame_index'] and current:
            failures.append({'sample': row['sample_index'], 'reason': '权威取消后仍可见旧段控制'})
        if current:
            if left:
                failures.append({'sample': row['sample_index'], 'reason': '旧段离开后重新成为可见当前控制'})
            seen = True
        elif seen:
            left = True
    return {'observation': 'observed_then_left' if seen and left else 'never_observed' if not seen else 'still_observed',
            'failures': failures}


def parse_sample_markers(text):
    """解析采样结束标记；完整引擎帧数来自正文，不使用日志前缀的模1000帧。"""
    result = []
    required = {'Avatar', 'Chain', 'Segment', 'Revision', 'sample_index', 'engine_frame', 'Yaw', 'WorldTime'}
    for number, line in enumerate(text.splitlines(), 1):
        if 'LogPython:' not in line or 'HitFacingSample ' not in line:
            continue
        pairs = re.findall(r'(\w+)=([^\s]+)', line.split('HitFacingSample ', 1)[1])
        fields = dict(pairs)
        if len(fields) != len(pairs) or not required <= fields.keys():
            raise ValueError(f'采样标记第{number}行重复或缺失字段: {line}')
        event = {'line': number, 'raw': line, **fields, 'chain_key': guid_key(fields['Chain'])}
        for key in ['Segment', 'Revision', 'sample_index', 'engine_frame']:
            if not re.fullmatch(r'[0-9]+', fields[key]):
                raise ValueError(f'采样标记第{number}行{key}不是非负整数')
            event[key] = int(fields[key])
        if any(event[key] > 0xffffffff for key in ['Segment', 'Revision']) or event['engine_frame'] > 0x7fffffffffffffff:
            raise ValueError(f'采样标记第{number}行身份或引擎帧数越界')
        for key in ['Yaw', 'WorldTime']:
            event[key] = float(fields[key])
            if not math.isfinite(event[key]):
                raise ValueError(f'采样标记第{number}行{key}非有限')
        result.append(event)
    return result


def correlate_sample_markers(rows, markers):
    """每条原始样本必须唯一匹配同Avatar标记，缺失、重复、错序或端点不一致均失败。"""
    by_key, links, failures = {}, {}, []
    for marker in markers:
        by_key.setdefault((marker['Avatar'], marker['sample_index']), []).append(marker)
    previous_by_avatar = {}
    for row in rows:
        key = (row['avatar'], row['sample_index'])
        candidates = by_key.get(key, [])
        if key in links or len(candidates) != 1:
            failures.append({'avatar': key[0], 'sample_index': key[1], 'reason': '样本或日志标记缺失／重复',
                             'native_lines': [value['line'] for value in candidates]})
            continue
        marker = candidates[0]
        if (marker['chain_key'], marker['Segment'], marker['Revision']) != \
                (guid_key(row['facing']['ChainId']), row['facing']['Segment'], row['facing']['Revision']) or \
                marker['engine_frame'] != row.get('engine_frame') or \
                marker['Yaw'] != row['actor_yaw'] or marker['WorldTime'] != row['world_time']:
            failures.append({'avatar': key[0], 'sample_index': key[1], 'line': marker['line'], 'reason': '标记身份、完整引擎帧或端点与原样本不一致'})
        previous = previous_by_avatar.get(key[0])
        if previous and (marker['line'] <= previous['line'] or marker['sample_index'] != previous['sample_index'] + 1 or
                         marker['engine_frame'] < previous['engine_frame']):
            failures.append({'avatar': key[0], 'sample_index': key[1], 'line': marker['line'], 'reason': '同目标采样标记日志错序、缺号或引擎帧倒退'})
        previous_by_avatar[key[0]] = marker
        links[key] = marker
    unmatched = [value['line'] for key, values in by_key.items() if key not in links for value in values]
    if unmatched:
        failures.append({'reason': '存在没有原始样本的额外标记', 'native_lines': unmatched})
    return links, failures


def native_interval(events, previous, current, marker_links=None):
    """新证据按同Avatar采样标记行序开左闭右归因；旧世界时路径仅兼容读取。"""
    if marker_links is not None:
        before = marker_links.get((previous['avatar'], previous['sample_index']))
        after = marker_links.get((current['avatar'], current['sample_index']))
        if not before or not after or before['line'] >= after['line']:
            return []
        return [event for event in events if event.get('Avatar') == current['avatar'] and
                before['line'] < event['line'] <= after['line']]
    identities = {}
    for row in (previous, current):
        identities.setdefault(facing_identity(row), set()).add(int(row['facing']['Revision']))
    return [event for event in events if
            event.get('Revision') in identities.get((event.get('Avatar'), event.get('chain_key'), event.get('Segment')), set()) and
            'WorldTime' in event and
            previous['world_time'] + 0.0000001 < event['WorldTime'] <= current['world_time'] + 0.0000001]


def audit_visible_interval(records, previous_yaw):
    """用当前约束的回放收尾核对可见端点，历史中间Yaw不能被当作额外正常转向。"""
    expected_yaw = previous_yaw
    replaying = False
    failures = []
    for event in records:
        kind = event.get('Event')
        if kind == 'ReplayBegin':
            if replaying:
                failures.append({'line': event['line'], 'reason': '回放作用域重复开始'})
            replaying = True
        elif kind == 'CurrentRestore':
            if not replaying or abs(yaw_delta(event['AfterYaw'], event['AnchorYaw'])) > .05 or \
                    abs(yaw_delta(expected_yaw, event['AnchorYaw'])) > .05:
                failures.append({'line': event['line'], 'reason': '当前锚点恢复未保持已核对的可见约束'})
        elif kind == 'HistoricalRestore' and not replaying:
            failures.append({'line': event['line'], 'reason': '历史朝向恢复脱离真实回放作用域'})
        elif kind in ('Correction', 'FrozenRestore'):
            # 同段Final校正明确允许瞬转，按真实端点更新；不能推广给CurrentRestore。
            expected_yaw = event['AfterYaw']
        elif kind == 'NativeCorrection':
            # 已获批的原生响应另留证，不能把旧缓存恢复伪装成下一次正常限速。
            if replaying or abs(yaw_delta(expected_yaw, event['BeforeYaw'])) > .05:
                failures.append({'line': event['line'], 'reason': '原生响应没有承接已核对端点，或错误嵌入历史回放'})
            expected_yaw = event['AfterYaw']
        elif kind == 'NativeRejectedRestore':
            # 拒绝恢复只能抵消本次未获授权的写入，前后净变化必须为零。
            if replaying or abs(yaw_delta(expected_yaw, event['BeforeYaw'])) > .05 or \
                    abs(yaw_delta(event['BeforeYaw'], event['AfterYaw'])) > .05:
                failures.append({'line': event['line'], 'reason': '拒绝回包恢复改变了此前已核对的可见朝向'})
            expected_yaw = event['AfterYaw']
        elif kind == 'Step' and not event.get('Replay'):
            if replaying:
                failures.append({'line': event['line'], 'reason': '回放作用域包含非历史移动步'})
            anchor = event['AfterYaw'] - event['normal_delta'] if event.get('nested_correction_lines') else event['BeforeYaw']
            if abs(yaw_delta(expected_yaw, anchor)) > .05:
                failures.append({'line': event['line'], 'reason': '正常移动步未承接已归因可见端点',
                                 'expected_yaw': expected_yaw, 'logged_anchor_yaw': anchor})
            expected_yaw += event['normal_delta']
        elif kind == 'ReplayEnd':
            if not replaying or abs(yaw_delta(expected_yaw, event['AfterYaw'])) > .05:
                failures.append({'line': event['line'], 'reason': '回放收尾可见端点与当前约束不一致'})
            replaying = False
    if replaying:
        failures.append({'reason': '回放作用域没有结束'})
    return expected_yaw, failures


def partition_native_motion(events, sample_markers=None):
    """保留所有事件，分离范围日志中嵌套的真实校正，避免同一次变化被计两遍。"""
    result = []
    previous_steps = {}
    for original in events:
        event = dict(original)
        if event.get('Event') != 'Step':
            result.append(event)
            continue
        event['normal_delta'] = yaw_delta(event['BeforeYaw'], event['AfterYaw'])
        event['nested_correction_lines'] = []
        previous_line = previous_steps.get(event['Avatar'], -1)
        # 明确的回放边界不能被外层下一次正常移动吞并，收尾校正属于已结束的作用域。
        previous_line = max([previous_line] + [value['line'] for value in result if
                            value.get('Avatar') == event['Avatar'] and value.get('Event') in ('ReplayBegin', 'ReplayEnd', 'NativeCorrection', 'NativeRejectedRestore')])
        if sample_markers is not None:
            previous_line = max([previous_line] + [marker['line'] for marker in sample_markers if
                                marker['Avatar'] == event['Avatar'] and marker['line'] < event['line']])
        nested = [value for value in result if value.get('Event') in ('Correction', 'FrozenRestore', 'HistoricalRestore') and
                  value.get('Avatar') == event['Avatar'] and value.get('chain_key') == event.get('chain_key') and
                  value.get('Segment') == event['Segment'] and value.get('Revision') == event['Revision'] and
                  previous_line < value['line'] < event['line'] and
                  (sample_markers is not None or abs(value['WorldTime'] - event['WorldTime']) <= 0.0000001)]
        # 新证据不跨采样标记配对；同身份且端点吻合的内层日志才从外层正常步剥离。
        if len(nested) == 1 and abs(yaw_delta(event['BeforeYaw'], nested[0]['BeforeYaw'])) <= .05 and \
                abs(yaw_delta(event['AfterYaw'], nested[0]['AfterYaw'])) <= .05:
            event['normal_delta'] -= yaw_delta(nested[0]['BeforeYaw'], nested[0]['AfterYaw'])
            event['nested_correction_lines'] = [nested[0]['line']]
        previous_steps[event['Avatar']] = event['line']
        result.append(event)
    return result


def audit_report(report, native_events, native_resolutions=None, sample_markers=None):
    """离线验收实际记录；缺样本、缺原生证据和任何失败均保留为失败。"""
    checks = []
    native_events = partition_native_motion(native_events, sample_markers if report.get('requires_sample_markers') else None)
    mesh_observations = []
    resolution_observations, convergence_observations, cancellation_observations, boundary_observations = [], [], [], []

    def check(name, condition, detail=None):
        checks.append({'name': name, 'passed': bool(condition), 'detail': detail})

    marker_links = None
    markers_required = report.get('requires_sample_markers') is True
    marker_failures = []
    warmup = report.get('warmup', {})
    all_rows = warmup.get('samples', []) + report.get('samples', [])
    all_hit_events = warmup.get('hit_events', []) + report.get('hit_events', [])
    if markers_required:
        marker_links, marker_failures = correlate_sample_markers(all_rows, sample_markers or [])
        check('逐帧样本均有唯一同身份因果标记', bool(all_rows) and not marker_failures, marker_failures)
    check('采集无异常并完整清理', report.get('error') is None and report.get('cleanup_complete') is True)
    check('保留原生执行记录', any(event.get('Event') == 'Step' for event in native_events))
    cases = report.get('cases', [])
    check('全部请求场景实际完成', bool(cases) and [case['name'] for case in cases] == report.get('requested_cases'))
    if report.get('requires_recovery_scope_proof'):
        recovery_failures = recovery_scope_failures(report)
        check('仅获批两项专项隔离恢复且已完整还原', not recovery_failures, recovery_failures)
    if report.get('requires_warmup'):
        check('真实预热完整留证并先于正式场景完成', warmup.get('completed') is True and warmup.get('error') is None and
              warmup.get('requested_actions') == ['WarmupHitStun', 'WarmupKnockback', 'WarmupLaunch'] and
              [case['name'] for case in warmup.get('cases', [])] == warmup.get('requested_actions') and
              bool(warmup.get('samples')) and bool(report.get('samples')) and
              max(row['frame_index'] for row in warmup['samples']) < min(row['frame_index'] for row in report['samples']))
    for case in warmup.get('cases', []) + cases:
        name = case['name']
        is_warmup = name in warmup.get('requested_actions', [])
        rows = [row for row in all_rows if row['case'] == name]
        events = [event for event in all_hit_events if event['case'] == name]
        if report.get('requires_native_resolutions'):
            events, resolution_failures = correlate_resolution_events(case, rows, events, native_resolutions or [])
            check(name + ':每次结算都有唯一原生同步证据', not resolution_failures, resolution_failures)
            resolution_observations.extend(events)
            if is_warmup:
                expected = 'Launch' if name == 'WarmupLaunch' else 'Knockback' if name == 'WarmupKnockback' else 'HitStun'
                check(name + ':原生真实计划与预热动作一致', len(events) == 1 and events[0].get('probe_kind') == expected and
                      enum_is(events[0]['outcome'], 'Applied') and events[0].get('controlled_at_resolution') and
                      (events[0].get('phase_at_resolution') == 'Airborne' if expected == 'Launch' else
                       events[0].get('phase_at_resolution') == 'Controlled'), events)
            elif name in ('ContactNoImpact', 'SameLevel', 'AirFollow'):
                failures = followup_proof_failures(name, events)
                check(name + ':同步结算符合实际追加语义', not failures, failures)
            elif name == 'Upgrade':
                check(name + ':真实控制窗内升级接管', len(events) == 2 and all(enum_is(event['outcome'], 'Applied') and
                      event.get('controlled_at_resolution') for event in events) and
                      events[0].get('facing_chain') == events[1].get('facing_chain') and
                      [event.get('facing_segment') for event in events] == [1, 2], events)
            elif name == 'FullChain':
                failures = full_chain_proof_failures(events)
                check(name + ':同目标正式参数完整结算链', not failures, failures)
        is_cancel = name == 'Cancel'
        cancel_snapshot = case.get('cancel_snapshot')
        if is_cancel:
            check(name + ':取消发生于真实转动后的活动窗口', bool(cancel_snapshot) and cancel_snapshot['controlled'] and
                  cancel_snapshot['facing']['bHasTarget'] and not cancel_snapshot['facing']['bFinal'] and
                  cancel_snapshot['server_time'] < cancel_snapshot['facing']['EndsAt'] and
                  abs(yaw_delta(cancel_snapshot['facing']['StartYaw'], cancel_snapshot['actor_yaw'])) > 1., cancel_snapshot)
        check(name + ':真实正伤害', len(events) >= case['expected_hits'] and all(event['damage'] > 0. for event in events))
        check(name + ':运行断言', bool(case['checks']) and all(item['passed'] for item in case['checks']), case['checks'])
        check(name + ':完整逐帧采样', bool(rows) and all(row.get('sample_index') is not None for row in rows))
        authority = [row for row in rows if row['peer'] == 'authority' and row['controlled']]
        check(name + ':实际建立面源段', any(row['facing']['bHasTarget'] for row in authority))
        # 硬锁可以先于第一个CMC步；该用例以冻结不重开为准，其他场景仍须证明实际转动。
        if name != 'HardLock' and not is_warmup:
            check(name + ':确实从侧背方向发生转动', any(abs(yaw_delta(case['initial_target_yaw'], row['actor_yaw'])) > 1. for row in authority))
        check(name + ':同时保留Mesh世界朝向', all(math.isfinite(row['mesh_world_yaw']) and math.isfinite(row['mesh_base_relative_yaw']) for row in rows))
        check(name + ':关键样本数值有限', all(math.isfinite(row[key]) for row in rows
              for key in ['world_time', 'server_time', 'actor_yaw', 'mesh_world_yaw', 'mesh_base_relative_yaw']))
        peer_roles = {row['peer_key']: row['peer'] for row in rows}
        expected_roles = ['authority'] if report.get('mode') == 'single' else \
            ['authority', 'owner'] if report.get('mode') == 'listen' and report.get('player_target') else \
            ['authority', 'proxy'] if report.get('mode') == 'listen' else \
            ['authority', 'owner', 'proxy'] if report.get('player_target') else ['authority', 'proxy', 'proxy']
        check(name + ':实际端角色完整', sorted(peer_roles.values()) == sorted(expected_roles), peer_roles)
        for peer in sorted({row['peer_key'] for row in rows}):
            observed = [row for row in rows if row['peer_key'] == peer]
            violations = []
            compared = 0
            for previous, current in zip(observed, observed[1:]):
                if current['sample_index'] != previous['sample_index'] + 1 or current['world_time'] < previous['world_time']:
                    violations.append({'reason': '采样不连续或时间倒退', 'sample': current['sample_index']})
                if current.get('frame_index') != previous.get('frame_index', -2) + 1:
                    violations.append({'reason': '目标在完整逐帧采集期间缺失，不能声称连续相关', 'sample': current['sample_index']})
                records = native_interval(native_events, previous, current, marker_links)
                causal_boundary = marker_links is not None and (previous['controlled'] or current['controlled'] or bool(records))
                if not (previous['controlled'] and current['controlled']) and not causal_boundary:
                    continue
                compared += 1
                if previous['controlled'] != current['controlled'] or facing_identity(previous) != facing_identity(current):
                    boundary_observations.append({'case': name, 'peer': peer, 'sample': current['sample_index'],
                        'previous_identity': facing_identity(previous), 'current_identity': facing_identity(current),
                        'previous_controlled': previous['controlled'], 'current_controlled': current['controlled'],
                        'previous_yaw': previous['actor_yaw'], 'current_yaw': current['actor_yaw'],
                        'previous_driver': previous.get('rotation_driver'), 'current_driver': current.get('rotation_driver'),
                        'previous_action': previous.get('rotation_action_request'), 'current_action': current.get('rotation_action_request'),
                        'native_lines': [event['line'] for event in records]})
                if current['peer'] != 'proxy':
                    actual = yaw_delta(previous['actor_yaw'], current['actor_yaw'])
                    endpoint, replay_failures = audit_visible_interval(records, previous['actor_yaw'])
                    violations.extend(replay_failures)
                    recorded = yaw_delta(previous['actor_yaw'], endpoint)
                    if abs(yaw_delta(recorded, actual)) > .05:
                        violations.append({'reason': '可见Actor变化没有同身份原生归因', 'sample': current['sample_index'],
                                           'actual': actual, 'recorded': recorded, 'native_lines': [event['line'] for event in records]})
                if current['peer'] == 'owner' and current['facing']['bFinal'] and not current['paused']:
                    # 收到权威状态后的实际消费时点由原生 Correction 另核；这里记录各可见收敛样本。
                    current['owner_final_error'] = abs(yaw_delta(current['actor_yaw'], current['facing']['FinalYaw']))
            check(name + ':' + peer + ':历史与可见Yaw连续', (compared > 0 or is_cancel) and not violations, violations)
            finals = [row for row in observed if row['controlled'] and row['facing']['bFinal']]
            if is_cancel and cancel_snapshot:
                proof = cancel_identity_proof(observed, facing_identity(cancel_snapshot)[1:],
                                               observed[0]['peer'] == 'authority', cancel_snapshot)
                cancellation_observations.append({'case': name, 'peer': peer, **proof})
                check(name + ':' + peer + ':取消身份不复活并最终退出', not proof['failures'] and
                      proof['observation'] != 'still_observed', proof)
            if not is_cancel:
                check(name + ':' + peer + ':观察到同段权威结束', bool(finals))
            final_groups = {}
            for row in finals:
                key = facing_identity(row) + (int(row['facing']['Revision']),)
                final_groups.setdefault(key, []).append(row)
            for final_key, group in final_groups.items():
                last = group[-1]
                if last['peer'] == 'owner':
                    check(name + ':' + peer + ':Owner最终误差不超过1度',
                          abs(yaw_delta(last['actor_yaw'], last['facing']['FinalYaw'])) <= 1.)
                    identity = facing_identity(last)
                    revision = int(last['facing']['Revision'])
                    consumed = [event for event in native_events if event.get('Event') == 'Correction' and
                                (event.get('Avatar'), event.get('chain_key'), event.get('Segment')) == identity and
                                event.get('Revision') == revision]
                    check(name + ':' + peer + ':同段最终修订仅消费一次校正', len(consumed) == 1,
                          [event['line'] for event in consumed])
                    first_final_marker_before = None
                    if marker_links is not None:
                        first_final_marker_before = marker_links.get((group[0]['avatar'], group[0]['sample_index'] - 1))
                    eligible = [event for event in native_events if event.get('Event') == 'Step' and
                                (event.get('Avatar'), event.get('chain_key'), event.get('Segment')) == identity and
                                event.get('Revision') == revision and not event.get('Replay') and not event.get('Paused') and
                                event.get('Final') and
                                ((first_final_marker_before is not None and event['line'] > first_final_marker_before['line']) if
                                 marker_links is not None else event['WorldTime'] >= group[0]['world_time'] - 0.0000001)]
                    first_step = eligible[0] if eligible else None
                    check(name + ':' + peer + ':首个允许移动收尾已完成校正', bool(first_step) and len(consumed) == 1 and
                          consumed[0]['line'] <= first_step['line'] and
                          abs(yaw_delta(first_step['AfterYaw'], last['facing']['FinalYaw'])) <= 1.,
                          {'first_step': first_step['line'] if first_step else None, 'corrections': [value['line'] for value in consumed]})
                elif last['peer'] == 'proxy':
                    proof, settled = proxy_convergence_proof(observed, group)
                    convergence_observations.append({'case': name, 'peer': peer, 'identity': final_key, **proof})
                    check(name + ':' + peer + ':代理0.25秒内收敛或旧身份提前失效',
                          proof['verdict'] in ('converged', 'interrupted'), proof)
                    mesh_observations.append({'case': name, 'peer': peer, 'samples': len(settled),
                        'max_mesh_error_after_025s': max([abs(yaw_delta(row['mesh_world_yaw'],
                            row['facing']['FinalYaw'] + row['mesh_base_relative_yaw'])) for row in settled], default=None),
                        'note': 'Mesh单独记录，未用Actor验收替代动画或画面结论'})
        releases = [row for row in warmup.get('release_snapshots', []) + report.get('release_snapshots', []) if row['case'] == name]
        check(name + ':各端实际释放', len(releases) == case['peer_count'] and all(not row['controlled'] and
              row.get('rms') == 0. and row.get('animation_root') == 0. and row.get('playing_root') == 0. and
              not row.get('active_reaction_montages') and 'HIT_REACTION' not in row.get('rotation_driver', 'HIT_REACTION').upper()
              for row in releases), releases)
    native_failures = []
    consumed_revisions = set()
    native_response_keys = set()
    for event in native_events:
        if event.get('Event') == 'Step':
            delta = abs(event['normal_delta'])
            bound = event['MaxYawSpeed'] * event['EffectiveDeltaTime'] + .05
            difference = yaw_delta(event['BeforeYaw'], event['TargetYaw'])
            # 与生产CMC一致，恰好背向的最短路径固定取正Yaw，不能误判为反向推进。
            if abs(abs(difference) - 180.) <= .0001:
                difference = 180.
            if event['DeltaTime'] < 0. or event['EffectiveDeltaTime'] < 0. or \
                    event['EffectiveDeltaTime'] > event['DeltaTime'] + 0.000001 or delta > bound or \
                    delta > abs(difference) + .05 or event['normal_delta'] * difference < -.05 or \
                    ((event.get('Paused') or event.get('Hard')) and delta > .05):
                native_failures.append({'line': event['line'], 'delta': delta, 'bound': bound})
        elif event.get('Event') == 'NativeRejectedRestore':
            if event.get('NativeType') not in ('Rotation', 'RMS') or event['Role'] != 2 or \
                    not 1 <= event['Segment'] <= 0xffffffff or not 1 <= event['Revision'] <= 0xffffffff or event['chain_key'] == '0' * 32 or \
                    abs(yaw_delta(event['BeforeYaw'], event['AfterYaw'])) > .05:
                native_failures.append({'line': event['line'], 'reason': '拒绝回包只能恢复同身份原可见Yaw，不能产生新转向许可'})
        elif event.get('Event') == 'NativeCorrection':
            # 只有真实接受的新ACK诊断有此资格；不能用Final标记、未知类型或重复时间戳豁免。
            response_key = (event['Avatar'], event['MoveTimeStamp'])
            if event.get('NativeType') not in ('Rotation', 'RMS') or event['MoveTimeStamp'] < 0. or \
                    event['Role'] != 2 or not 1 <= event['Segment'] <= 0xffffffff or not 1 <= event['Revision'] <= 0xffffffff or \
                    event['chain_key'] == '0' * 32 or response_key in native_response_keys or \
                    abs(yaw_delta(event['AfterYaw'], event['NativeYaw'])) > .05:
                native_failures.append({'line': event['line'], 'reason': '原生响应类型、ACK身份或实际应用端点无效／重复'})
            native_response_keys.add(response_key)
        elif event.get('Event') in ('Correction', 'FrozenRestore'):
            key = (event['Avatar'], event['chain_key'], event['Segment'], event['Revision'])
            if event['Event'] == 'Correction':
                if key in consumed_revisions:
                    native_failures.append({'line': event['line'], 'reason': '同身份最终修订重复消费校正'})
                consumed_revisions.add(key)
            elif key not in consumed_revisions:
                native_failures.append({'line': event['line'], 'reason': '冻结恢复缺少该修订首次校正证据'})
            if abs(yaw_delta(event['AfterYaw'], event['FinalYaw'])) > 1.:
                native_failures.append({'line': event['line'], 'reason': '校正执行后未收敛到同段FinalYaw'})
        elif event.get('Event') == 'CurrentRestore':
            if abs(yaw_delta(event['AfterYaw'], event['AnchorYaw'])) > .05:
                native_failures.append({'line': event['line'], 'reason': '回放恢复未落在当前动作锚点'})
        elif event.get('Event') == 'HistoricalRestore':
            paired = [step for step in native_events if step.get('Event') == 'Step' and step.get('Replay') and
                      event['line'] in step.get('nested_correction_lines', [])]
            if abs(yaw_delta(event['AfterYaw'], event['RecordedYaw'])) > .05 or len(paired) != 1:
                native_failures.append({'line': event['line'], 'reason': '历史恢复端点或配对移动步证据不完整'})
    check('全部原生移动步和真实校正符合约束', not native_failures, native_failures)
    return {'passed': bool(checks) and all(item['passed'] for item in checks), 'checks': checks,
            'native_correction_observations': [event for event in native_events if event.get('Event') == 'NativeCorrection'],
            'native_rejected_restore_observations': [event for event in native_events if event.get('Event') == 'NativeRejectedRestore'],
            'native_event_count': len(native_events), 'sample_count': len(report.get('samples', [])),
            'mesh_observations': mesh_observations,
            'resolution_observations': resolution_observations, 'proxy_convergence_observations': convergence_observations,
            'cancellation_observations': cancellation_observations,
            'identity_boundary_observations': boundary_observations,
            'causal_marker_evidence_complete': markers_required and bool(all_rows) and not marker_failures,
            'causal_attribution_scope': 'sample_marker_line_order' if markers_required else 'legacy_world_time_only',
            'sample_marker_count': len(sample_markers or []),
            'warmup_observation': {'required': bool(report.get('requires_warmup')), 'completed': warmup.get('completed'),
                                   'action_count': len(warmup.get('cases', [])), 'sample_count': len(warmup.get('samples', [])),
                                   'formal_pass_credit': False, 'note': '预热仅排除加载干扰；原始冷场景失败与预热全部样本保留，不证明无冷启动卡顿'},
            'coverage_limitations': report.get('coverage_limitations', [])}


def run_runtime(options):
    """独占编辑器中启动采集；根启动器在退出后合并原生日志并执行离线验收。"""
    import builtins
    import os
    import unreal

    expected_pid = options.get('HIT_FACING_EXCLUSIVE_EDITOR_PID')
    command_line = unreal.SystemLibrary.get_command_line()
    assert '-HodgeCombatValidationExclusive' in command_line.split() and type(expected_pid) is int and expected_pid == os.getpid(), '受击朝向采集要求根启动器专用编辑器及精确PID'
    dirty = list(unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()) + list(unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages())
    assert not dirty and not unreal.EditorLevelLibrary.get_pie_worlds(True), '启动前有脏包或已有PIE，拒绝重载'
    sys.path.insert(0, str(Path(unreal.Paths.project_dir()) / 'Tools/HitReactionDummy'))
    import control

    mode = options.get('HIT_FACING_MODE', 'single')
    lag = int(options.get('HIT_FACING_LAG_MS', 0))
    player_target = bool(options.get('HIT_FACING_PLAYER_TARGET', False))
    assert mode in ('single', 'listen', 'dedicated') and lag in (0, 100)
    npc_source = mode == 'single' and player_target
    names = ['SideLight', 'BackHeavy', 'BackLaunch', 'ContactNoImpact', 'SameLevel', 'Upgrade', 'AirFollow', 'HardLock', 'Cancel', 'FullChain']
    requested = options.get('HIT_FACING_CASES', names)
    assert requested and len(set(requested)) == len(requested) and all(name in names for name in requested)
    # 预热沿用真实攻击路径，单独留证；正式场景列表及玩法参数保持原值。
    warmup_steps = [{'name': 'WarmupHitStun', 'scenario': 'SideLight', 'probe_kind': 'HitStun'},
                    {'name': 'WarmupKnockback', 'scenario': 'BackHeavy', 'probe_kind': 'Knockback'},
                    {'name': 'WarmupLaunch', 'scenario': 'BackLaunch', 'probe_kind': 'Launch'}]
    steps = warmup_steps + [{'name': name, 'scenario': name} for name in requested]
    output = Path(unreal.Paths.project_saved_dir()) / options.get('HIT_FACING_REPORT_DIR', 'HitReactionFacing/Unassigned')
    output.mkdir(parents=True, exist_ok=False)
    editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    play = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.LevelEditorPlaySettings'))
    performance = unreal.get_default_object(unreal.load_class(None, '/Script/UnrealEd.EditorPerformanceSettings'))
    old_preferences = {key: play.get_editor_property(key) for key in ['PlayNumberOfClients', 'RunUnderOneProcess']}
    try:
        old_mode = int(play.get_editor_property('PlayNetMode').value)
    except TypeError as error:
        old_mode = int(re.search(r"Cannot pythonize '(\d+)'", str(error)).group(1))
    old_throttle = performance.get_editor_property('bThrottleCPUWhenNotForeground')
    lib = unreal.HodgeCombatValidationLibrary
    assert lib.configure_validation_pie(1 if mode == 'single' else 2, mode == 'dedicated')
    performance.set_editor_property('bThrottleCPUWhenNotForeground', False)
    assert unreal.EditorLoadingAndSavingUtils.load_map('/Game/Tests/Combat/L_HitReactionDummy')
    report = {'mode': mode, 'lag_ms': lag, 'player_target': player_target, 'requested_cases': list(requested),
              'cases': [], 'samples': [], 'hit_events': [], 'release_snapshots': [], 'error': None,
              'cleanup_complete': False, 'collection_only': True, 'requires_native_resolutions': True, 'requires_sample_markers': True,
              'input': '既有EnemyCombo真实意图请求 -> GAS -> 实际Melee Notify/GE' if npc_source else '真实装备授予 -> 原生能力队列 -> 实际Melee Notify/GE；非物理键盘',
              'submitted_requests': [], 'requires_warmup': True, 'requires_recovery_scope_proof': True,
              'ground_recovery_scopes': [],
              'warmup': {'requested_actions': [step['name'] for step in warmup_steps], 'cases': [], 'samples': [],
                         'hit_events': [], 'release_snapshots': [], 'submitted_requests': [], 'actions': [], 'completed': False, 'error': None,
                         'purpose': '真实加载预热，不计入正式场景通过；冷启动失败证据不删除'},
              'fixture_parameters': {'SameLevel': {'request_control_duration': 1., 'input_mode': 'ExplicitImpact',
                                                    'hero_ground_recovery_isolated_in_memory': player_target,
                                                    'scope': '仅证明1s专用夹具的成功同级保持，不证明正式0.4s连打时序'},
                                     'Upgrade': {'initial_request_control_duration': 1., 'input_modes': 'Explicit HitStun -> Explicit Knockback',
                                                 'hero_ground_recovery_isolated_in_memory': player_target,
                                                 'scope': '确定性控制窗内Light到Heavy接管，最终None等级合法映射，不证明正式Poise连打时序'}},
              'coverage_limitations': ['多攻击者、无效来源、死亡换Pawn、时间缩放及动画暂停本脚本未覆盖',
                                       '真实SavedMove回放由全量原生日志与原生回归另证', 'Mesh组件Yaw不等于骨骼动作或画面审美验收',
                                       '反射状态不含冻结来源位置；真实PIE验角度上限，精确源方向clamp由FacingCandidate.FrozenXYAndAngles原生回归另证']}
    state = {'phase': 'boot', 'index': 0, 'wall': time.monotonic(), 'saved_hits': {}, 'saved_notifies': {},
             'modified_montages': set(), 'mesh_bases': {}, 'peer_indices': {}, 'profile_restore_paths': set()}
    builtins.HODGE_HIT_FACING_RUNTIME_SCOPE = {'report': report, 'state': state}

    def current_step():
        """统一定位当前预热或正式步骤，避免预热冒用正式场景身份。"""
        return steps[state['index']]

    def evidence():
        """原始预热动作、采样及结算独立保存，不混进正式报告数组。"""
        return report['warmup'] if state['index'] < len(warmup_steps) else report

    def restore_recovery_fixture(reason, at=None):
        """成功与异常共用同一还原入口，只有原值读回成功才清除待还原令牌。"""
        restore_ground_recovery_scope(state.get('recovery_scope'), reason, at)
        state.pop('recovery_scope', None)

    def begin_recovery_fixture(at):
        """沿用现有控制链专项隔离，仅Hero的同级和升级用例关闭内存自动恢复。"""
        step = current_step()
        if not player_target or step['name'] not in ('SameLevel', 'Upgrade'):
            return
        assert not state.get('recovery_scope'), '前一专项尚未完成恢复，拒绝建立新覆盖'
        profile = state['reaction'].get_profile()
        before = profile.get_editor_property('bEnableGroundRecovery')
        assert profile.get_path_name() == HERO_RECOVERY_PROFILE and before is True, '专项前正式Hero恢复配置与基线不符'
        record = {'case': step['name'], 'profile': profile.get_path_name(), 'before_enabled': before,
                  'entered_at': at, 'restored': False}
        report['ground_recovery_scopes'].append(record)
        state['recovery_scope'] = {'profile': profile, 'record': record}
        state['profile_restore_paths'].add(profile.get_path_name())
        profile.set_editor_property('bEnableGroundRecovery', False)
        record['override_enabled'] = profile.get_editor_property('bEnableGroundRecovery')
        assert record['override_enabled'] is False, '专项隔离开关未实际生效'

    def clone(value):
        """临时反射结构按值复制，不通过正式资产保存测试配置。"""
        copied = type(value)()
        copied.import_text(value.export_text())
        return copied

    def parse_npc_transition(serialized):
        """用本机反射结构解析表格文本，避免将嵌套结构字符串误当作JSON对象。"""
        transition = unreal.HodgeComboTransition()
        transition.import_text(serialized)
        tag = transition.get_editor_property('TriggerInputIntentTag')
        return tag.export_text() if unreal.GameplayTagLibrary.is_gameplay_tag_valid(tag) else None

    def world_targets(world):
        """按真实PlayerId关联同一玩家，敌人按唯一假人实例识别。"""
        values = unreal.GameplayStatics.get_all_actors_of_class(world, state['target_class'])
        if player_target:
            values = [actor for actor in values if actor.get_editor_property('PlayerState') and
                      actor.get_editor_property('PlayerState').get_editor_property('PlayerId') == state['player_id']]
        return values

    def check(name, condition, detail=None):
        """运行失败保留此前样本与已执行断言，不能继续下一场景掩盖错误。"""
        state['checks'].append({'name': name, 'passed': bool(condition), 'detail': detail})
        assert condition, (current_step()['name'], name, detail)

    def cancel_npc_request():
        """每次探针与退出前关闭本脚本拥有的真实请求，不取消其他角色的能力。"""
        request_id = state.pop('npc_request_id', None)
        if request_id is not None and state.get('npc'):
            state['npc'].get_component_by_class(unreal.HodgeCombatComponentBase).cancel_combat_action_request(request_id)

    def submit_npc_probe():
        """复用成熟NPC的共享意图入口，保留其Guardian骨架Montage与真实命中检测。"""
        cancel_npc_request()
        source, target = state['source'], state['target']
        offset = target.get_actor_location() - source.get_actor_location()
        assert offset.x * offset.x + offset.y * offset.y < 330. ** 2, 'NPC目标超出原有近战探针范围'
        yaw = math.degrees(math.atan2(offset.y, offset.x))
        source.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
        source.get_controller().set_control_rotation(unreal.Rotator(yaw=yaw))
        request = source.get_component_by_class(unreal.HodgeCombatComponentBase).submit_combat_intent(state['npc_intent'])
        state['npc_request_id'] = request.request_id
        evidence()['submitted_requests'].append({'case': current_step()['name'], 'request_id': request.request_id.export_text(),
                                              'status': str(request.status), 'reason': str(request.reason)})
        assert 'REJECTED' not in str(request.status).upper(), '既有NPC意图请求被拒绝: ' + request.export_text()

    def configure_probe(kind, amount, explicit_hit_stun=False):
        """只临时配置独立探针，真实正伤害与分段资格仍由正式Notify/GE裁决。"""
        probe = unreal.load_asset(control.ROOT + '/DA_Probe_' + kind)
        definition = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Validation/DA_ComboAttack') if npc_source else probe
        assert definition and probe, '真实攻击定义或既有探针定义不存在'
        path = definition.get_path_name()
        if path not in state['saved_hits']:
            state['saved_hits'][path] = definition.get_editor_property('DefaultHitConfig').export_text()
        hit = clone(definition.get_editor_property('DefaultHitConfig'))
        baseline = clone(probe.get_editor_property('DefaultHitConfig'))
        if probe.get_path_name() in state['saved_hits']:
            baseline.import_text(state['saved_hits'][probe.get_path_name()])
        request = clone(baseline.get_editor_property('Reaction'))
        request.set_editor_property('InputMode', unreal.HodgeReactionInputMode.POISE_DRIVEN if kind == 'HitStun' and not explicit_hit_stun else unreal.HodgeReactionInputMode.EXPLICIT_IMPACT)
        # 显式效果沿用互斥作者契约，不能携带仅削韧模式生效的数值。
        request.set_editor_property('ReservedPoiseDamage', amount if kind == 'HitStun' and not explicit_hit_stun else 0.)
        request.set_editor_property('PresentationSide', unreal.HodgeHitSide.RIGHT)
        if explicit_hit_stun:
            impacts = request.get_editor_property('Impacts')
            assert kind == 'HitStun' and len(impacts) == 1 and impacts[0].get_editor_property('Type') == unreal.HodgeImpactType.HIT_STUN, '既有同级探针必须提供单个HitStun'
            impact = clone(impacts[0])
            impact.set_editor_property('ControlDuration', 1.)
            request.set_editor_property('Impacts', [impact])
        elif kind == 'HitStun':
            request.set_editor_property('Impacts', [])
        hit.set_editor_property('Reaction', request)
        hit.set_editor_property('DamageMultiplier', .01)
        definition.set_editor_property('DefaultHitConfig', hit)
        montage = definition.get_editor_property('ExecutionConfig').montage
        state['modified_montages'].add(montage.get_path_name())
        found = 0
        for event in unreal.AnimationLibrary.get_animation_notify_events(montage):
            if isinstance(event.notify, unreal.HodgeAnimNotify_Hit):
                found += 1
                notify = event.notify
                if notify.get_path_name() not in state['saved_notifies']:
                    state['saved_notifies'][notify.get_path_name()] = (notify, notify.get_editor_property('Hit').export_text())
                value = clone(notify.get_editor_property('Hit'))
                value.set_editor_property('bCanLaunch', kind == 'Launch')
                value.set_editor_property('bUseDefaultReaction', True)
                if player_target:
                    value.set_editor_property('bAllowFriendlyFire', True)
                notify.set_editor_property('Hit', value)
        assert found, '实际攻击Montage缺少HodgeAnimNotify_Hit: ' + montage.get_path_name()
        action = {'case': current_step()['name'], 'kind': kind, 'definition': definition.get_path_name(),
                  'montage': montage.get_path_name(), 'time': unreal.GameplayStatics.get_time_seconds(state['target']),
                  'hit_config': hit.export_text(), 'dispatch_returned': False,
                  'source_position': str(state['source'].get_actor_location()), 'target_position': str(state['target'].get_actor_location())}
        evidence().setdefault('actions', []).append(action)
        if npc_source:
            # 记录既有NPC通知与站位，空中追加未命中时不据请求Started推断检测已覆盖。
            evidence().setdefault('npc_probe_inputs', []).append({'case': current_step()['name'], 'kind': kind,
                'time': unreal.GameplayStatics.get_time_seconds(state['target']),
                'source_position': str(state['source'].get_actor_location()), 'target_position': str(state['target'].get_actor_location()),
                'target_velocity': str(state['target'].get_velocity()), 'hit_config': hit.export_text(),
                'notify_inputs': [{'time': unreal.AnimationLibrary.get_anim_notify_event_trigger_time(event),
                                   'hit': event.notify.get_editor_property('Hit').export_text()}
                                  for event in unreal.AnimationLibrary.get_animation_notify_events(montage)
                                  if isinstance(event.notify, unreal.HodgeAnimNotify_Hit)]})
        state.update(probe_kind=kind, probe_can_launch=kind == 'Launch')
        if npc_source:
            submit_npc_probe()
        else:
            control.probe(kind)
        action['dispatch_returned'] = True

    def record_polled_result(result):
        """轮询只驱动测试步骤；真实结算阶段由离线唯一关联的原生诊断证明。"""
        reaction, target = state['reaction'], state['target']
        plan, facing = reaction.get_reaction_state(), reaction.get_facing_state()
        ids = parse_facing_uint32_fields(facing.export_text())
        pose = lib.inspect_montage_state(target, plan.montage) if plan.montage else None
        at = unreal.GameplayStatics.get_time_seconds(target)
        evidence()['hit_events'].append({'case': current_step()['name'], 'avatar': target.get_path_name(),
            'time': at, 'hit_id': result.hit_id.export_text(), 'damage': result.actual_damage,
            'outcome': str(result.outcome), 'level': str(result.reaction_level), 'collected_by': 'SlatePoll',
            'phase_observed': str(plan.phase), 'controlled_observed': reaction.is_controlled(),
            'falling_observed': target.character_movement.is_falling(), 'sequence_observed': plan.sequence,
            'chain_started_observed': plan.chain_started_at, 'facing_chain_observed': guid_key(facing.get_editor_property('ChainId').export_text()),
            'facing_segment_observed': ids['Segment'], 'facing_revision_observed': ids['Revision'],
            'montage_observed': plan.montage.get_path_name() if plan.montage else None,
            'montage_instance_observed': pose.instance_id if pose else -1,
            'probe_kind': state['probe_kind'], 'can_launch_observed': state['probe_can_launch']})
        if result.actual_damage > 0.:
            state['first_hit'] = state['first_hit'] or at

    def sample(worlds):
        """每个Slate后帧保留全部端样本，Actor与实际Mesh世界Yaw分别记录。"""
        state['frame_index'] = state.get('frame_index', -1) + 1
        for world in worlds:
            values = world_targets(world)
            if len(values) != 1:
                continue
            target = values[0]
            reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
            facing = reaction.get_facing_state()
            fields = {key: facing.get_editor_property(key) for key in ['ChainId', 'Preset', 'StartedAt', 'EndsAt',
                      'StartYaw', 'TargetYaw', 'MaxYawSpeed', 'FinalYaw', 'bHasTarget', 'bFinal', 'bHardLocked', 'bRetired']}
            fields.update(parse_facing_uint32_fields(facing.export_text()))
            fields['ChainId'] = guid_key(fields['ChainId'].export_text())
            fields['Preset'] = str(fields['Preset'])
            peer = 'authority' if unreal.SystemLibrary.is_server(world) else 'owner' if target.is_locally_controlled() else 'proxy'
            key = world.get_path_name() + '|' + target.get_path_name()
            state['peer_indices'][key] = state['peer_indices'].get(key, -1) + 1
            state['mesh_bases'].setdefault(key, target.mesh.get_editor_property('RelativeRotation').yaw)
            movement = lib.inspect_reaction_motion(target)
            plan = reaction.get_reaction_state()
            row = {'case': current_step()['name'], 'world': world.get_path_name(), 'avatar': target.get_path_name(),
                   'peer': peer, 'peer_key': key, 'sample_index': state['peer_indices'][key], 'frame_index': state['frame_index'],
                   'world_time': unreal.GameplayStatics.get_time_seconds(world),
                   'server_time': unreal.GameplayStatics.get_game_state(world).get_server_world_time_seconds(),
                   'actor_yaw': target.get_actor_rotation().yaw, 'mesh_world_yaw': target.mesh.get_world_rotation().yaw,
                   'mesh_relative_yaw': target.mesh.get_editor_property('RelativeRotation').yaw, 'mesh_base_relative_yaw': state['mesh_bases'][key],
                   'paused': target.mesh.get_editor_property('bPauseAnims') or target.mesh.get_editor_property('GlobalAnimRateScale') <= 0. or target.get_editor_property('CustomTimeDilation') <= 0.,
                   'facing': fields, 'controlled': reaction.is_controlled(),
                   'sequence': plan.sequence, 'phase': str(plan.phase), 'level': str(plan.reaction_level),
                   'motion_velocity': [plan.motion_velocity.x, plan.motion_velocity.y, plan.motion_velocity.z],
                   'position': [target.get_actor_location().x, target.get_actor_location().y, target.get_actor_location().z],
                   'velocity': [target.get_velocity().x, target.get_velocity().y, target.get_velocity().z],
                   'rms': movement.y, 'animation_root': movement.x, 'playing_root': movement.z,
                   'montage': plan.montage.get_path_name() if plan.montage else None}
            if plan.montage:
                pose = lib.inspect_montage_state(target, plan.montage)
                anim = target.mesh.get_anim_instance()
                rate = anim.montage_get_play_rate(plan.montage) if anim else None
                row.update(montage_instance=pose.instance_id, montage_position=pose.position, montage_weight=pose.weight,
                           montage_playing=pose.playing, montage_play_rate=rate)
                row['paused'] = row['paused'] or pose.instance_id >= 0 and (not pose.playing or rate == 0.)
            # 只读既有反射字段，用于建立或释放边界失败时追踪最终驱动和动作身份。
            rotation = target.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state()
            row['rotation_driver'] = str(rotation.get_editor_property('Driver'))
            row['rotation_action_request'] = rotation.get_editor_property('ActionRequestId').export_text()
            profile = reaction.get_profile()
            row['recovery_profile'] = profile.get_path_name()
            row['ground_recovery_enabled'] = profile.get_editor_property('bEnableGroundRecovery')
            evidence()['samples'].append(row)
            # 标记位于本端快照末尾；日志行序保留发生在读取之后的移动，不按世界时猜前后帧。
            row['engine_frame'] = int(unreal.SystemLibrary.get_frame_count())
            unreal.log('HitFacingSample Avatar=' + row['avatar'] + ' Chain=' + fields['ChainId'] +
                       ' Segment=' + str(fields['Segment']) + ' Revision=' + str(fields['Revision']) +
                       ' sample_index=' + str(row['sample_index']) + ' engine_frame=' + str(row['engine_frame']) +
                       ' Yaw=' + format(row['actor_yaw'], '.17g') + ' WorldTime=' + format(row['world_time'], '.17g'))

    def finish(error=None):
        """先恢复临时探针和标签，再关闭PIE；不保存任何正式资产。"""
        if state['phase'] == 'closing':
            return
        state.update(phase='closing', closing_at=time.monotonic())
        if report['error'] is None:
            report['error'] = error
        if state['index'] < len(warmup_steps):
            report['warmup']['error'] = error
        if state.get('checks'):
            evidence()['unfinished_checks'] = state['checks']

        def cleanup(label, operation):
            """清理失败独立留证，仍执行后续恢复和退出，不覆盖原场景首因。"""
            try:
                operation()
            except Exception:
                report.setdefault('cleanup_errors', []).append({'operation': label, 'traceback': traceback.format_exc()})
                if report['error'] is None:
                    report['error'] = '清理失败: ' + label

        cleanup('restore_ground_recovery_fixture', lambda: restore_recovery_fixture('cleanup'))
        if state.get('hard_tags'):
            cleanup('remove_hard_lock', lambda: unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(state['target'], state.pop('hard_tags'), True))
        cleanup('cancel_npc_request', cancel_npc_request)
        for path, original in state['saved_hits'].items():
            def restore_hit():
                """每份原始攻击结构独立恢复，失败不阻止其他包清理。"""
                definition = unreal.load_asset(path)
                value = clone(definition.get_editor_property('DefaultHitConfig'))
                value.import_text(original)
                definition.set_editor_property('DefaultHitConfig', value)
            cleanup('restore_hit:' + path, restore_hit)
        for notify, original in state['saved_notifies'].values():
            def restore_notify():
                """只恢复脚本临时写入的命中通知配置，不保存资产。"""
                value = clone(notify.get_editor_property('Hit'))
                value.import_text(original)
                notify.set_editor_property('Hit', value)
            cleanup('restore_notify:' + notify.get_path_name(), restore_notify)
        if not npc_source:
            cleanup('stop_control_probe', control.stop)
        elif state.get('npc'):
            cleanup('destroy_npc_fixture', lambda: state['npc'].destroy_actor())
        for world in unreal.EditorLevelLibrary.get_pie_worlds(True):
            if lag:
                cleanup('reset_packet_lag:' + world.get_path_name(), lambda: unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=0'))
        (output / 'warmup.json').write_text(json.dumps(report['warmup'], ensure_ascii=False, indent=2), encoding='utf-8')
        (output / 'runtime.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        cleanup('request_end_play', editor.editor_request_end_play)

    def tick(delta):
        """所有等待有界；仅真实命中前布置目标Yaw，受击后不写目标旋转或计划。"""
        try:
            worlds = unreal.EditorLevelLibrary.get_pie_worlds(True)
            if state['phase'] == 'closing':
                if worlds:
                    assert time.monotonic() - state['closing_at'] < 12., '清理后PIE未在有界时间内退出'
                    return
                assert lib.configure_validation_pie(1 if old_mode == 0 else 2, old_mode == 2)
                for key, value in old_preferences.items():
                    play.set_editor_property(key, value)
                performance.set_editor_property('bThrottleCPUWhenNotForeground', old_throttle)
                for path in list(state['saved_hits']) + sorted(state['modified_montages']) + sorted(state['profile_restore_paths']):
                    definition = unreal.load_asset(path)
                    unreal.EditorLoadingAndSavingUtils.reload_packages([definition.get_outermost()], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
                report['cleanup_complete'] = not report.get('cleanup_errors')
                report['dirty_after'] = [package.get_path_name() for package in list(unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()) + list(unreal.EditorLoadingAndSavingUtils.get_dirty_map_packages())]
                if report['dirty_after'] and report['error'] is None:
                    report['error'] = '清理后仍有脏包'
                (output / 'runtime.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
                unreal.unregister_slate_post_tick_callback(state['callback'])
                print('HIT_FACING_RUNTIME_CLEANUP ' + str(report['error']))
                if options.get('HIT_FACING_QUIT_EDITOR', True):
                    unreal.SystemLibrary.quit_editor()
                return
            assert time.monotonic() - state['wall'] < 170., '补转采集整体超时'
            server = next((world for world in worlds if unreal.SystemLibrary.is_server(world)), None)
            if not server:
                return
            now = unreal.GameplayStatics.get_time_seconds(server)
            if state['phase'] == 'boot':
                if len(worlds) != (1 if mode == 'single' else 3 if mode == 'dedicated' else 2):
                    return
                source = unreal.GameplayStatics.get_player_pawn(server, 0)
                if not isinstance(source, unreal.HodgeHeroCharacter) or not source.get_hodge_ability_system_component():
                    return
                if player_target:
                    target = unreal.GameplayStatics.get_player_pawn(server, 0 if npc_source else 1)
                    if not isinstance(target, unreal.HodgeHeroCharacter) or not target.get_hodge_ability_system_component():
                        return
                    assert target.get_hodge_ability_system_component().get_owner() == target.get_editor_property('PlayerState')
                    state['player_id'] = target.get_editor_property('PlayerState').get_editor_property('PlayerId')
                    target_class = unreal.HodgeHeroCharacter
                    for dummy in unreal.GameplayStatics.get_all_actors_of_class(server, unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()):
                        dummy.set_actor_location(unreal.Vector(5000, 5000, 95), False, True)
                    if npc_source:
                        if not state.get('npc'):
                            blueprint = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Validation/BP_EnemyCombo')
                            assert blueprint, '既有EnemyCombo测试夹具不存在'
                            state['npc'] = lib.spawn_hit_reaction_validation_actor(server, blueprint.generated_class(), unreal.Vector(850, 0, 100))
                            assert state['npc'], '既有EnemyCombo生成失败'
                        source = state['npc']
                        if not source.get_enemy_init_component().is_gameplay_ready():
                            return
                        brain = source.get_controller().get_editor_property('BrainComponent')
                        assert brain, '真实NPC的BrainComponent尚未初始化'
                        brain.stop_logic('HitFacing deterministic real-attack probe')
                        source.get_controller().stop_movement()
                        table = unreal.load_asset('/Game/Main/Character/Enemies/Guardian/Validation/DT_EnemyCombo')
                        rows = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(table))
                        intent = unreal.GameplayTag()
                        intent.import_text(unique_npc_intent_export(rows, parse_npc_transition))
                        state['npc_intent'] = intent
                        assert source.get_hodge_ability_system_component() and source != target, 'NPC攻击源必须具有原有完整ASC且区别于玩家'
                else:
                    target_class = unreal.load_asset(control.ROOT + '/BP_HitReactionDummy').generated_class()
                    targets = unreal.GameplayStatics.get_all_actors_of_class(server, target_class)
                    if len(targets) != 1 or not targets[0].initialize_enemy_ability_system():
                        return
                    target = targets[0]
                reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
                if not reaction.get_profile():
                    return
                assert reaction.get_profile().get_editor_property('Facing').get_editor_property('bEnabled'), '目标正式Profile未获批准启用，采集器不会自行开启'
                report['ground_recovery_baseline'] = {'profile': reaction.get_profile().get_path_name(),
                    'enabled': reaction.get_profile().get_editor_property('bEnableGroundRecovery')}
                if not npc_source:
                    control.start(target if player_target else None)
                state.update(source=source, target=target, reaction=reaction, target_class=target_class, phase='reset')
                for world in worlds:
                    if lag:
                        unreal.SystemLibrary.execute_console_command(world, 'Net PktLag=' + str(lag))
            if state['phase'] == 'reset':
                restore_recovery_fixture('reset', now)
                if npc_source:
                    cancel_npc_request()
                    state['reaction'].cancel_reaction()
                    for actor, position in [(state['source'], unreal.Vector(0, 0, 95)), (state['target'], unreal.Vector(180, 0, 95))]:
                        actor.character_movement.stop_movement_immediately()
                        actor.character_movement.set_movement_mode(unreal.MovementMode.MOVE_WALKING)
                        actor.set_actor_location(position, False, True)
                    state['source'].set_actor_rotation(unreal.Rotator(yaw=0.), False)
                else:
                    control.reset()
                yaw = 90. if current_step()['scenario'] in ('SideLight', 'ContactNoImpact', 'SameLevel', 'Upgrade') else 0.
                for world in worlds:
                    for target in world_targets(world):
                        target.set_actor_rotation(unreal.Rotator(yaw=yaw), False)
                        if target.get_controller():
                            target.get_controller().set_control_rotation(unreal.Rotator(yaw=yaw))
                state.update(phase='settle', at=now, initial_yaw=yaw, checks=[], seen_hits={state['reaction'].get_last_result().hit_id.export_text()},
                             first_hit=None, followup=False, hard_applied=False, hard_removed=False, canceled=False, cancel_snapshot=None, full_stage=1,
                             npc_approach_started=False,
                             samples_from=len(evidence()['samples']), events_from=len(evidence()['hit_events']))
                return
            if state['phase'] == 'settle':
                if now - state['at'] < 1.8:
                    return
                name = current_step()['scenario']
                state['initial_yaw'] = state['target'].get_actor_rotation().yaw
                expected_offset = abs(yaw_delta(state['initial_yaw'], 180.))
                check('命中前侧背起始方向有效', expected_offset > 30., expected_offset)
                begin_recovery_fixture(now)
                configure_probe(current_step().get('probe_kind', 'Launch' if name in ('BackLaunch', 'AirFollow') else 'HitStun'),
                                60. if name in ('BackHeavy', 'HardLock') else 1. if name == 'FullChain' else 20.,
                                explicit_hit_stun=name in ('SameLevel', 'Upgrade'))
                state.update(phase='observe', at=now)
            if state['phase'] != 'observe':
                return
            name = current_step()['scenario']
            result = state['reaction'].get_last_result()
            identity = result.hit_id.export_text()
            if identity not in state['seen_hits'] and identity != unreal.Guid().export_text():
                state['seen_hits'].add(identity)
                record_polled_result(result)
            sample(worlds)
            current = next(row for row in reversed(evidence()['samples']) if row['peer'] == 'authority' and row['frame_index'] == state['frame_index'])
            events = evidence()['hit_events'][state['events_from']:]
            if state['first_hit'] is not None:
                age = now - state['first_hit']
                if name in ('ContactNoImpact', 'SameLevel', 'Upgrade') and age >= (0. if npc_source else .04) and not state['followup']:
                    configure_probe('Knockback' if name == 'Upgrade' else 'HitStun', 1., explicit_hit_stun=name == 'SameLevel')
                    state['followup'] = True
                # NPC地面盒沿用原通知；下降阶段发出真实追加，结算仍须由原生证据证明离地。
                if name == 'AirFollow' and not state['followup'] and state['target'].character_movement.is_falling() and \
                        (not npc_source or age >= .8 and state['target'].get_velocity().z < 0.):
                    configure_probe('AirHit', 0.)
                    state['followup'] = True
                if name == 'FullChain' and len(events) >= state['full_stage'] and state['full_stage'] < 4:
                    # 原NPC地面盒短于击退距离，使用既有AI移动靠近后再出刀，不移动或重置目标。
                    if npc_source and state['full_stage'] == 3:
                        controller = state['source'].get_controller()
                        if not state['npc_approach_started']:
                            cancel_npc_request()
                            request = controller.move_to_actor(state['target'], 100., False, False, False, None, True)
                            evidence().setdefault('npc_approach', []).append({'time': now, 'request': str(request)})
                            check('NPC既有直线接近请求可执行', 'FAILED' not in str(request).upper(), str(request))
                            state['npc_approach_started'] = True
                        offset = state['target'].get_actor_location() - state['source'].get_actor_location()
                        ready = offset.x * offset.x + offset.y * offset.y <= 175. ** 2 and \
                            abs(state['target'].get_velocity().x) + abs(state['target'].get_velocity().y) < 1.
                        if ready:
                            controller.stop_movement()
                    else:
                        ready = True
                    if ready:
                        kind, amount = [('HitStun', 20.), ('HitStun', 40.), ('Launch', 0.)][state['full_stage'] - 1]
                        configure_probe(kind, amount)
                        state['full_stage'] += 1
                if name == 'HardLock' and not state['hard_applied']:
                    tags = unreal.GameplayTagContainer()
                    tags.import_text('(GameplayTags=((TagName="Status.Rotation.Locked")))')
                    assert unreal.AbilitySystemLibrary.add_loose_gameplay_tags(state['target'], tags, True)
                    state.update(hard_tags=tags, hard_applied=True, hard_at=now)
                if name == 'HardLock' and state['hard_applied'] and not state['hard_removed'] and now - state['hard_at'] >= .06:
                    unreal.AbilitySystemLibrary.remove_loose_gameplay_tags(state['target'], state.pop('hard_tags'), True)
                    state['hard_removed'] = True
                if name == 'Cancel' and not state['canceled'] and current['controlled'] and current['facing']['bHasTarget'] and \
                        not current['facing']['bFinal'] and current['server_time'] < current['facing']['EndsAt'] and \
                        abs(yaw_delta(current['facing']['StartYaw'], current['actor_yaw'])) > 1.:
                    state['cancel_snapshot'] = current
                    state['reaction'].cancel_reaction()
                    state['canceled'] = True
            if now - state['at'] < (10. if name == 'FullChain' else 7. if name in ('BackLaunch', 'AirFollow') else 3.5):
                return
            rows = evidence()['samples'][state['samples_from']:]
            server_rows = [row for row in rows if row['peer'] == 'authority' and row['controlled']]
            check('真实服务器受击有控制样本', bool(server_rows))
            check('原地动画不施加动画根运动', all(not row['animation_root'] and not row['playing_root'] for row in rows))
            expected_presets = ['LIGHT', 'HEAVY', 'LAUNCH'] if name == 'FullChain' else ['LIGHT', 'HEAVY'] if name == 'Upgrade' else ['LAUNCH'] if name in ('BackLaunch', 'AirFollow') else ['HEAVY'] if name in ('BackHeavy', 'HardLock') else ['LIGHT']
            for preset in expected_presets:
                matching = [row['facing'] for row in server_rows if preset in row['facing']['Preset'].upper() and row['facing']['bHasTarget']]
                angle = (30. if player_target else 45.) if preset == 'LIGHT' else (60. if player_target else 90.)
                window = .08 if preset == 'LIGHT' else (.12 if player_target else .14)
                check('实际最终计划使用' + preset + '和批准参数', bool(matching) and all(
                    facing_preset_matches(value, angle, window) for value in matching))
            segment_ids = {(row['facing']['ChainId'], row['facing']['Segment']) for row in server_rows if row['facing']['Segment']}
            if name in ('ContactNoImpact', 'SameLevel', 'AirFollow'):
                check('同级或空中追加不重开补转段', state['followup'] and len(segment_ids) == 1, sorted(segment_ids))
            if name == 'Upgrade':
                check('升级实际建立新补转段', state['followup'] and len(segment_ids) == 2, sorted(segment_ids))
            check('轮询观察到本场景全部真实正伤害', len(events) == (4 if name == 'FullChain' else 2 if name in
                  ('ContactNoImpact', 'SameLevel', 'Upgrade', 'AirFollow') else 1) and all(event['damage'] > 0. for event in events), events)
            if name == 'Cancel':
                check('实际转动后仍在活动窗内取消', state['canceled'] and state['cancel_snapshot'] is not None, state['cancel_snapshot'])
            if name == 'HardLock':
                locked = [row for row in server_rows if row['facing']['bHardLocked']]
                check('硬锁建立且移除后本段仍永久冻结', state['hard_removed'] and bool(locked) and all(
                    abs(yaw_delta(row['actor_yaw'], locked[0]['facing']['FinalYaw'])) <= .05 for row in locked), locked[-1:] if locked else [])
            if name in ('BackLaunch', 'AirFollow', 'FullChain'):
                check('真实离地落地并经过起身', any('AIRBORNE' in row['phase'] for row in server_rows) and
                      any('DOWNED' in row['phase'] and row['velocity'][2] <= .05 for row in server_rows) and
                      any('GETTING_UP' in row['phase'] or row['montage'] and 'StandUp' in row['montage'] for row in server_rows))
            for world in worlds:
                targets = world_targets(world)
                check('每端目标仍唯一', len(targets) == 1, world.get_path_name())
                target = targets[0]
                reaction = target.get_component_by_class(unreal.HodgeHitReactionComponent)
                motion = lib.inspect_reaction_motion(target)
                used = {row['montage'] for row in rows if row['avatar'] == target.get_path_name() and row['montage']}
                active = [{'montage': path, 'instance_id': pose.instance_id, 'weight': pose.weight, 'playing': pose.playing}
                          for path in sorted(used) for pose in lib.inspect_montage_states(target, unreal.load_asset(path))
                          if pose.instance_id >= 0 and (pose.playing or pose.weight > .001)]
                driver = str(target.get_component_by_class(unreal.HodgeCharacterRotationComponent).get_resolved_state().driver)
                released = {'case': current_step()['name'], 'world': world.get_path_name(), 'avatar': target.get_path_name(),
                            'controlled': reaction.is_controlled(), 'rms': motion.y, 'animation_root': motion.x,
                            'playing_root': motion.z, 'active_reaction_montages': active, 'rotation_driver': driver}
                evidence()['release_snapshots'].append(released)
                check('控制自然恢复或明确取消后已释放', not reaction.is_controlled() and not motion.x and not motion.y and
                      not motion.z and not active and 'HIT_REACTION' not in driver.upper(), released)
            restore_recovery_fixture('case_end', now)
            evidence()['cases'].append({'name': current_step()['name'], 'scenario': name,
                                    'purpose': 'warmup' if state['index'] < len(warmup_steps) else 'formal',
                                    'checks': state['checks'], 'initial_target_yaw': state['initial_yaw'],
                                    'cancel_snapshot': state['cancel_snapshot'],
                                    'observed_from_world_time': state['at'], 'observed_until_world_time': now,
                                    'expected_hits': 4 if name == 'FullChain' else 2 if name in ('ContactNoImpact', 'SameLevel', 'Upgrade', 'AirFollow') else 1,
                                    'peer_count': len(worlds)})
            state['checks'] = []
            state['index'] += 1
            if state['index'] == len(warmup_steps):
                report['warmup']['completed'] = True
                report['warmup']['completed_world_time'] = now
                (output / 'warmup.json').write_text(json.dumps(report['warmup'], ensure_ascii=False, indent=2), encoding='utf-8')
            if state['index'] == len(steps):
                finish()
            else:
                state['phase'] = 'reset'
        except Exception:
            if state['phase'] == 'closing':
                report['cleanup_error'] = traceback.format_exc()
                (output / 'runtime.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
                unreal.unregister_slate_post_tick_callback(state['callback'])
                unreal.log_error(report['cleanup_error'])
                if options.get('HIT_FACING_QUIT_EDITOR', True):
                    unreal.SystemLibrary.quit_editor()
            else:
                finish(traceback.format_exc())

    state['callback'] = unreal.register_slate_post_tick_callback(tick)
    editor.editor_request_begin_play()
    print('HIT_FACING_RUNTIME_STARTED ' + mode + ' ' + str(lag))


def main():
    """系统Python仅做离线报告校验；编辑器执行入口由根启动器传入全局配置。"""
    parser = argparse.ArgumentParser()
    parser.add_argument('--report', required=True)
    parser.add_argument('--native-log', required=True)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    destination = Path(args.output)
    if destination.exists():
        raise FileExistsError('不覆盖既有验收报告: ' + str(destination))
    try:
        report = json.loads(Path(args.report).read_text(encoding='utf-8-sig'))
        native_text = Path(args.native_log).read_text(encoding='utf-8-sig', errors='replace')
        events = parse_native_log(native_text)
        audit = audit_report(report, events, parse_native_resolutions(native_text), parse_sample_markers(native_text))
    except (KeyError, TypeError, ValueError) as error:
        audit = {'passed': False, 'error': str(error), 'traceback': traceback.format_exc()}
    destination.write_text(json.dumps(audit, ensure_ascii=False, indent=2), encoding='utf-8')
    print('HIT_FACING_OFFLINE_AUDIT ' + str(audit['passed']))
    return 0 if audit['passed'] else 1


if __name__ == '__main__':
    if '--report' in sys.argv:
        raise SystemExit(main())
    run_runtime(globals())
