// TrustManager.cpp
#include "TrustManager.h"
#include <iostream>

// Definition of the static trustMap
std::unordered_map<std::pair<salticidae::PeerId, salticidae::PeerId>, TrustManager::TrustInfo, TrustManager::pair_hash> TrustManager::trustMap;

// Implementation of the methods
void TrustManager::updateTrust(const salticidae::PeerId& localPeer, const salticidae::PeerId& remotePeer, bool satisfactory) {
    // Ensure a canonical order for the pair
    auto key = (localPeer < remotePeer) ? std::make_pair(localPeer, remotePeer) : std::make_pair(remotePeer, localPeer);

    // Find the TrustInfo in the trustMap
    auto it = trustMap.find(key);

    if (it != trustMap.end()) {
        // Pair already exists, update trust values.
        if (satisfactory) {
            it->second.Sat++;
        } else {
            it->second.Unsat++;
        }
    } else {
        // Pair doesn't exist, create a new TrustInfo.
        TrustInfo trustInfo;
        trustInfo.localPeer = localPeer;
        trustInfo.remotePeer = remotePeer;
        trustInfo.Sat = satisfactory ? 1 : 0;
        trustInfo.Unsat = satisfactory ? 0 : 1;

        // Insert the new pair into the map.
        trustMap[key] = trustInfo;
    }
}




void TrustManager::printTrustMap() {
    std::cout << "TrustMap contents:\n";
    for (const auto &entry : trustMap) {
        const auto &key = entry.first;
        const auto &value = entry.second;

        std::cout << "Local Peer ID: " << key.first.to_hex() << ", Remote Peer ID: " << key.second.to_hex()
             << ", Number of Satisfactory transactions: " << value.Sat
             << ", Number of Unsatisfactory transactions: " << value.Unsat << "\n";
    }
}




// double TrustManager::calculateDirectTrust(const salticidae::PeerId& localPeer) {
//     double  C = 0;
//     uint64_t Stotal = 0;

//     for (const auto& entry : trustMap) {
//         const auto& key = entry.first;
//         const auto& value = entry.second;

//         if (key.first == localPeer || key.second == localPeer ) {
//             // Calculate sat(i, j) - unsat(i, j)
//             int S = value.Sat - value.Unsat;

//             // Update total satisfactory transactions
//             Stotal += std::max(S, 0);

//         }
//     }

//     if (Stotal == 0) {
//         // Set Cij to N if Stotal is zero
//         C = 1/NUM_NODES;  // Assuming trustMap contains all possible remotePeers
//     } else {
//         // Calculate maxS(Sij, 0) / Stotal for each remotePeer
//         for (const auto& entry : trustMap) {
//             const auto& key = entry.first;
//             const auto& value = entry.second;

//             if (key.first == localPeer || key.second == localPeer ){
//                  int S = value.Sat - value.Unsat;
//                 uint64_t C = std::max(S, 0)  / Stotal;


//             }
//         }
//     }

//     return C;
// }



void TrustManager::updateDirectTrust(const salticidae::PeerId& localPeer) {
    uint64_t Stotal = 0;

    // Calculate total satisfactory transactions involving localPeer
    for (const auto& entry : trustMap) {
        const auto& key = entry.first;
        const auto& value = entry.second;

        if (key.first == localPeer || key.second == localPeer) {
            // Calculate sat(i, j) - unsat(i, j)
            int S = value.Sat - value.Unsat;

            // Update total satisfactory transactions
            Stotal += std::max(S, 0);
        }
    }


    // Update Direct_Trust values for each remotePeer
    for (auto& entry : trustMap) {
        auto& key = entry.first;
        auto& value = entry.second;

        if (key.first == localPeer || key.second == localPeer) {
            // Retrieve the Direct_Trust value from TrustInfo
              int S = value.Sat - value.Unsat;
            double directTrust = (Stotal == 0) ? 1.0 / NUM_NODES : std::max(S, 0) / static_cast<double>(Stotal);

            // Update the Direct_Trust value in TrustInfo
            value.Direct_Trust = directTrust;
        }
    }
}
