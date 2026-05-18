#include "Calculators.hpp"

static double TimeDiffSec(const std::chrono::steady_clock::time_point& a,
                           const std::chrono::steady_clock::time_point& b)
{
    return std::chrono::duration<double>(a - b).count();
}

static float ClampPercent(double value) {
    if (value < 0.0) {
        return 0.0f;
    }

    if (value > 100.0) {
        return 100.0f;
    }

    return static_cast<float>(value);
}

CpuCoreStats Calculators::CalculateCpuCoreDelta(const CpuSnapshot& prev, const CpuSnapshot& curr) const
{
    CpuCoreStats delta;

    if (
        curr.user_time < prev.user_time ||
        curr.nice_time < prev.nice_time ||
        curr.system_time < prev.system_time ||
        curr.idle_time < prev.idle_time ||
        curr.iowait_time < prev.iowait_time ||
        curr.irq_time < prev.irq_time ||
        curr.softirq_time < prev.softirq_time ||
        curr.steal_time < prev.steal_time
    ) {
        return delta;
    }

    uint64_t prev_idle = prev.idle_time + prev.iowait_time;
    uint64_t curr_idle = curr.idle_time + curr.iowait_time;

    uint64_t prev_non_idle = prev.user_time + prev.nice_time + prev.system_time + prev.irq_time + prev.softirq_time + prev.steal_time;
    uint64_t curr_non_idle = curr.user_time + curr.nice_time + curr.system_time + curr.irq_time + curr.softirq_time + curr.steal_time;

    uint64_t prev_total = prev_idle + prev_non_idle;
    uint64_t curr_total = curr_idle + curr_non_idle;

    uint64_t totald = curr_total - prev_total;
    uint64_t idled = curr_idle - prev_idle;

    delta.total_usage = totald > 0 ? ClampPercent((totald - idled) * 100.0 / totald) : 0.0f;
    delta.user_usage = totald > 0 ? ClampPercent((curr.user_time - prev.user_time) * 100.0 / totald) : 0.0f;
    delta.system_usage = totald > 0 ? ClampPercent((curr.system_time - prev.system_time) * 100.0 / totald) : 0.0f;
    delta.idle_usage = totald > 0 ? ClampPercent((curr_idle - prev_idle) * 100.0 / totald) : 0.0f;

    return delta;
}

NetworkStats Calculators::CalculateNetworkDelta(
    const NetworkSnapshot& prev,
    const NetworkSnapshot& curr
) const
{
    NetworkStats delta{};
    delta.interface = curr.interface;

    const double dt = TimeDiffSec(curr.ts, prev.ts);

    if (dt <= 0.0) {
        return delta;
    }

    const uint64_t rx_bytes = curr.rx_bytes - prev.rx_bytes;
    const uint64_t tx_bytes = curr.tx_bytes - prev.tx_bytes;
    const uint64_t rx_packets = curr.rx_packets - prev.rx_packets;
    const uint64_t tx_packets = curr.tx_packets - prev.tx_packets;

    delta.rx_bytes_per_sec = static_cast<uint64_t>(rx_bytes / dt);
    delta.tx_bytes_per_sec = static_cast<uint64_t>(tx_bytes / dt);
    delta.rx_packets_per_sec = static_cast<uint64_t>(rx_packets / dt);
    delta.tx_packets_per_sec = static_cast<uint64_t>(tx_packets / dt);

    return delta;
}

DiskStats Calculators::CalculateDiskDelta(
    const DiskSnapshot& prev,
    const DiskSnapshot& curr,
    uint16_t sector_size
) const
{
    DiskStats delta{};
    delta.device = curr.device;

    const double dt = TimeDiffSec(curr.ts, prev.ts);

    if (dt <= 0.0) {
        return delta;
    }

    const uint64_t read_sectors = curr.read_sectors - prev.read_sectors;
    const uint64_t write_sectors = curr.write_sectors - prev.write_sectors;

    const uint64_t read_bytes = read_sectors * sector_size;
    const uint64_t write_bytes = write_sectors * sector_size;

    delta.read_bytes_per_sec = static_cast<uint64_t>(read_bytes / dt);
    delta.write_bytes_per_sec = static_cast<uint64_t>(write_bytes / dt);

    const uint64_t read_ms = curr.ms_reading - prev.ms_reading;
    const uint64_t write_ms = curr.ms_writing - prev.ms_writing;

    const double busy_ms = static_cast<double>(read_ms + write_ms);
    const double window_ms = dt * 1000.0;

    if (window_ms > 0.0) {
        delta.utilization_percent = (busy_ms / window_ms) * 100.0f;
    } else {
        delta.utilization_percent = 0.0f;
    }

    return delta;
}

ProcessData Calculators::CalculateProcessDelta(
    const ProcessSnapshot& prev,
    const ProcessSnapshot& curr,
    uint64_t total_mem,
    std::size_t cpu_count
) const
{
    ProcessData delta{};
    delta.state = curr.state;

    const double dt = TimeDiffSec(curr.ts, prev.ts);

    if (dt <= 0.0) {
        return delta;
    }

    const uint64_t cpu_diff =
        (curr.cpu_usage > prev.cpu_usage)
        ? (curr.cpu_usage - prev.cpu_usage)
        : 0;

    const double raw_cpu_percent = cpu_diff * 100.0 / dt;
    const double normalized_cpu_percent =
        cpu_count > 0 ? raw_cpu_percent / static_cast<double>(cpu_count) : 0.0;

    delta.cpu_percent = ClampPercent(normalized_cpu_percent);

    delta.memory_percent =
        total_mem > 0
        ? (curr.memory_bytes * 100.0f / total_mem)
        : 0.0f;

    return delta;
}