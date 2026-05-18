#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

struct DiskSnapshot
{
    std::string device;

    std::uint64_t read_sectors;
    std::uint64_t write_sectors;
    std::uint64_t ms_reading;
    std::uint64_t ms_writing;

    std::chrono::steady_clock::time_point ts;
};

class DiskStatReader {
    std::vector<DiskSnapshot> devices_snapshots = {};
public:
    void Read();
    std::vector<DiskSnapshot> GetDiskSnapshots() const;
    std::uint16_t GetSectorSize() const;
};