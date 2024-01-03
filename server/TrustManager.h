// TrustManager.h
#pragma once

#include <unordered_map>
#include <functional>
#include <salticidae/network.h>

// Define a structure to store information about trust between nodes.
struct TrustInfo {
    std::string nodeId;  // ID of the current node
    salticidae::PeerId peerId;  // ID of the peer node
    uint64_t Sat;  // statisfactory messages
    uint64_t Unsat;  // Unsatisfactory messages
};

// Custom hash function for std::pair<std::string, salticidae::PeerId>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Use a simple combination hash function
        return h1 ^ h2;
    }
};

// Declaration of the global trustMap as extern.
extern std::unordered_map<std::pair<std::string, salticidae::PeerId>, TrustInfo, pair_hash> trustMap;



