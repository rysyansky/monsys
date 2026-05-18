#pragma once

#include "readers/CpuStatReader.hpp"
#include "Models.hpp"
#include <chrono>
#include <map>
#include "Calculators.hpp"
#include "readers/MemoryStatReader.hpp"
#include "readers/NetworkStatReader.hpp"
#include "readers/DiskStatReader.hpp"
#include "readers/ProcessStatReader.hpp"

struct SystemSnapshot {
    CpuSnapshot all_cpu_percent;
    std::vector<CpuSnapshot> cpu_cores;

    std::vector<NetworkSnapshot> network;

    std::vector<DiskSnapshot> disk;

    std::map<std::uint32_t, ProcessSnapshot> processes;
};

class StatCollector {
    CpuStatReader cpu_reader;
    MemoryStatReader memory_reader;
    NetworkStatReader network_reader;
    DiskStatReader disk_reader;
    ProcessStatReader process_reader;
    Calculators calculators;
    uint16_t disk_sector_size = 512;

    SystemSnapshot prev_system_snapshot = {};
    SystemSnapshot curr_system_snapshot = {};

public:
    void Snapshot();
    std::optional<SystemData> GetSystemData();
private:
    bool is_first_snapshot = true;
};