#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <chrono>

struct CpuSnapshot {
    std::uint64_t user_time;
    std::uint64_t nice_time;
    std::uint64_t system_time;
    std::uint64_t idle_time;
    std::uint64_t iowait_time;
    std::uint64_t irq_time;
    std::uint64_t softirq_time;
    std::uint64_t steal_time;
    std::uint64_t guest_time;
    std::uint64_t guest_nice_time;
    std::chrono::steady_clock::time_point ts;
};

class CpuStatReader {
    std::map<std::string, CpuSnapshot> cpu_data = {};
public:
    void Read();
    std::map<std::string, CpuSnapshot> GetCpuSnapshots() const;
};