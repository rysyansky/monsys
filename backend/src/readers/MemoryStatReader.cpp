#include "readers/MemoryStatReader.hpp"

void MemoryStatReader::Read() {
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        throw std::runtime_error("Failed to open /proc/meminfo");
    }
    memory_stats = {};

    std::string line;
    while(std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            memory_stats.total_bytes = std::stoull(line.substr(9)) * 1024;
        } else if (line.find("MemAvailable:") == 0) {
            memory_stats.available_bytes = std::stoull(line.substr(13)) * 1024;
        } else if (line.find("SwapTotal:") == 0) {
            memory_stats.swap_total_bytes = std::stoull(line.substr(10)) * 1024;
        } else if (line.find("SwapFree:") == 0) {
            uint64_t swap_free_bytes = std::stoull(line.substr(9)) * 1024;
            memory_stats.swap_used_bytes = memory_stats.swap_total_bytes - swap_free_bytes;
        }
    }
}

MemoryStats MemoryStatReader::GetMemoryStats() const{
    return memory_stats;
}