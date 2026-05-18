#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct CpuCoreStats
{
    float total_usage;
    float user_usage;
    float system_usage;
    float idle_usage;
};


inline void to_json(nlohmann::json& j, const CpuCoreStats& s) {
    j = {
        {"total_usage", s.total_usage},
        {"user_usage", s.user_usage},
        {"system_usage", s.system_usage},
        {"idle_usage", s.idle_usage}
    };
}

struct MemoryStats {
    std::uint64_t total_bytes;
    std::uint64_t available_bytes;
    
    std::uint64_t swap_total_bytes;
    std::uint64_t swap_used_bytes;
};

inline void to_json(nlohmann::json& j, const MemoryStats& s) {
    j = {
        {"total_bytes", s.total_bytes},
        {"available_bytes", s.available_bytes},
        {"swap_total_bytes", s.swap_total_bytes},
        {"swap_used_bytes", s.swap_used_bytes}
    };
}

struct NetworkStats {
    std::string interface;

    std::uint64_t rx_bytes_per_sec;
    std::uint64_t tx_bytes_per_sec;
    std::uint64_t rx_packets_per_sec;
    std::uint64_t tx_packets_per_sec;
};

inline void to_json(nlohmann::json& j, const NetworkStats& s) {
    j = {
        {"interface", s.interface},
        {"rx_bytes_per_sec", s.rx_bytes_per_sec},
        {"tx_bytes_per_sec", s.tx_bytes_per_sec},
        {"rx_packets_per_sec", s.rx_packets_per_sec},
        {"tx_packets_per_sec", s.tx_packets_per_sec}
    };
}

struct DiskStats {
    std::string device;

    std::uint64_t read_bytes_per_sec;
    std::uint64_t write_bytes_per_sec;

    float utilization_percent;
};

inline void to_json(nlohmann::json& j, const DiskStats& s) {
    j = {
        {"device", s.device},
        {"read_bytes_per_sec", s.read_bytes_per_sec},
        {"write_bytes_per_sec", s.write_bytes_per_sec},
        {"utilization_percent", s.utilization_percent}
    };
}

struct ProcessData {
    std::uint32_t pid;
    std::string name;
    char state;

    float cpu_percent;
    float memory_percent;
};

inline void to_json(nlohmann::json& j, const ProcessData& s) {
    j = {
        {"pid", s.pid},
        {"name", s.name},
        {"state", std::string(1, s.state)},
        {"cpu_percent", s.cpu_percent},
        {"memory_percent", s.memory_percent}
    };
}

struct SystemData {
    uint64_t timestamp_ms;

    CpuCoreStats all_cpu_percent;
    std::vector<CpuCoreStats> cpu_cores;

    MemoryStats memory;

    std::vector<NetworkStats> network;

    std::vector<DiskStats> disks;

    std::map<std::uint32_t, ProcessData> processes_data;

    uint64_t uptime_seconds;
};

inline void to_json(nlohmann::json& j, const SystemData& s) {
    j = {
        {"timestamp_ms", s.timestamp_ms},
        {"all_cpu_percent", s.all_cpu_percent},
        {"cpu_cores", s.cpu_cores},
        {"memory", s.memory},
        {"network", s.network},
        {"disks", s.disks},
        {"processes_data", s.processes_data},
        {"uptime_seconds", s.uptime_seconds}
    };
}