#include "Models.hpp"
#include "StatCollector.hpp"

#include <crow.h>
#include <fstream>
#include <sstream>
#include <vector>

void StatCollector::Snapshot() {
    prev_system_snapshot = curr_system_snapshot;
    curr_system_snapshot = {};

    cpu_reader.Read();

    auto cpu_snaps = cpu_reader.GetCpuSnapshots();

    auto cpu_it = cpu_snaps.find("cpu");
    if (cpu_it != cpu_snaps.end()) {
        curr_system_snapshot.all_cpu_percent = cpu_it->second;
    }

    for (const auto& [cpu_name, snap] : cpu_snaps) {
        if (cpu_name != "cpu") {
            curr_system_snapshot.cpu_cores.push_back(snap);
        }
    }

    network_reader.Read();
    curr_system_snapshot.network = network_reader.GetNetworkSnapshots();

    disk_reader.Read();
    curr_system_snapshot.disk = disk_reader.GetDiskSnapshots();

    process_reader.ReadMeta();
    process_reader.Snap();

    const auto proc_metas = process_reader.GetProcessMetas();
    const auto proc_snaps = process_reader.GetProcessSnaps();

    for (const auto& meta : proc_metas) {
        auto it = proc_snaps.find(meta.pid);

        if (it != proc_snaps.end()) {
            curr_system_snapshot.processes[meta.pid] = it->second;
        }
    }

    is_first_snapshot = false;
}

std::optional<SystemData> StatCollector::GetSystemData() {
    if (is_first_snapshot) {
        return std::nullopt;
    }

    SystemData data;

    data.timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

    {
        std::ifstream uptime_file("/proc/uptime");
        double uptime = 0.0;
        if (uptime_file) {
            uptime_file >> uptime;
        }
        data.uptime_seconds = static_cast<uint64_t>(uptime);
    }

    data.all_cpu_percent =
        calculators.CalculateCpuCoreDelta(
            prev_system_snapshot.all_cpu_percent,
            curr_system_snapshot.all_cpu_percent
        );

    for (size_t i = 0; i < curr_system_snapshot.cpu_cores.size(); ++i) {
        if (i >= prev_system_snapshot.cpu_cores.size()) {
            break;
        }

        data.cpu_cores.push_back(
            calculators.CalculateCpuCoreDelta(
                prev_system_snapshot.cpu_cores[i],
                curr_system_snapshot.cpu_cores[i]
            )
        );
    }

    memory_reader.Read();
    data.memory = memory_reader.GetMemoryStats();

    for (size_t i = 0; i < curr_system_snapshot.network.size(); ++i) {
        if (i >= prev_system_snapshot.network.size()) {
            break;
        }

        data.network.push_back(
            calculators.CalculateNetworkDelta(
                prev_system_snapshot.network[i],
                curr_system_snapshot.network[i]
            )
        );
    }

    for (size_t i = 0; i < curr_system_snapshot.disk.size(); ++i) {
        if (i >= prev_system_snapshot.disk.size()) {
            break;
        }

        data.disks.push_back(
            calculators.CalculateDiskDelta(
                prev_system_snapshot.disk[i],
                curr_system_snapshot.disk[i],
                disk_sector_size
            )
        );
    }

    const auto processes_meta = process_reader.GetProcessMetas();

    for (const auto& meta : processes_meta) {
        auto prev_it = prev_system_snapshot.processes.find(meta.pid);
        auto curr_it = curr_system_snapshot.processes.find(meta.pid);

        if (
            prev_it == prev_system_snapshot.processes.end() ||
            curr_it == curr_system_snapshot.processes.end()
        ) {
            continue;
        }

        auto delta = calculators.CalculateProcessDelta(
            prev_it->second,
            curr_it->second,
            data.memory.total_bytes,
            curr_system_snapshot.cpu_cores.size()
        );

        delta.pid = meta.pid;
        delta.name = meta.name;

        data.processes_data[meta.pid] = delta;
    }

    CROW_LOG_DEBUG << "cores=" << data.cpu_cores.size() << " disks=" << data.disks.size() << " net=" << data.network.size() << " proc_data=" << data.processes_data.size();

    return data;
}