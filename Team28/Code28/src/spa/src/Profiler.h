#pragma once

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Profiler {
public:
    static void clear();
    static void print();

    static void record_start(const std::string& name);
    static void record_end(const std::string& name);
    static void increment(const std::string& name);

private:
    static std::unordered_map<std::string, long> counter;
    static std::unordered_map<std::string, std::chrono::steady_clock::duration> profile;
    static std::unordered_map<std::string, std::chrono::time_point<std::chrono::steady_clock>>
        start_times;
};
