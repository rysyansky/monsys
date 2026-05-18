#pragma once

#include "readers/CpuStatReader.hpp"
#include "readers/DiskStatReader.hpp"
#include "Models.hpp"
#include "readers/NetworkStatReader.hpp"
#include "readers/ProcessStatReader.hpp"

class Calculators {
public:
    CpuCoreStats CalculateCpuCoreDelta(const CpuSnapshot& prev, const CpuSnapshot& curr) const;
    NetworkStats CalculateNetworkDelta(const NetworkSnapshot& prev, const NetworkSnapshot& curr) const;
    DiskStats CalculateDiskDelta(const DiskSnapshot& prev, const DiskSnapshot& curr, uint16_t sector_size) const;
    ProcessData CalculateProcessDelta(const ProcessSnapshot& prev, const ProcessSnapshot& curr, uint64_t total_mem, std::size_t cpu_count) const;
};