"""导出暗涌原生自动化结果；检查JSON内容，不把UE进程退出零误判为全部通过。"""
import argparse
import json
from pathlib import Path


def export_results(report_path: Path, output_path: Path) -> bool:
    """保留每个测试的结果与诊断，返回是否执行完整且没有失败。"""
    report = json.loads(report_path.read_text(encoding="utf-8-sig"))
    tests = []
    for row in report["tests"]:
        tests.append({
            "name": row["fullTestPath"],
            "state": row["state"],
            "duration_seconds": row.get("duration", 0),
            "diagnostics": [entry.get("event", {}) for entry in row.get("entries", [])],
        })
    havoc = [row for row in tests if row["name"].startswith("Hodge.Havoc.")]
    passed = (len(havoc) >= 8 and report["failed"] == 0 and report["notRun"] == 0
              and report.get("inProcess", 0) == 0 and all(row["state"] == "Success" for row in tests))
    result = {
        "design": "Docs/Design/havoc-dark-surge.md",
        "implementation_status": "native_and_formal_assets_integrated",
        "engine": "UE 5.5.4",
        "report_created_on": report["reportCreatedOn"],
        "source_report": str(report_path.resolve()),
        "native_tests_passed": passed,
        "total": len(tests),
        "havoc_tests": len(havoc),
        "succeeded_without_warnings": report["succeeded"],
        "succeeded_with_warnings": report["succeededWithWarnings"],
        "failed": report["failed"],
        "not_run": report["notRun"],
        "tests": tests,
        "not_validated": [
            "新动作通知帧与Box范围的逐帧美术校准",
            "Buff期间R、真实死亡重生及换Pawn的组合PIE场景",
            "HUD多分辨率适配及视觉验收",
            "真实输入端到端、Listen Server、PIE Dedicated和高延迟",
            "Cook、打包与独立服务器可执行文件",
        ],
        "coverage_note": "记录模块及需求用例覆盖；未生成源码行覆盖率，原生通过不代表蓝图或PIE通过。",
    }
    # Designer结构验证与原生自动化分开记录，不能把结构通过当成PIE通过。
    designer = Path(__file__).resolve().parents[2] / "Saved/Havoc/ui-designer-validation.json"
    if designer.exists():
        result["designer_validation"] = json.loads(designer.read_text(encoding="utf-8-sig"))
    # 保存断言副本到可入库报告，原始逐帧采样继续留在Saved中。
    root = Path(__file__).resolve().parents[2]
    runtime_reports = []
    paths = [root / 'Saved/Havoc' / (mode + suffix) for suffix, modes in [
        ('-havoc-e2e.json', ['single', 'listen', 'dedicated', 'listen-lag']),
        ('-havoc-hits-e2e.json', ['single', 'listen', 'dedicated'])] for mode in modes]
    paths.append(root / 'Saved/Havoc/UIRegression/single-e2e.json')
    for path in paths:
        if path.exists():
            row = json.loads(path.read_text(encoding='utf-8-sig'))
            runtime_reports.append({'source': str(path), 'mode': row['mode'], 'passed': row['passed'],
                                    'checks': row['checks'], 'error': row.get('error')})
    result['runtime_reports'] = runtime_reports
    result['runtime_assertions'] = sum(len(r['checks']) for r in runtime_reports)
    result['runtime_reports_all_passed'] = len(runtime_reports) == 8 and all(r['passed'] for r in runtime_reports)
    assets = root / 'Saved/Havoc/asset-validation.json'
    if assets.exists():
        result['asset_validation'] = json.loads(assets.read_text(encoding='utf-8-sig'))
    if result['runtime_reports_all_passed']:
        result['not_validated'].remove('真实输入端到端、Listen Server、PIE Dedicated和高延迟')
    result['all_implemented_checks_passed'] = passed and result['runtime_reports_all_passed'] and result.get('asset_validation', {}).get('passed', False)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    return result['all_implemented_checks_passed']


def main() -> int:
    """只读取已有测试报告，不启动编辑器或修改玩法资产。"""
    root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", type=Path, default=root / "Saved/Havoc/AutomationPassed/index.json")
    parser.add_argument("--output", type=Path, default=root / "Docs/Validation/havoc-dark-surge-2026-10-15-results.json")
    args = parser.parse_args()
    passed = export_results(args.report, args.output)
    print(json.dumps({"passed": passed, "output": str(args.output)}, ensure_ascii=False))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
