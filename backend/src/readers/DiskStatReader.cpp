#include "readers/DiskStatReader.hpp"
#include <vector>

void DiskStatReader::Read() {
    namespace fs = std::filesystem;

    std::ifstream diskstats_file("/proc/diskstats");
    if (!diskstats_file.is_open()) {
        throw std::runtime_error("Failed to open /proc/diskstats");
    }

    std::vector<std::string> valid_devices;
    for (const auto& entry : fs::directory_iterator("/sys/block")) {
        if (entry.is_directory()) {
            valid_devices.push_back(entry.path().filename().string());
        }
    }

    std::string line;
    devices_snapshots.clear();
    while (std::getline(diskstats_file, line)) {
        std::istringstream iss(line);
        /* 
         * DiskSnapshot needs:
         * - device name
         * - read sectors: Field  3 -- # of sectors read
         * - write sectors: Field  7 -- # of sectors written
         * - time spent reading: Field  4 -- # of milliseconds spent reading
         * - time spent writing: Field  8 -- # of milliseconds spent writing
         */
        DiskSnapshot snapshot;

        /*
         * Structure of each line in /proc/diskstats:
         * maj_num min_num device_name reads_completed reads_merged sectors_read ms_reading writes_completed writes_merged sectors_written ms_writing <...> 
         *   skip    skip       ^           skip            skip           ^          ^           skip            skip            ^            ^       <skip>
         */
        std::string skip;
        iss >> skip >> skip;

        iss >> snapshot.device;
        if (std::find(valid_devices.begin(), valid_devices.end(), snapshot.device) == valid_devices.end()) {
            continue;
        }
        
        iss >> skip >> skip;

        iss >> snapshot.read_sectors >> snapshot.ms_reading;

        iss >> skip >> skip;

        iss >> snapshot.write_sectors >> snapshot.ms_writing;

        snapshot.ts = std::chrono::steady_clock::now();

        devices_snapshots.push_back(snapshot);
    }
}

std::vector<DiskSnapshot> DiskStatReader::GetDiskSnapshots() const {
    return devices_snapshots;
}

std::uint16_t DiskStatReader::GetSectorSize() const {
    std::ifstream sector_size_file("/sys/block/sda/queue/hw_sector_size");
    if (!sector_size_file.is_open()) {
        throw std::runtime_error("Failed to open /sys/block/sda/queue/hw_sector_size");
    }
    std::string line;
    std::getline(sector_size_file, line);
    return static_cast<std::uint16_t>(std::stoul(line));
}