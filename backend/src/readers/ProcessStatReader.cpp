#include "readers/ProcessStatReader.hpp"
#include <fstream>
#include <string>


void ProcessStatReader::ReadMeta() {
   std::vector<std::uint32_t> new_pids = FindPidsInProc();

    pids = std::move(new_pids);

    process_metas.clear();

    for (const auto& pid : pids) {
        try {
            std::ifstream comm_file =
                OpenFile("/proc/" + std::to_string(pid) + "/comm");

            std::string name;
            std::getline(comm_file, name);

            process_metas.push_back({pid, name});
        } catch (const std::exception&) {
        }
    }
}

void ProcessStatReader::Snap() {
    process_snaps.clear();
    for (const auto& pid : pids) {
        try {
            process_snaps[pid] = SnapProcess(std::to_string(pid));
        } catch (const std::exception&) {
        }
    }
}

std::vector<ProcessMeta> ProcessStatReader::GetProcessMetas() const {
    return process_metas;
}

std::map<std::uint32_t, ProcessSnapshot> ProcessStatReader::GetProcessSnaps() const {
    return process_snaps;
}

std::vector<std::uint32_t> ProcessStatReader::FindPidsInProc() const {
    namespace fs = std::filesystem;

    std::vector<std::uint32_t> pids;

    for (const auto& entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory()) {
            continue;
        }

        std::string name = entry.path().filename().string();

        if (std::any_of(
            name.begin(),
            name.end(),
            [](char c) {
                return !std::isdigit(static_cast<unsigned char>(c));
            }
        )) {
            continue;
        }

        pids.push_back(std::stoul(name));
    }
    std::sort(pids.begin(), pids.end());
    return pids;
}

ProcessSnapshot ProcessStatReader::SnapProcess(const std::string& pid) const {
    std::ifstream file = OpenFile("/proc/" + pid + "/stat");
    std::string line;
    std::getline(file, line);
    std::string skip;
    ProcessSnapshot snap;
    std::istringstream ss(line);
    ss >> skip; // pid
    ss >> skip; // comm
    ss >> snap.state; // state
    for (int i = 0; i < 11; ++i) {
        ss >> skip; // skip to utime
    }
    ss >> snap.cpu_usage; // utime

    file.close();

    file = OpenFile("/proc/" + pid + "/status");
    while (std::getline(file, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream mem_ss(line);
            std::string label;
            mem_ss >> label >> snap.memory_bytes; // VmRSS
            snap.memory_bytes *= 1024; // convert to bytes
            break;
        }
    }
    file.close();

    snap.ts = std::chrono::steady_clock::now();
    return snap;
}

std::ifstream ProcessStatReader::OpenFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open " + path);
    }
    return file;
}