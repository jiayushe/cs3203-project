import fire
import glob
import os
import subprocess
import xml.etree.ElementTree as ET

FILE_DIR = os.path.dirname(os.path.abspath(__file__))
TEST_DIR = os.path.join(FILE_DIR, "../../Tests28/")

TEST_SUITE_COUNT = len(glob.glob(os.path.join(TEST_DIR, "*_source.txt")))
TEST_SUITE_FILES = [
    (
        i,
        os.path.join(TEST_DIR, f"{i}_source.txt"),
        os.path.join(TEST_DIR, f"{i}_queries.txt"),
        os.path.join(TEST_DIR, f"{i}_out.xml")
    )
    for i in range(1, TEST_SUITE_COUNT + 1)
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
        "slowest": []
    }
    time = []

    root = ET.parse(out_path).getroot()
    for query in root.findall("queries/query"):
        statistics["query_count"] += 1
        if query.find("passed") is not None:
            statistics["passed_count"] += 1
        if query.find("failed") is not None:
            statistics["failed_count"] += 1
        time.append((float(query.findtext("time_taken")), query.findtext("id")))

    sorted_time = sorted(time)
    statistics["time_25"], _ = sorted_time[len(sorted_time) // 4]
    statistics["time_50"], _ = sorted_time[len(sorted_time) // 2]
    statistics["time_75"], _ = sorted_time[(len(sorted_time) // 4) * 3]
    for t, _ in time:
        statistics["time_avg"] += t
    statistics["time_avg"] /= len(sorted_time)
    statistics["slowest"] = sorted_time[-1:-num_slowest:-1]

    return statistics

def autotest(binary_path, num_slowest=10):
    for i, source_path, queries_path, out_path in TEST_SUITE_FILES:
        print(f"Running test suite #{i}...")
        subprocess.run([binary_path, source_path, queries_path, out_path],
                stdout=subprocess.DEVNULL)
        statistics = get_statistics(out_path, num_slowest)
        print(f"# of queries = {statistics['query_count']}")
        print(f"# of passed  = {statistics['passed_count']}")
        print(f"# of failed  = {statistics['failed_count']}")
        print(f"p25 time     = {statistics['time_25']:.3f}ms")
        print(f"p50 time     = {statistics['time_50']:.3f}ms")
        print(f"p75 time     = {statistics['time_75']:.3f}ms")
        print(f"avg time     = {statistics['time_avg']:.3f}ms")
        print(f"{num_slowest} slowest queries:")
        for t, i in statistics["slowest"]:
            print(f"{i:4}: {t:.3f}ms")
        print()

if __name__ == "__main__":
    fire.Fire(autotest)

