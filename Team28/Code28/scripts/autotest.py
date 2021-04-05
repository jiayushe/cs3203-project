import fire
import glob
import os
import re
import sys
import subprocess
import xml.etree.ElementTree as ET

FILE_DIR = os.path.dirname(os.path.abspath(__file__))
TEST_DIR = os.path.join(FILE_DIR, "..", "..", "Tests28")

TEST_SUITE_IDS = sorted([
    re.search(r"([a-zA-Z0-9]+)_source.txt$", source)[1]
    for source in glob.glob(os.path.join(TEST_DIR, "*_source.txt"))
])
TEST_SUITE_FILES = [
    (
        suite_id,
        os.path.join(TEST_DIR, f"{suite_id}_source.txt"),
        os.path.join(TEST_DIR, f"{suite_id}_queries.txt"),
        os.path.join(TEST_DIR, f"{suite_id}_out.xml")
    )
    for suite_id in TEST_SUITE_IDS
]

def get_statistics(out_path, num_slowest):
    statistics = {
        "query_count": 0,
        "passed_count": 0,
        "failed_count": 0,
        "time_25": 0,
        "time_50": 0,
        "time_75": 0,
        "time_avg": 0,
        "time_total": 0,
        "slowest": []
    }
    time = []

    root = ET.parse(out_path).getroot()
    for query in root.findall("queries/query"):
        statistics["query_count"] += 1
        if query.find("passed") is not None:
            statistics["passed_count"] += 1
            time.append((float(query.findtext("time_taken")), query.findtext("id")))
        if query.find("failed") is not None:
            statistics["failed_count"] += 1
            time.append((float(query.findtext("time_taken")), query.findtext("id")))
        if query.find("exception") is not None:
            statistics["failed_count"] += 1

    sorted_time = sorted(time)
    statistics["time_25"], _ = sorted_time[len(sorted_time) // 4]
    statistics["time_50"], _ = sorted_time[len(sorted_time) // 2]
    statistics["time_75"], _ = sorted_time[(len(sorted_time) // 4) * 3]
    for t, _ in time:
        statistics["time_total"] += t
    statistics["time_avg"] = statistics["time_total"] / len(sorted_time)
    statistics["slowest"] = sorted_time[-1:-num_slowest:-1]

    return statistics

def autotest(binary_path, name=None, num_slowest=10):
    failed = False
    summary = {}
    for i, source_path, queries_path, out_path in TEST_SUITE_FILES:
        if name is not None and name != i:
            continue

        print(f"Running test suite {i}...")

        subprocess.run([binary_path, source_path, queries_path, out_path],
                stdout=subprocess.DEVNULL)

        statistics = get_statistics(out_path, num_slowest)
        if statistics["failed_count"] > 0:
            failed = True
        summary[i] = statistics

        print(f"# total      = {statistics['query_count']}")
        print(f"# passed     = {statistics['passed_count']}")
        print(f"# failed     = {statistics['failed_count']}")
        print(f"p25 time     = {statistics['time_25']:.3f}ms")
        print(f"p50 time     = {statistics['time_50']:.3f}ms")
        print(f"p75 time     = {statistics['time_75']:.3f}ms")
        print(f"avg time     = {statistics['time_avg']:.3f}ms")
        print(f"total time   = {statistics['time_total']:.3f}ms")
        print(f"{num_slowest} slowest queries:")
        for t, i in statistics["slowest"]:
            print(f"{i:4}: {t:.3f}ms")
        print()

    print("Summary")
    print("----------------------------------------------------------------------------------------------------------------------------")
    print("| Test case        |    # total |   # passed |   # failed |   p25 time |   p50 time |   p75 time |   avg time | total time |")
    print("----------------------------------------------------------------------------------------------------------------------------")
    for i, _, __, ___ in TEST_SUITE_FILES:
        if name is not None and name != i:
            continue

        print(f"| {i:16} |", end="")
        print(f" {summary[i]['query_count']:10} |", end="")
        print(f" {summary[i]['passed_count']:10} |", end="")
        print(f" {summary[i]['failed_count']:10} |", end="")
        print(f" {summary[i]['time_25']:10.3f} |", end="")
        print(f" {summary[i]['time_50']:10.3f} |", end="")
        print(f" {summary[i]['time_75']:10.3f} |", end="")
        print(f" {summary[i]['time_avg']:10.3f} |", end="")
        print(f" {summary[i]['time_total']:10.3f} |")
    print("----------------------------------------------------------------------------------------------------------------------------")
    print()

    if failed:
        sys.exit(1)

if __name__ == "__main__":
    fire.Fire(autotest)

