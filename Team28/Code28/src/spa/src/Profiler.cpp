#include "Profiler.h"

std::unordered_map<std::string, long> Profiler::counter;

std::unordered_map<std::string, std::chrono::steady_clock::duration> Profiler::profile;

std::unordered_map<std::string, std::chrono::time_point<std::chrono::steady_clock>>
    Profiler::start_times;

void Profiler::clear() {
    counter.clear();
    profile.clear();
    start_times.clear();
}

void Profiler::print() {
    std::cout << std::endl;
    std::cout << "Counter:" << std::endl;
    for (auto const& entry : counter) {
        std::cout << "  " << entry.first << " = " << entry.second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Profile:" << std::endl;
    for (auto const& entry : profile) {
        std::cout << "  " << entry.first << " = "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(entry.second).count()
                  << "ms" << std::endl;
    }
    std::cout << std::endl;
}

void Profiler::record_start(const std::string& name) {
    start_times[name] = std::chrono::steady_clock::now();
}

void Profiler::record_end(const std::string& name) {
    if (start_times.find(name) == start_times.end()) {
        throw std::runtime_error("Cannot record end before record start");
    }

    auto start_time = start_times[name];
    auto end_time = std::chrono::steady_clock::now();
    profile[name] += (end_time - start_time);
}

void Profiler::increment(const std::string& name) { counter[name]++; }