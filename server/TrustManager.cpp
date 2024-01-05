// TrustManager.cpp
#include "TrustManager.h"
#include <iostream>

// Definition of the static trustMap
std::unordered_map<std::pair<salticidae::PeerId, salticidae::PeerId>, TrustManager::TrustInfo, TrustManager::pair_hash> TrustManager::trustMap;
// Definition of the static globalTrustMap
std::unordered_map<salticidae::PeerId, double> TrustManager::globalTrustMap;
// Declaration of the consensus group
std::vector<salticidae::PeerId> TrustManager::consensusGroup;
// Declaration of the primary group
std::vector<salticidae::PeerId> TrustManager::primaryGroup;

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



void TrustManager::updateGlobalTrust(const salticidae::PeerId& localPeer) {
    // Initialize global trust to 1 / NUM_NODES
    
    double newGlobalTrust = globalTrustMap[localPeer] ;

    // Iterate over the trustMap
    for (const auto& entry : trustMap) {
        const auto& key = entry.first;
        const auto& value = entry.second;

        // Check if localPeer is one of the nodes and it's not the same node
        if ((key.first == localPeer || key.second == localPeer) && key.first != key.second) {
            salticidae::PeerId remotePeer;

            // Determine the remote peer based on the conditions
            if (key.first == localPeer) {
                remotePeer = key.second;
            } else {
                remotePeer = key.first;
            }

            // Use remotePeer to calculate the contribution to global trust
             double remotePeerGlobalTrust = globalTrustMap[remotePeer] ;
            newGlobalTrust += remotePeerGlobalTrust * value.Direct_Trust;

        }
    }

    globalTrustMap[localPeer] = newGlobalTrust;

}




void TrustManager::getConsensusGroup(double d) {
    // Ensure that D is within the valid range (0 < d < 1)
    d = std::max(std::min(d, 1.0), 0.0);

    // Clear the existing consensusGroup
    consensusGroup.clear();

    // Create a vector of pairs (PeerId, globalTrust) for sorting
    std::vector<std::pair<salticidae::PeerId, double>> trustVector(globalTrustMap.begin(), globalTrustMap.end());

    // Sort the vector based on global trust in descending order
    std::sort(trustVector.begin(), trustVector.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Determine the number of nodes to include in the consensus group
    size_t numNodesInConsensus = static_cast<size_t>(d * NUM_NODES);

    // Populate the consensusGroup with the top nodes
    for (size_t i = 0; i < numNodesInConsensus; ++i) {
        consensusGroup.push_back(trustVector[i].first);
    }
}


void TrustManager::getPrimaryGroup(double m) {
    // Ensure m is within the valid range (0 to 1)
    m = std::max(0.0, std::min(1.0, m));

    // Calculate the number of nodes to select based on the percentage
    size_t numNodesToSelect = static_cast<size_t>(consensusGroup.size() * m);

    // Sort the consensusGroup based on global trust (assuming you have a comparator function)
    std::sort(consensusGroup.begin(), consensusGroup.end(), [](const salticidae::PeerId& a, const salticidae::PeerId& b) {
        return globalTrustMap[a] > globalTrustMap[b];
    });

    // Clear the existing primaryGroup
    primaryGroup.clear();

    // Copy the top m% nodes to the primaryGroup
    primaryGroup.insert(primaryGroup.end(), consensusGroup.begin(), consensusGroup.begin() + numNodesToSelect);
}









 // Print the value of TrustMap
void TrustManager::printTrustMap() {
    std::cout << "TrustMap contents:\n";
    for (const auto &entry : trustMap) {
        const auto &key = entry.first;
        const auto &value = entry.second;

        std::cout << "Local Peer ID: " << key.first.to_hex() << ", Remote Peer ID: " << key.second.to_hex()
             << ", Number of Satisfactory transactions: " << value.Sat << "\n"
             << ", Number of Unsatisfactory transactions: " << value.Unsat << "\n";
    }
}

 // Print the Initial value of TrustMap
void TrustManager::printInitialTrustMap() {
    std::cout << "TrustMap Initial contents:\n";
    for (const auto &entry : trustMap) {
        const auto &key = entry.first;
        const auto &value = entry.second;

        std::cout << "Local Peer ID: " << key.first.to_hex() <<  "\n"
             <<  ", Remote Peer ID: " << key.second.to_hex() << "\n"
             << ", Number of Satisfactory transactions: " << value.Sat << "\n"
             << ", Number of Unsatisfactory transactions: " << value.Unsat << "\n"
             << ", Direct Trust Value " << value.Direct_Trust << "\n";
    }
}





void TrustManager::printGlobalTrustMap() {
    std::cout << "Global Trust Map contents:\n";
    for (const auto& entry : globalTrustMap) {
        const auto& key = entry.first;
        const auto& value = entry.second;

        std::cout << "Node ID: " << key.to_hex() << ", Global Trust: " << value << "\n";
    }
}


void TrustManager::printConsensusGroup() {
    std::cout << "Consensus Group contents:\n";
    for (const auto& node : consensusGroup) {
        std::cout << "Node ID: " << node.to_hex() << "\n";
    }
}


void TrustManager::printPrimaryGroup() {
    std::cout << "Primary Group contents:\n";
    for (const auto& node : primaryGroup) {
        std::cout << "Node ID: " << node.to_hex() << "\n";
    }
}