// TrustManager.h
#pragma once

#include <unordered_map>
#include <functional>
#include <salticidae/network.h>
#include <iostream>
#include "config.h"

class TrustManager {
public:
    // Define a structure to store information about trust between nodes.
    struct TrustInfo {
        salticidae::PeerId localPeer;  // ID of the peer node
        salticidae::PeerId remotePeer;  // ID of the node interacting with local node
        int Sat;  // satisfactory messages
        int Unsat;  // Unsatisfactory messages
        double Direct_Trust ;
    };

    // Custom hash function for std::pair<salticidae::PeerId, salticidae::PeerId>
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
    static std::unordered_map<std::pair<salticidae::PeerId, salticidae::PeerId>, TrustInfo, pair_hash> trustMap;

    // Methods to manipulate the trustMap
    static void updateTrust(const salticidae::PeerId& localPeer, const salticidae::PeerId& remotePeer, bool satisfactory);

    static void updateDirectTrust(const salticidae::PeerId& localPeer) ;

    


    // Add more methods as needed

        // Method to print trustMap
    static void printTrustMap();

private:
    // Private constructor to prevent instantiation
    TrustManager() = default;
};
