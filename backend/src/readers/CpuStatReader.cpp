#include "readers/CpuStatReader.hpp"

void CpuStatReader::Read() {
    std::ifstream stat_file("/proc/stat");
    if (!stat_file.is_open()) {
        throw std::runtime_error("Failed to open /proc/stat");
    }
    cpu_data.clear();

    std::string line;
    while (std::getline(stat_file, line)) {
        if (line.substr(0, 3) == "cpu") {
            std::istringstream iss(line);
            std::string cpu_name;
            CpuSnapshot snapshot{};
            iss >> cpu_name >> snapshot.user_time >> snapshot.nice_time >> snapshot.system_time
                >> snapshot.idle_time >> snapshot.iowait_time >> snapshot.irq_time
                >> snapshot.softirq_time >> snapshot.steal_time >> snapshot.guest_time
                >> snapshot.guest_nice_time;
            
            snapshot.ts = std::chrono::steady_clock::now();
            cpu_data[cpu_name] = snapshot;
        }
    }
}

std::map<std::string, CpuSnapshot> CpuStatReader::GetCpuSnapshots() const {
    return cpu_data;
}