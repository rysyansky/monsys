#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <chrono>

struct NetworkSnapshot {
    std::string interface;

    std::uint64_t rx_bytes;
    std::uint64_t tx_bytes;
    std::uint64_t rx_packets;
    std::uint64_t tx_packets;


    std::chrono::steady_clock::time_point ts;
};

class NetworkStatReader{
    std::vector<NetworkSnapshot> interfaces_snaps = {};
public:
    void Read();
    std::vector<NetworkSnapshot> GetNetworkSnapshots() const;
};