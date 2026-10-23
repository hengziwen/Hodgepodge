"""受击补转离线断言的失败注入；不加载Unreal，不冒充真实PIE或联机结果。"""
import copy
import importlib.util
import json
from pathlib import Path
import unittest


SOURCE = Path(__file__).resolve().parents[1] / 'validate_hit_facing_runtime.py'
SPEC = importlib.util.spec_from_file_location('hit_facing_runtime_assertions', SOURCE)
RUNTIME = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(RUNTIME)
CHAIN = '00000001000000020000000300000004'
HIT = '00000005000000060000000700000008'

# 原始证据为Saved/CombatActionRequests/assets.json，夹具保留实际导出的字符串结构而不依赖Saved目录存在。
RECORDED_NPC_JSON = r'''[
    {"Name":"Combo.Entry","ComboTag":"(TagName=\"Combo.Entry\")","AbilityTag":"(TagName=\"\")","GrantedTags":"(GameplayTags=)",
     "Transitions":["(TriggerInputIntentTag=(TagName=\"InputIntent.Attack.Light\"),TriggerEventTag=(TagName=\"\"),TargetComboTag=(TagName=\"Combo.Light.01\"),RequiredWindowTags=(GameplayTags=),bAllowAfterExecutionEnded=False,RequiredSourceTags=(GameplayTags=),BlockedSourceTags=(GameplayTags=),TransitionPriority=0)"]},
    {"Name":"Combo.Light.01","ComboTag":"(TagName=\"Combo.Light.01\")","AbilityTag":"(TagName=\"Ability.Type.Action.Melee\")","GrantedTags":"(GameplayTags=)","Transitions":[]}
]'''
RECORDED_NPC_INTENT = '(TagName="InputIntent.Attack.Light")'


def native(kind='Step', avatar='server', before=0., after=10., at=.02, revision=2, extra=''):
    """生成明确身份的日志夹具，仅用于断言测试，不注入游戏运行状态。"""
    fields = f'Event={kind} Avatar={avatar} Role={2 if avatar == "owner" else 3} Chain={CHAIN} Segment=1 Revision={revision} WorldTime={at} '
    if kind in ('Step', 'Correction', 'FrozenRestore', 'NativeCorrection', 'NativeRejectedRestore', 'CurrentRestore', 'HistoricalRestore', 'ReplayBegin'):
        fields += f'BeforeYaw={before} '
    if kind != 'ReplayBegin':
        fields += f'AfterYaw={after} '
    if kind == 'Step':
        fields += 'DeltaTime=.02 EffectiveDeltaTime=.02 MaxYawSpeed=720 TargetYaw=10 Replay=0 Paused=0 Final=1 '
    elif kind in ('Correction', 'FrozenRestore'):
        fields += 'FinalYaw=10 '
    elif kind == 'NativeCorrection':
        fields += f'NativeType=RMS MoveTimeStamp=.02 NativeYaw={after} '
    elif kind == 'NativeRejectedRestore':
        fields += 'NativeType=RMS MoveTimeStamp=.01 RejectedYaw=80 '
    elif kind == 'CurrentRestore':
        fields += f'AnchorYaw={after} '
    elif kind == 'HistoricalRestore':
        fields += f'RecordedYaw={after} '
    return 'LogHodgeHitReactionFacing: ' + fields + extra


def fixture(peers=('authority',)):
    """最小完整采集报告，保留各端连续样本及真实伤害证据占位值。"""
    rows = []
    for peer in peers:
        avatar = 'server' if peer == 'authority' else peer
        for index, (at, yaw, final) in enumerate([(0., 0., False), (.02, 10., True), (.3, 10., True)]):
            rows.append({'case': 'SideLight', 'peer': peer, 'peer_key': avatar, 'avatar': avatar,
                         'sample_index': index, 'frame_index': index, 'world_time': at, 'server_time': at, 'controlled': True,
                         'actor_yaw': yaw, 'mesh_world_yaw': yaw, 'mesh_base_relative_yaw': 0., 'paused': False,
                         'facing': {'ChainId': CHAIN, 'Segment': 1, 'Revision': 2 if final else 1,
                                    'bHasTarget': True, 'bFinal': final, 'FinalYaw': 10.}})
    return {'error': None, 'cleanup_complete': True, 'mode': 'single' if len(peers) == 1 else 'listen',
            'player_target': 'owner' in peers, 'requested_cases': ['SideLight'],
            'cases': [{'name': 'SideLight', 'expected_hits': 1, 'initial_target_yaw': 0.,
                       'checks': [{'passed': True}], 'peer_count': len(peers)}],
            'samples': rows, 'hit_events': [{'case': 'SideLight', 'damage': .31}],
            'release_snapshots': [{'case': 'SideLight', 'controlled': False, 'rms': 0., 'animation_root': 0.,
                                  'playing_root': 0., 'active_reaction_montages': [], 'rotation_driver': 'LOCOMOTION'} for _ in peers]}


def audit(report, lines):
    """始终调用真实离线入口，避免测试复制一份判定逻辑。"""
    return RUNTIME.audit_report(report, RUNTIME.parse_native_log('\n'.join(lines)))


def resolution(hit=HIT, at=.02, sequence=1, outcome='Applied', phase='Controlled', level='None', falling=0, can_launch=0):
    """原生同步诊断格式夹具只验证解析关联，不将文本夹具计为真实GE。"""
    return (f'LogHodgeHitPresentation: Verbose: HitFacingResolved Avatar=server Hit={hit} Time={at} '
            f'Outcome={outcome} Level={level} Phase={phase} Falling={falling} Controlled=1 '
            f'Sequence={sequence} ChainStartedAt=.01 Chain={CHAIN} Segment=1 Revision=2 '
            f'Montage=/Game/Test/Hit MontageInstance=7 CanLaunch={can_launch}')


def polled_result(hit=HIT, at=.02):
    """轮询观察不含同步阶段，必须由真实解析器另行关联原生记录。"""
    return {'case': 'SameLevel', 'avatar': 'server', 'hit_id': hit, 'time': at, 'damage': .31,
            'outcome': '<HodgeHitReactionOutcome.APPLIED: 11>', 'level': '<HodgeReactionLevel.NONE: 0>',
            'probe_kind': 'HitStun', 'collected_by': 'SlatePoll'}


def sample_marker(row):
    """使用完整引擎帧构造独立标记，前缀模1000值仅作为无权威的显示文本。"""
    facing = row['facing']
    return (f"[007]LogPython: HitFacingSample Avatar={row['avatar']} Chain={facing['ChainId']} "
            f"Segment={facing['Segment']} Revision={facing['Revision']} sample_index={row['sample_index']} "
            f"engine_frame={row['engine_frame']} Yaw={row['actor_yaw']:.17g} WorldTime={row['world_time']:.17g}")


def warmup_fixture():
    """预热与正式数组各自完整保存；预热冷帧零补转不充当正式实际转动证明。"""
    report = fixture()
    report.update(requires_warmup=True, requires_sample_markers=True, requires_native_resolutions=True)
    names = ['WarmupHitStun', 'WarmupKnockback', 'WarmupLaunch']
    report['warmup'] = {'requested_actions': names, 'cases': [], 'samples': [], 'hit_events': [],
                        'release_snapshots': [], 'completed': True, 'error': None}
    lines = []
    for stage, name in enumerate(names + ['SideLight']):
        part = report['warmup'] if stage < 3 else report
        copied = fixture()
        base = float(stage)
        case = copied['cases'][0]
        case.update(name=name, purpose='warmup' if stage < 3 else 'formal',
                    observed_from_world_time=base, observed_until_world_time=base + .3)
        part['cases'].append(case) if stage < 3 else part.update(cases=[case])
        rows = copied['samples']
        for row in rows:
            row.update(case=name, world_time=base + row['world_time'], server_time=base + row['server_time'],
                       sample_index=stage * 3 + row['sample_index'], frame_index=stage * 3 + row['frame_index'])
            row['engine_frame'] = 1007 + row['sample_index']
            if stage < 3:
                row.update(actor_yaw=0., mesh_world_yaw=0.)
                row['facing']['FinalYaw'] = 0.
        part['samples'].extend(rows) if stage < 3 else part.update(samples=rows)
        hit = f'{stage + 5:08X}000000060000000700000008'
        event = polled_result(hit, base + .02)
        event.update(case=name, probe_kind=['HitStun', 'Knockback', 'Launch', 'HitStun'][stage])
        part['hit_events'].append(event) if stage < 3 else part.update(hit_events=[event])
        release = copied['release_snapshots'][0]
        release['case'] = name
        part['release_snapshots'].append(release) if stage < 3 else part.update(release_snapshots=[release])
        lines.extend([sample_marker(rows[0]), resolution(hit=hit, at=base + .02,
                      phase='Airborne' if stage == 2 else 'Controlled', falling=int(stage == 2), can_launch=int(stage == 2)),
                      native(after=0. if stage < 3 else 10., at=base + .02,
                             extra='TargetYaw=0' if stage < 3 else ''), sample_marker(rows[1]), sample_marker(rows[2])])
    return report, '\n'.join(lines)


class HitFacingAssertionsTest(unittest.TestCase):
    """锁定已批准的Actor容差、限速、回放端点及一次校正边界。"""

    def test_guid_formats_keep_full_identity(self):
        """反射GUID与原生日志相同，其他Avatar或段仍须保持区别。"""
        self.assertEqual(RUNTIME.guid_key('(A=1,B=2,C=3,D=4)'), CHAIN)
        self.assertEqual(RUNTIME.guid_key('00000001-00000002-00000003-00000004'), CHAIN)
        with self.assertRaises(ValueError):
            RUNTIME.guid_key('missing')

    def test_sample_marker_uses_complete_engine_frame_and_rejects_malformed_data(self):
        """相同显示帧前缀不能合并不同完整帧，缺字段、重复字段与非有限端点都拒绝。"""
        row = fixture()['samples'][0]
        row['engine_frame'] = 1007
        first = sample_marker(row)
        row['engine_frame'] = 2007
        markers = RUNTIME.parse_sample_markers(first + '\n' + sample_marker(row))
        self.assertEqual([value['engine_frame'] for value in markers], [1007, 2007])
        for line in [first.replace(' Segment=1', ''), first + ' Revision=2',
                     first.replace('engine_frame=1007', 'engine_frame=-1'), first.replace('Yaw=0', 'Yaw=nan')]:
            with self.subTest(line=line), self.assertRaises(ValueError):
                RUNTIME.parse_sample_markers(line)

    def test_marker_order_excludes_same_world_time_event_after_current_sample(self):
        """相同WorldTime不能把当前标记后的真实执行移进前一可见区间。"""
        rows = fixture()['samples']
        rows[1]['actor_yaw'] = 0.
        for row in rows:
            row['engine_frame'] = 1007 + row['sample_index']
        text = '\n'.join([sample_marker(rows[0]), sample_marker(rows[1]), native(at=.02), sample_marker(rows[2])])
        markers = RUNTIME.parse_sample_markers(text)
        links, failures = RUNTIME.correlate_sample_markers(rows, markers)
        self.assertFalse(failures)
        events = RUNTIME.parse_native_log(text)
        self.assertEqual(RUNTIME.native_interval(events, rows[0], rows[1], links), [])
        self.assertEqual([event['line'] for event in RUNTIME.native_interval(events, rows[1], rows[2], links)], [3])

    def test_marker_order_includes_ai_step_despite_different_world_time_and_identity(self):
        """AI时间推进及历史段身份不负责决定因果区间，区间内同Avatar事件全部保留。"""
        rows = fixture()['samples']
        for row in rows:
            row['engine_frame'] = 9007 + row['sample_index']
        text = '\n'.join([sample_marker(rows[0]), native(at=.04).replace('Segment=1', 'Segment=9'),
                          sample_marker(rows[1]), sample_marker(rows[2])])
        links, failures = RUNTIME.correlate_sample_markers(rows, RUNTIME.parse_sample_markers(text))
        self.assertFalse(failures)
        events = RUNTIME.native_interval(RUNTIME.parse_native_log(text), rows[0], rows[1], links)
        self.assertEqual([event['line'] for event in events], [2])
        self.assertEqual(events[0]['Segment'], 9)

    def test_required_marker_report_rejects_missing_duplicate_wrong_endpoint_or_frame(self):
        """实际报告的每条样本均需唯一精确标记，缺失或错配不能退回世界时冒充证据。"""
        report = fixture()
        report['requires_sample_markers'] = True
        for row in report['samples']:
            row['engine_frame'] = 1007 + row['sample_index']
        rows = report['samples']
        text = '\n'.join([sample_marker(rows[0]), native(), sample_marker(rows[1]), sample_marker(rows[2])])
        result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), sample_markers=RUNTIME.parse_sample_markers(text))
        self.assertTrue(result['passed'], result['checks'])
        self.assertTrue(result['causal_marker_evidence_complete'])
        variants = [text.replace(sample_marker(rows[1]), ''), text + '\n' + sample_marker(rows[1]),
                    text.replace('engine_frame=1008', 'engine_frame=7'),
                    text.replace('Yaw=10 WorldTime=0.02', 'Yaw=11 WorldTime=0.02')]
        for changed in variants:
            with self.subTest(changed=changed):
                result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(changed),
                                               sample_markers=RUNTIME.parse_sample_markers(changed))
                self.assertFalse(result['passed'])
                self.assertFalse(result['causal_marker_evidence_complete'])

    def test_legacy_report_cannot_claim_new_marker_evidence(self):
        """旧报告仍可按旧范围读取，未记录的因果标记不会被审计器补造。"""
        result = audit(fixture(), [native()])
        self.assertTrue(result['passed'])
        self.assertFalse(result['causal_marker_evidence_complete'])
        self.assertEqual(result['causal_attribution_scope'], 'legacy_world_time_only')

    def test_nested_correction_uses_marker_scope_instead_of_equal_world_time(self):
        """前一标记内的校正不能被后步吞并，当前区间的校正也不能因AI时间递增而漏配。"""
        row = fixture(('owner',))['samples'][0]
        row['engine_frame'] = 1007
        texts = [native('Correction', 'owner') + '\n' + sample_marker(row) + '\n' + native('Step', 'owner'),
                 sample_marker(row) + '\n' + native('Correction', 'owner', at=.01) + '\n' + native('Step', 'owner')]
        for text, expected in zip(texts, [[], [2]]):
            with self.subTest(text=text):
                parsed = RUNTIME.partition_native_motion(RUNTIME.parse_native_log(text), RUNTIME.parse_sample_markers(text))
                self.assertEqual(parsed[-1]['nested_correction_lines'], expected)
                self.assertEqual(parsed[-1]['normal_delta'], 0. if expected else 10.)

    def test_owner_first_allowed_step_is_selected_by_previous_marker(self):
        """首个可执行步依标记行序核对，较早WorldTime及同时间的提前未校正步均不得跳过。"""
        report = fixture(('authority', 'owner'))
        report['requires_sample_markers'] = True
        for row in report['samples']:
            row['engine_frame'] = 1007 + row['sample_index']
        server, owner = report['samples'][:3], report['samples'][3:]
        prefix = [sample_marker(server[0]), sample_marker(owner[0]), native()]
        suffix = [sample_marker(server[1]), sample_marker(owner[1]), sample_marker(server[2]), sample_marker(owner[2])]
        for deferred in [False, True]:
            with self.subTest(deferred=deferred):
                early = [native('Step', 'owner', after=0., at=.01)] if deferred else []
                text = '\n'.join(prefix + early + [native('Correction', 'owner', at=.01), native('Step', 'owner', at=.01)] + suffix)
                result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), sample_markers=RUNTIME.parse_sample_markers(text))
                first = [check for check in result['checks'] if '首个允许移动收尾' in check['name']]
                self.assertEqual([check['passed'] for check in first], [not deferred])
                self.assertEqual(result['passed'], not deferred, result['checks'])

    def test_marker_audits_establish_and_release_boundaries_without_facing_event(self):
        """建立与释放帧的未归因瞬转必须失败，身份换段不能跳过真实Actor端点。"""
        for boundary in ['establish', 'release']:
            with self.subTest(boundary=boundary):
                report = fixture()
                report['requires_sample_markers'] = True
                rows = report['samples']
                for row in rows:
                    row['engine_frame'] = 1007 + row['sample_index']
                if boundary == 'establish':
                    rows[0]['controlled'] = False
                    lines = [sample_marker(row) for row in rows]
                else:
                    rows[-1].update(controlled=False, actor_yaw=20.)
                    lines = [sample_marker(rows[0]), native(), sample_marker(rows[1]), sample_marker(rows[2])]
                text = '\n'.join(lines)
                result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), sample_markers=RUNTIME.parse_sample_markers(text))
                self.assertFalse(result['passed'])
                continuity = [check for check in result['checks'] if '历史与可见Yaw连续' in check['name']]
                self.assertTrue(any(value['reason'] == '可见Actor变化没有同身份原生归因' for value in continuity[0]['detail']))
                self.assertTrue(result['identity_boundary_observations'])

    def test_marker_audits_uncontrolled_interval_with_real_facing_execution(self):
        """两端都未控时若仍有转向日志，也必须核对其真实可见端点。"""
        report = fixture()
        report['requires_sample_markers'] = True
        rows = report['samples']
        for row in rows:
            row['engine_frame'] = 1007 + row['sample_index']
        rows[1]['controlled'] = rows[2]['controlled'] = False
        text = '\n'.join([sample_marker(rows[0]), native(), sample_marker(rows[1]),
                          native(before=10., after=11., at=.3, extra='TargetYaw=11'), sample_marker(rows[2])])
        result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), sample_markers=RUNTIME.parse_sample_markers(text))
        continuity = [check for check in result['checks'] if '历史与可见Yaw连续' in check['name']]
        self.assertFalse(continuity[0]['passed'])

    def test_replay_end_correction_cannot_be_nested_into_next_normal_step(self):
        """回放收尾后的普通步不能吞并前作用域校正，掩盖有效步长为零的非法转动。"""
        text = '\n'.join([native('ReplayBegin', 'owner', before=60.),
                          native('Correction', 'owner', before=60., after=0., extra='FinalYaw=0'),
                          native('ReplayEnd', 'owner', after=0.),
                          native('Step', 'owner', before=60., after=0., extra='EffectiveDeltaTime=0 TargetYaw=0')])
        records = RUNTIME.partition_native_motion(RUNTIME.parse_native_log(text), [])
        self.assertEqual(records[-1]['nested_correction_lines'], [])
        self.assertEqual(records[-1]['normal_delta'], -60.)
        self.assertTrue(RUNTIME.audit_visible_interval(records, 60.)[1])
        report = fixture(('authority', 'owner'))
        result = audit(report, [native(), text])
        physical = [check for check in result['checks'] if check['name'] == '全部原生移动步和真实校正符合约束']
        self.assertFalse(physical[0]['passed'])

    def test_normal_step_cannot_hide_opposing_unlogged_yaw_writes(self):
        """只累加正确delta仍不足；Step的BeforeYaw必须承接此前已建立的可见锚点。"""
        records = RUNTIME.partition_native_motion(RUNTIME.parse_native_log(native(before=20., after=30., extra='TargetYaw=30')))
        endpoint, failures = RUNTIME.audit_visible_interval(records, 0.)
        self.assertEqual(endpoint, 10.)
        self.assertTrue(any(value['reason'] == '正常移动步未承接已归因可见端点' for value in failures))

    def test_warmup_raw_markers_are_separate_and_do_not_grant_formal_turn_credit(self):
        """三类真实结算的独立预热标记必须全关联，冷帧零补转不会代替正式实际转动。"""
        report, text = warmup_fixture()
        result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), RUNTIME.parse_native_resolutions(text),
                                      RUNTIME.parse_sample_markers(text))
        self.assertTrue(result['passed'], result['checks'])
        self.assertTrue(result['causal_marker_evidence_complete'])
        self.assertEqual(len(report['cases']), 1)
        self.assertEqual(result['warmup_observation']['action_count'], 3)
        self.assertFalse(result['warmup_observation']['formal_pass_credit'])
        self.assertEqual(len(result['resolution_observations']), 4)
        for row in report['samples']:
            row['actor_yaw'] = 0.
        result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), RUNTIME.parse_native_resolutions(text),
                                      RUNTIME.parse_sample_markers(text))
        self.assertFalse(result['passed'])
        self.assertTrue(any(not item['passed'] and item['name'] == 'SideLight:确实从侧背方向发生转动' for item in result['checks']))

    def test_warmup_failure_missing_marker_or_wrong_native_plan_cannot_pass(self):
        """预热未完、采样遗失及原生真实计划错误均保留失败，不能用正式场景补足。"""
        for mutation in ['incomplete', 'missing_marker', 'wrong_plan']:
            with self.subTest(mutation=mutation):
                report, text = warmup_fixture()
                if mutation == 'incomplete':
                    report['warmup']['completed'] = False
                elif mutation == 'missing_marker':
                    text = text.replace(sample_marker(report['warmup']['samples'][1]), '')
                else:
                    text = text.replace('Outcome=Applied', 'Outcome=NoImpact', 1)
                    report['warmup']['hit_events'][0]['outcome'] = 'NoImpact'
                result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(text), RUNTIME.parse_native_resolutions(text),
                                              RUNTIME.parse_sample_markers(text))
                self.assertFalse(result['passed'])

    def test_native_resolution_parser_preserves_identity_and_rejects_malformed_fields(self):
        """原生行号、原文与完整身份保留；字段缺失、重复、越界及未知枚举都拒绝。"""
        parsed = RUNTIME.parse_native_resolutions(resolution())
        self.assertEqual(parsed[0]['hit_key'], HIT)
        self.assertEqual(parsed[0]['chain_key'], CHAIN)
        self.assertEqual(parsed[0]['line'], 1)
        self.assertEqual(parsed[0]['raw'], resolution())
        for line in [resolution().replace(' Controlled=1', ''), resolution() + ' Segment=2',
                     resolution().replace('Falling=0', 'Falling=2'), resolution().replace('Time=0.02', 'Time=nan'),
                     resolution().replace('Revision=2', 'Revision=4294967296'), resolution().replace('Outcome=Applied', 'Outcome=Unknown')]:
            with self.subTest(line=line), self.assertRaises(ValueError):
                RUNTIME.parse_native_resolutions(line)

    def test_resolution_linking_preserves_poll_and_uses_native_instant(self):
        """Slate观察可以稍晚，最终语义使用同一Hit的原生结算时点与阶段。"""
        polled = polled_result(at=.03)
        linked, failures = RUNTIME.correlate_resolution_events({}, fixture()['samples'], [polled],
                                                              RUNTIME.parse_native_resolutions(resolution()))
        self.assertFalse(failures)
        self.assertEqual(linked[0]['time'], .02)
        self.assertEqual(linked[0]['poll_observation'], polled)
        self.assertEqual(linked[0]['phase_at_resolution'], 'Controlled')
        self.assertEqual(linked[0]['collected_by'], 'NativeHitFacingResolved')
        self.assertNotIn('phase_at_resolution', polled)

    def test_resolution_linking_rejects_missing_duplicate_wrong_avatar_and_unpolled_hits(self):
        """原生日志必须一一对应，不能遗漏同帧命中或借其他目标证明本次结算。"""
        lines = [[], [resolution(), resolution()], [resolution().replace('Avatar=server', 'Avatar=other')],
                 [resolution(), resolution(hit='000000090000000A0000000B0000000C')],
                 [resolution(at=.04)], [resolution(outcome='NoImpact')]]
        for values in lines:
            with self.subTest(values=values):
                _, failures = RUNTIME.correlate_resolution_events({}, fixture()['samples'], [polled_result()],
                                                                   RUNTIME.parse_native_resolutions('\n'.join(values)))
                self.assertTrue(failures)

    def test_resolution_time_accepts_only_half_microsecond_serialization_interval(self):
        """原生六位小数的半微秒舍入可关联，真正超出该区间的未来结算仍拒绝。"""
        records = RUNTIME.parse_native_resolutions(resolution(at=.02))
        for poll_time, accepted in [(.0199995, True), (.0199994999, False), (.02, True), (.0200005, True)]:
            with self.subTest(poll_time=poll_time):
                linked, failures = RUNTIME.correlate_resolution_events({}, fixture()['samples'],
                                       [polled_result(at=poll_time)], records)
                self.assertEqual(not failures, accepted)
                self.assertEqual(linked[0]['native_resolution']['Time'], .02)
                self.assertEqual(linked[0]['native_resolution']['line'], 1)

    def test_resolution_time_case_bounds_use_same_half_microsecond_interval(self):
        """场景起止也只接受同一序列化区间相交，不能扩大到任意诊断时间容差。"""
        records = RUNTIME.parse_native_resolutions(resolution(at=.02))
        for start, end, accepted in [(.019, .0199995, True), (.019, .0199994999, False),
                                     (.0200005, .03, True), (.0200005001, .03, False)]:
            with self.subTest(start=start, end=end):
                _, failures = RUNTIME.correlate_resolution_events(
                    {'observed_from_world_time': start, 'observed_until_world_time': end},
                    fixture()['samples'], [polled_result(at=.021)], records)
                self.assertEqual(not failures, accepted)

    def test_same_level_semantics_require_real_native_success_and_same_montage_instance(self):
        """同级证明要求两个Applied、计划序号推进和原实例续播，轮询数据不能替代。"""
        second_hit = '000000090000000A0000000B0000000C'
        native_records = RUNTIME.parse_native_resolutions(resolution() + '\n' + resolution(hit=second_hit, at=.2, sequence=2))
        events, failures = RUNTIME.correlate_resolution_events({}, fixture()['samples'],
                            [polled_result(), polled_result(second_hit, .2)], native_records)
        self.assertFalse(failures)
        self.assertFalse(RUNTIME.followup_proof_failures('SameLevel', events))
        changed = copy.deepcopy(events)
        changed[1]['montage_instance_at_resolution'] = 8
        self.assertTrue(RUNTIME.followup_proof_failures('SameLevel', changed))
        changed = copy.deepcopy(events)
        changed[1]['outcome'] = 'NoImpact'
        self.assertTrue(RUNTIME.followup_proof_failures('SameLevel', changed))
        self.assertTrue(RUNTIME.followup_proof_failures('SameLevel', [polled_result(), polled_result(second_hit, .2)]))

    def test_air_follow_requires_native_airborne_falling_and_legal_noimpact(self):
        """请求时离地不足以证明空中GE，原生结算必须同时满足合法接触与离地。"""
        second_hit = '000000090000000A0000000B0000000C'
        second = polled_result(second_hit, .2)
        second.update(outcome='HodgeHitReactionOutcome.NO_IMPACT', probe_kind='AirHit')
        records = RUNTIME.parse_native_resolutions(resolution(phase='Airborne', falling=1, can_launch=1) + '\n' +
                    resolution(hit=second_hit, at=.2, outcome='NoImpact', phase='Airborne', falling=1))
        events, failures = RUNTIME.correlate_resolution_events({}, fixture()['samples'], [polled_result(), second], records)
        self.assertFalse(failures)
        self.assertFalse(RUNTIME.followup_proof_failures('AirFollow', events))
        for key, value in [('falling_at_resolution', False), ('phase_at_resolution', 'Downed'),
                           ('outcome', 'InvalidRequest'), ('can_launch', True)]:
            with self.subTest(key=key):
                changed = copy.deepcopy(events)
                changed[1][key] = value
                self.assertTrue(RUNTIME.followup_proof_failures('AirFollow', changed))

    def test_proxy_can_converge_before_short_segment_is_cleared(self):
        """短段已在保守接收下界0.25秒内到位，不要求0.25秒后仍持有旧控制。"""
        rows = [row for row in fixture(('proxy',))['samples']]
        rows[-1].update(world_time=.1, controlled=False)
        proof, _ = RUNTIME.proxy_convergence_proof(rows, [rows[1]])
        self.assertEqual(proof['verdict'], 'converged')
        self.assertEqual(proof['receive_lower_bound'], 0.)
        rows[1]['actor_yaw'] = 20.
        proof, _ = RUNTIME.proxy_convergence_proof(rows, [rows[1]])
        self.assertEqual(proof['verdict'], 'interrupted')
        self.assertIsNone(proof['converged_at'])

    def test_proxy_missing_receipt_bracket_or_continuity_cannot_claim_convergence(self):
        """首观测并非精确OnRep；没有前帧下界或采样断档时不能标收敛通过。"""
        rows = fixture(('proxy',))['samples']
        proof, _ = RUNTIME.proxy_convergence_proof(rows[1:], rows[1:])
        self.assertEqual(proof['verdict'], 'unproven')
        rows[1]['frame_index'] += 1
        proof, _ = RUNTIME.proxy_convergence_proof(rows, rows[1:])
        self.assertEqual(proof['verdict'], 'unproven')

    def test_proxy_late_mismatch_with_persistent_identity_is_failure(self):
        """身份持续有效时超过截止仍未到位或到位后又偏离都必须失败。"""
        rows = fixture(('proxy',))['samples']
        rows[-1]['actor_yaw'] = 13.
        proof, _ = RUNTIME.proxy_convergence_proof(rows, rows[1:])
        self.assertEqual(proof['verdict'], 'unproven')
        rows[1]['actor_yaw'] = 13.
        proof, _ = RUNTIME.proxy_convergence_proof(rows, rows[1:])
        self.assertEqual(proof['verdict'], 'unproven')

    def test_cancel_allows_initial_blank_then_late_active_then_clear(self):
        """初始空状态不是清空确认，合法迟到活动之后退出仍不构成复活。"""
        rows = fixture(('owner',))['samples']
        rows[0]['controlled'] = False
        rows[-1]['controlled'] = False
        proof = RUNTIME.cancel_identity_proof(rows, (CHAIN, 1), False, rows[1])
        self.assertEqual(proof['observation'], 'observed_then_left')
        self.assertFalse(proof['failures'])
        rows[1]['controlled'] = False
        self.assertEqual(RUNTIME.cancel_identity_proof(rows, (CHAIN, 1), False, rows[1])['observation'], 'never_observed')

    def test_cancel_rejects_visible_old_identity_reentry(self):
        """见过旧段并退出后再进入该旧段必须失败，不能靠最终释放掩盖。"""
        rows = fixture(('owner',))['samples']
        rows[1]['controlled'] = False
        proof = RUNTIME.cancel_identity_proof(rows, (CHAIN, 1), False, rows[0])
        self.assertTrue(proof['failures'])

    def test_real_collection_flag_requires_native_resolution(self):
        """实际采集报告必须显式保有原生日志，旧纯断言夹具不自动当成GE证据。"""
        report = fixture()
        report['requires_native_resolutions'] = True
        report['hit_events'] = [{**polled_result(), 'case': 'SideLight'}]
        result = RUNTIME.audit_report(report, RUNTIME.parse_native_log(native()), [])
        self.assertFalse(result['passed'])

    def test_facing_export_reads_zero_and_uint32_max_without_defaults(self):
        """零值和最大值必须来自完整导出字段，不能靠缺失时默认零通过。"""
        zero = '(ChainId=(A=0,B=0,C=0,D=0),Segment=0,Revision=0,Preset=None,bRetired=False)'
        self.assertEqual(RUNTIME.parse_facing_uint32_fields(zero), {'Segment': 0, 'Revision': 0})
        self.assertEqual(RUNTIME.parse_facing_uint32_fields('(Segment=4294967295,Revision=4294967295)'),
                         {'Segment': 0xffffffff, 'Revision': 0xffffffff})

    def test_facing_export_rejects_missing_duplicate_or_invalid_identity(self):
        """字段缺失、重复、越界及非十进制整数必须中止采集。"""
        invalid = ['()', '(Segment=1)', '(Revision=1)', '(Segment=1,Segment=2,Revision=3)',
                   '(Segment=1,Revision=2,Revision=3)', '(OtherSegment=1,Revision=2)',
                   '(Segment=1,OtherRevision=2)', '(Segment=1,Revision=2,)', 'Segment=1,Revision=2']
        invalid += ['(Segment=' + value + ',Revision=1)' for value in
                    ['-1', '+1', '4294967296', '1.0', '1e2', '0xff', '"1"', '', '١']]
        invalid += ['(Segment=1,Revision=' + value + ')' for value in ['-1', '4294967296', '2.0', '']]
        for exported in invalid:
            with self.subTest(exported=exported), self.assertRaises(ValueError):
                RUNTIME.parse_facing_uint32_fields(exported)

    def test_facing_export_matches_only_top_level_exact_field_names(self):
        """嵌套结构及带转义引号的字符串不能冒充顶层段身份。"""
        exported = r'(Other=(Segment=99,Revision=88),Text="escaped\" Segment=77,Revision=66", Segment = 3 , Revision = 4 )'
        self.assertEqual(RUNTIME.parse_facing_uint32_fields(exported), {'Segment': 3, 'Revision': 4})
        with self.assertRaises(ValueError):
            RUNTIME.parse_facing_uint32_fields('(Other=(Segment=99,Revision=88))')

    def test_facing_export_rejects_unbalanced_or_nontext_input(self):
        """格式损坏和非文本输入不能静默生成看似合法的身份。"""
        for exported in ['(Segment=1,Revision=2))', '(Other=(X=1,Segment=1,Revision=2)',
                         '(Text="unfinished,Segment=1,Revision=2)', '(Segment=1,,Revision=2)']:
            with self.subTest(exported=exported), self.assertRaises(ValueError):
                RUNTIME.parse_facing_uint32_fields(exported)
        with self.assertRaises(TypeError):
            RUNTIME.parse_facing_uint32_fields(None)

    def test_npc_recorded_exporttext_is_passed_intact_and_full_tag_is_preserved(self):
        """实际资产中的转换是字符串；原样交给反射解析器，完整Tag文本不能再次拼接。"""
        rows = json.loads(RECORDED_NPC_JSON)
        received = []

        def reflected_result(serialized):
            """这里只替身反射的返回值，不在纯Python中复制UE结构解析器。"""
            received.append(serialized)
            return RECORDED_NPC_INTENT

        self.assertIsInstance(rows[0]['Transitions'][0], str)
        self.assertEqual(RUNTIME.unique_npc_intent_export(rows, reflected_result), RECORDED_NPC_INTENT)
        self.assertEqual(received, rows[0]['Transitions'])

    def test_npc_intents_reject_empty_ambiguous_or_wrong_schema(self):
        """过滤空Tag并去重同一合法意图，缺失、歧义和未经反射的对象格式都须停止。"""
        rows = json.loads(RECORDED_NPC_JSON)
        original = rows[0]['Transitions'][0]
        rows[0]['Transitions'] = [original, original, 'empty-transition']
        self.assertEqual(RUNTIME.unique_npc_intent_export(rows,
                         lambda value: None if value == 'empty-transition' else RECORDED_NPC_INTENT), RECORDED_NPC_INTENT)
        with self.assertRaises(ValueError):
            RUNTIME.unique_npc_intent_export(rows, lambda value: None)
        with self.assertRaises(ValueError):
            RUNTIME.unique_npc_intent_export(rows, lambda value: value)
        with self.assertRaises(TypeError):
            RUNTIME.unique_npc_intent_export([{'Transitions': [{'TriggerInputIntentTag': RECORDED_NPC_INTENT}]}], lambda value: value)
        with self.assertRaises(TypeError):
            RUNTIME.unique_npc_intent_export(rows, lambda value: 3)

    def test_single_authority_baseline_passes(self):
        """完整单机证据可通过离线检查，未证明任何真实运行结果。"""
        result = audit(fixture(), [native()])
        self.assertTrue(result['passed'], result['checks'])

    def test_proxy_asserts_actor_and_retains_independent_mesh_error(self):
        """Mesh偏差不会冒充Actor失败，Actor超出2度仍必须失败。"""
        report = fixture(('authority', 'proxy'))
        report['samples'][-1]['mesh_world_yaw'] = 25.
        result = audit(report, [native()])
        self.assertTrue(result['passed'], result['checks'])
        self.assertEqual(result['mesh_observations'][0]['max_mesh_error_after_025s'], 15.)
        report['samples'][-1]['actor_yaw'] = 12.01
        self.assertFalse(audit(report, [native()])['passed'])

    def test_owner_correction_is_not_counted_twice_as_normal_turn(self):
        """同身份同端点的Correction和scope Step只表达一次可见跳转。"""
        lines = [native(), native('Correction', 'owner'), native('Step', 'owner')]
        parsed = RUNTIME.partition_native_motion(RUNTIME.parse_native_log('\n'.join(lines)))
        self.assertEqual(parsed[-1]['normal_delta'], 0.)
        self.assertEqual(parsed[-1]['nested_correction_lines'], [2])
        result = audit(fixture(('authority', 'owner')), lines)
        self.assertTrue(result['passed'], result['checks'])

    def test_duplicate_or_wrong_identity_correction_fails(self):
        """不能重复消费同修订，其他段或修订也不能替本段豁免瞬转。"""
        report = fixture(('authority', 'owner'))
        lines = [native(), native('Correction', 'owner'), native('Step', 'owner')]
        self.assertFalse(audit(copy.deepcopy(report), lines + [native('Correction', 'owner', before=10., at=.3)])['passed'])
        lines[1] = native('Correction', 'owner', revision=9)
        self.assertFalse(audit(report, lines)['passed'])

    def test_first_allowed_owner_step_cannot_defer_correction(self):
        """即使末帧最终到位，首个允许CMC收尾仍不能延迟校正。"""
        report = fixture(('authority', 'owner'))
        for row in report['samples']:
            if row['peer'] == 'owner' and row['world_time'] == .02:
                row['actor_yaw'] = 0.
        lines = [native(), native('Step', 'owner', after=0.),
                 native('Correction', 'owner', at=.3), native('Step', 'owner', at=.3)]
        self.assertFalse(audit(report, lines)['passed'])

    def test_final_flag_does_not_exempt_normal_rate_or_visible_change(self):
        """没有真实校正记录的Final状态仍必须服从正常限速及可见归因。"""
        report = fixture()
        report['samples'][1]['actor_yaw'] = 40.
        lines = [native(after=40., extra='TargetYaw=45')]
        self.assertFalse(audit(report, lines)['passed'])
        report = fixture()
        report['samples'][1]['actor_yaw'] = 12.
        self.assertFalse(audit(report, [native()])['passed'])

    def test_effective_step_and_direction_bounds_cannot_be_relaxed(self):
        """世界窗口有效步长、单调性和目标限幅分别接受失败注入。"""
        for extra in ['EffectiveDeltaTime=.001', 'DeltaTime=.01', 'TargetYaw=9', 'TargetYaw=-10', 'Paused=1', 'Hard=1']:
            with self.subTest(extra=extra):
                self.assertFalse(audit(fixture(), [native(extra=extra)])['passed'])
        self.assertTrue(audit(fixture(), [native(extra='TargetYaw=180')])['passed'])

    def test_current_restore_only_restores_existing_visible_anchor(self):
        """当前动作恢复不能借回放名义生成无速率约束的新转身。"""
        records = RUNTIME.partition_native_motion(RUNTIME.parse_native_log('\n'.join([
            native('ReplayBegin', 'owner', before=4., after=4.),
            native('Step', 'owner', before=4., after=9., extra='Replay=1'),
            native('CurrentRestore', 'owner', before=9., after=10.),
            native('ReplayEnd', 'owner', after=10.)])))
        endpoint, failures = RUNTIME.audit_visible_interval(records, 10.)
        self.assertEqual(endpoint, 10.)
        self.assertFalse(failures)
        _, failures = RUNTIME.audit_visible_interval(records, 0.)
        self.assertTrue(failures)
        _, failures = RUNTIME.audit_visible_interval(records[2:], 10.)
        self.assertTrue(failures)

    def test_replay_final_correction_uses_visible_final_endpoint(self):
        """历史中间Yaw无需计作可见运动，真正同段Final端点仍须完整核对。"""
        records = RUNTIME.partition_native_motion(RUNTIME.parse_native_log('\n'.join([
            native('ReplayBegin', 'owner', before=-20.),
            native('Step', 'owner', before=-20., after=-10., extra='Replay=1'),
            native('Correction', 'owner', before=-10.),
            native('ReplayEnd', 'owner') ])))
        endpoint, failures = RUNTIME.audit_visible_interval(records, 0.)
        self.assertEqual(endpoint, 10.)
        self.assertFalse(failures)
        records[-1]['AfterYaw'] = 11.
        self.assertTrue(RUNTIME.audit_visible_interval(records, 0.)[1])

    def test_interval_does_not_double_count_previous_frame(self):
        """相邻帧边界采用开闭区间，前一帧事件不能再次归因。"""
        rows = fixture()['samples']
        events = RUNTIME.parse_native_log(native())
        self.assertEqual(len(RUNTIME.native_interval(events, rows[0], rows[1])), 1)
        self.assertEqual(RUNTIME.native_interval(events, rows[1], rows[2]), [])

    def test_segment_change_does_not_skip_visible_boundary_sample(self):
        """升级换段边界的实际跳转仍需同端身份日志，不能通过换段绕开检查。"""
        report = fixture()
        for row in report['samples'][1:]:
            row['facing']['Segment'] = 2
        lines = [native().replace('Segment=1', 'Segment=2')]
        self.assertTrue(audit(copy.deepcopy(report), lines)['passed'])
        self.assertFalse(audit(report, [native(avatar='other')])['passed'])

    def test_historical_restore_requires_explicit_paired_evidence(self):
        """历史大角度还原必须有独立日志配对，Replay或Final标记本身不能豁免。"""
        lines = [native(), native('ReplayBegin', 'owner', before=-50.),
                 native('HistoricalRestore', 'owner', before=-50.),
                 native('Step', 'owner', before=-50., extra='Replay=1'),
                 native('Correction', 'owner', before=10.), native('ReplayEnd', 'owner'),
                 native('Step', 'owner', before=10.)]
        result = audit(fixture(('authority', 'owner')), lines)
        self.assertTrue(result['passed'], result['checks'])
        self.assertFalse(audit(fixture(('authority', 'owner')), lines[:2] + lines[3:])['passed'])

    def test_missing_peer_gap_damage_cleanup_or_nonfinite_data_fails(self):
        """缺端、漏帧、零伤害、残留PIE与非有限证据均不能伪装为成功。"""
        reports = [fixture(('authority', 'proxy')) for _ in range(6)]
        reports[0]['samples'] = [row for row in reports[0]['samples'] if row['peer'] == 'authority']
        reports[1]['samples'][-1]['sample_index'] += 1
        reports[2]['hit_events'][0]['damage'] = 0.
        reports[3]['cleanup_complete'] = False
        reports[4]['samples'][-1]['actor_yaw'] = float('nan')
        reports[5]['samples'][-1]['world_time'] = .01
        for index, report in enumerate(reports):
            with self.subTest(index=index):
                self.assertFalse(audit(report, [native()])['passed'])
        for extra in ['BeforeYaw=nan', 'AfterYaw=inf']:
            with self.assertRaises(ValueError):
                RUNTIME.parse_native_log(native(extra=extra))

    def test_relevancy_gap_or_reaction_residue_cannot_pass_release(self):
        """缺失观察帧不能声称持续相关，残留运动、动画或转向请求都必须失败。"""
        report = fixture()
        report['samples'][-1]['frame_index'] += 1
        self.assertFalse(audit(report, [native()])['passed'])
        for key, value in [('rms', 1.), ('animation_root', 1.), ('playing_root', 1.),
                           ('active_reaction_montages', [{'weight': .1}]), ('rotation_driver', 'HIT_REACTION')]:
            with self.subTest(key=key):
                report = fixture()
                report['release_snapshots'][0][key] = value
                self.assertFalse(audit(report, [native()])['passed'])

    def test_recovery_scope_requires_two_approved_cases_and_formal_configuration_elsewhere(self):
        """专项隔离必须成对还原，正式全链和预热不得继承关闭自动恢复的配置。"""
        names = ['SameLevel', 'Upgrade', 'FullChain']
        profile = RUNTIME.HERO_RECOVERY_PROFILE
        report = {'player_target': True, 'requested_cases': names,
                  'ground_recovery_baseline': {'profile': profile, 'enabled': True},
                  'ground_recovery_scopes': [{'case': name, 'profile': profile, 'before_enabled': True,
                      'override_enabled': False, 'restored': True, 'restored_enabled': True,
                      'restore_reason': 'case_end', 'entered_at': 1., 'restored_at': 2.} for name in names[:2]],
                  'samples': [{'case': name, 'sample_index': index, 'recovery_profile': profile,
                               'ground_recovery_enabled': name == 'FullChain'} for index, name in enumerate(names)],
                  'warmup': {'samples': [{'case': 'WarmupHitStun', 'sample_index': 0,
                                         'recovery_profile': profile, 'ground_recovery_enabled': True}]}}
        self.assertEqual(RUNTIME.recovery_scope_failures(report), [])
        broken = []
        for key, value in [('restored', False), ('restored_enabled', False), ('profile', 'Dummy'),
                           ('restore_reason', 'cleanup'), ('case', 'FullChain')]:
            candidate = copy.deepcopy(report)
            candidate['ground_recovery_scopes'][0][key] = value
            broken.append(candidate)
        candidate = copy.deepcopy(report)
        candidate['samples'][-1]['ground_recovery_enabled'] = False
        broken.append(candidate)
        candidate = copy.deepcopy(report)
        candidate['warmup']['samples'][0]['ground_recovery_enabled'] = False
        broken.append(candidate)
        candidate = copy.deepcopy(report)
        candidate['ground_recovery_scopes'].pop()
        broken.append(candidate)
        candidate = copy.deepcopy(report)
        candidate['player_target'] = False
        broken.append(candidate)
        for candidate in broken:
            self.assertTrue(RUNTIME.recovery_scope_failures(candidate), candidate)

    def test_recovery_scope_cleanup_restores_original_value_and_keeps_failed_restore_retryable(self):
        """异常还原必须读回原值；一次setter失败不能把待还原记录伪装为成功。"""
        class Profile:
            """模拟配置写入一次失败，仅验证清理契约，不冒充Unreal执行。"""
            enabled = False
            fail_once = True

            def set_editor_property(self, key, value):
                """首次失败保留旧值，后续清理可重试同一原值。"""
                if self.fail_once:
                    self.fail_once = False
                    raise RuntimeError('injected restore failure')
                self.enabled = value

            def get_editor_property(self, key):
                """返回实际读回值，不能只看setter未抛异常。"""
                return self.enabled

        profile = Profile()
        record = {'profile': RUNTIME.HERO_RECOVERY_PROFILE, 'before_enabled': True, 'restored': False}
        scope = {'profile': profile, 'record': record}
        with self.assertRaises(RuntimeError):
            RUNTIME.restore_ground_recovery_scope(scope, 'cleanup')
        self.assertFalse(record['restored'])
        self.assertFalse(profile.enabled)
        RUNTIME.restore_ground_recovery_scope(scope, 'cleanup')
        self.assertTrue(profile.enabled)
        self.assertTrue(record['restored'])
        self.assertTrue(record['restored_enabled'])
        self.assertEqual(record['restore_reason'], 'cleanup')

    def test_native_rotation_response_is_distinct_from_unique_final_correction(self):
        """真实原生响应可单独瞬转，不能借此豁免普通步超速或重复消费Final。"""
        lines = [native(), native('NativeCorrection', 'owner', before=0., after=80.),
                 native('Correction', 'owner', before=80.), native('Step', 'owner', before=80.)]
        result = audit(fixture(('authority', 'owner')), lines)
        self.assertTrue(result['passed'], result['checks'])
        self.assertEqual(len(result['native_correction_observations']), 1)
        self.assertFalse(audit(fixture(('authority', 'owner')), lines + [native('Correction', 'owner')])['passed'])
        # 去掉实际Final纠正后，80到10的普通步仍超过14.4度，不能被原生事件豁免。
        self.assertFalse(audit(fixture(('authority', 'owner')), lines[:2] + lines[3:])['passed'])

    def test_native_response_requires_valid_ack_kind_identity_and_applied_endpoint(self):
        """未知类型、无效ACK、空身份或未应用端点都不能授权原生朝向跳转。"""
        base = [native(), native('NativeCorrection', 'owner', before=0., after=10.),
                native('Correction', 'owner'), native('Step', 'owner', before=10.)]
        self.assertTrue(audit(fixture(('authority', 'owner')), base)['passed'])
        variants = [base[1].replace('NativeType=RMS', 'NativeType=Montage'),
                    base[1].replace('MoveTimeStamp=.02', 'MoveTimeStamp=-1'),
                    base[1].replace('Segment=1', 'Segment=4294967296'),
                    base[1].replace(CHAIN, '0' * 32),
                    base[1].replace('Role=2', 'Role=1'),
                    base[1].replace('NativeYaw=10.0', 'NativeYaw=80')]
        for value in variants:
            self.assertFalse(audit(fixture(('authority', 'owner')), [base[0], value] + base[2:])['passed'], value)
        self.assertFalse(audit(fixture(('authority', 'owner')), base + [base[1]])['passed'])
        for missing in [' NativeType=RMS', ' MoveTimeStamp=.02', ' NativeYaw=10.0']:
            with self.assertRaises(ValueError):
                RUNTIME.parse_native_log(base[1].replace(missing, ''))

    def test_native_corrected_replay_restores_applied_anchor_before_next_normal_step(self):
        """同段历史重算结束应承接原生新锚点，既不能恢复旧21度也不能无事件跳转。"""
        lines = [native('NativeCorrection', 'owner', before=42., after=54.),
                 native('ReplayBegin', 'owner', before=54.),
                 native('Step', 'owner', before=54., after=56., extra='Replay=1 TargetYaw=60'),
                 native('Step', 'owner', before=56., after=60., extra='Replay=1 TargetYaw=60'),
                 native('Step', 'owner', before=60., after=60., extra='Replay=1 TargetYaw=60'),
                 native('CurrentRestore', 'owner', before=60., after=54.),
                 native('ReplayEnd', 'owner', after=54.),
                 native('Step', 'owner', before=54., after=60., extra='TargetYaw=60')]
        records = RUNTIME.partition_native_motion(RUNTIME.parse_native_log('\n'.join(lines)))
        endpoint, failures = RUNTIME.audit_visible_interval(records, 42.)
        self.assertEqual(failures, [])
        self.assertAlmostEqual(endpoint, 60.)
        self.assertTrue(RUNTIME.audit_visible_interval(records[1:], 42.)[1])
        broken = copy.deepcopy(records)
        broken[5].update(AfterYaw=21., AnchorYaw=21.)
        broken[6]['AfterYaw'] = 21.
        self.assertTrue(RUNTIME.audit_visible_interval(broken, 42.)[1])

    def test_rejected_native_yaw_restore_cannot_authorize_visible_turn(self):
        """重复回包恢复前后应保持原可见Yaw，不能冒充NativeCorrection或Final消费。"""
        lines = [native(), native('NativeRejectedRestore', 'owner', before=0., after=0.),
                 native('Correction', 'owner', before=0.), native('Step', 'owner', before=0.)]
        result = audit(fixture(('authority', 'owner')), lines)
        self.assertTrue(result['passed'], result['checks'])
        self.assertEqual(len(result['native_rejected_restore_observations']), 1)
        self.assertEqual(result['native_correction_observations'], [])
        changed = lines[1].replace('AfterYaw=0.0', 'AfterYaw=80')
        self.assertFalse(audit(fixture(('authority', 'owner')), [lines[0], changed] + lines[2:])['passed'])
        no_final = [lines[0], lines[1], native('Step', 'owner', before=0., after=80., extra='TargetYaw=90')]
        self.assertFalse(audit(fixture(('authority', 'owner')), no_final)['passed'])

    def test_rejected_native_restore_requires_original_anchor_and_raw_rejected_yaw(self):
        """拒绝恢复必须承接当前端点并保留被撤销的原生Yaw，不能跨回放作用域配对。"""
        record = native('NativeRejectedRestore', 'owner', before=20., after=20.)
        parsed = RUNTIME.parse_native_log(record)
        self.assertEqual(RUNTIME.audit_visible_interval(parsed, 20.)[1], [])
        self.assertTrue(RUNTIME.audit_visible_interval(parsed, 0.)[1])
        replay = RUNTIME.parse_native_log(native('ReplayBegin', 'owner', before=20.) + '\n' + record)
        self.assertTrue(RUNTIME.audit_visible_interval(replay, 20.)[1])
        with self.assertRaises(ValueError):
            RUNTIME.parse_native_log(record.replace(' RejectedYaw=80', ''))


if __name__ == '__main__':
    unittest.main()
