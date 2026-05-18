#pragma once

#include "Models.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

struct ProcessMeta {
    std::uint32_t pid;
    std::string name;
};

struct ProcessSnapshot {
    char state;

    std::uint64_t cpu_usage;
    std::uint64_t memory_bytes;

    std::chrono::steady_clock::time_point ts;
};

class ProcessStatReader {
    std::vector<std::uint32_t> pids = {};
    std::map<std::uint32_t, ProcessSnapshot> process_snaps = {};
    std::vector<ProcessMeta> process_metas = {};
public:
    void ReadMeta();
    void Snap();
    std::vector<ProcessMeta> GetProcessMetas() const;
    std::map<std::uint32_t, ProcessSnapshot> GetProcessSnaps() const;
private:
    std::vector<std::uint32_t> FindPidsInProc() const;
    ProcessSnapshot SnapProcess(const std::string& pid) const;
    std::ifstream OpenFile(const std::string& path) const;
};