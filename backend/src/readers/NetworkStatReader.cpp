#include "readers/NetworkStatReader.hpp"
#include <algorithm>

void NetworkStatReader::Read() {
    std::ifstream net_dev("/proc/net/dev");
    if (!net_dev.is_open()) {
        throw std::runtime_error("Failed to open /proc/net/dev");
    }

    std::string line;
    std::getline(net_dev, line);
    std::getline(net_dev, line);

    interfaces_snaps.clear();

    while (std::getline(net_dev, line)) {
        std::istringstream iss(line);
        std::string iface;
        std::uint64_t rx_bytes, tx_bytes, rx_packets, tx_packets;
        std::string skip;

        iss
            >> iface
            >> rx_bytes
            >> rx_packets
            >> skip
            >> skip
            >> skip
            >> skip
            >> skip
            >> skip
            >> tx_bytes
            >> tx_packets;

        if (!iface.empty() && iface.back() == ':') {
            iface.pop_back();
        }

        NetworkSnapshot snap;
        snap.interface = iface;
        snap.rx_bytes = rx_bytes;
        snap.tx_bytes = tx_bytes;
        snap.rx_packets = rx_packets;
        snap.tx_packets = tx_packets;

        snap.ts = std::chrono::steady_clock::now();

        interfaces_snaps.push_back(snap);
    }

    std::sort(interfaces_snaps.begin(), interfaces_snaps.end(),
        [](const NetworkSnapshot& a, const NetworkSnapshot& b) {
            return a.interface < b.interface;
        });
}

std::vector<NetworkSnapshot> NetworkStatReader::GetNetworkSnapshots() const {
    return interfaces_snaps;
}