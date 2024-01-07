#include "handler.h"

/** @file handler.cpp
 * 
 * @brief Implementation of the Handler class and its members.
*/

Handler::Handler(const salticidae::EventContext &ec, const Net::Config config) {
    // Resize vector containing the known nodes.
    nodes.resize(NUM_NODES);

    // Create a vector with the peerId from all the nodes.
    std::vector<salticidae::PeerId> peers;
    peers.resize(NUM_NODES);
    RSAKeyGenerator keyGenerator;
    std::unordered_map<salticidae::PeerId, CryptoPP::RSA::PublicKey> publicKeysID;
    // CryptoPP::RSA::PrivateKey groupPrivateKey = keyGenerator.GeneratePrivateKey();
    // CryptoPP::RSA::PublicKey groupPublicKey = keyGenerator.GeneratePublicKey(groupPrivateKey);
    // std::vector<Node> nodes_replica;
    // nodes_replica.resize(NUM_NODES);

    // Iterate over the number of nodes and create nodes.
    for (uint i = 0; i < NUM_NODES; i++) {
        std::string address_string = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + i);
        salticidae::NetAddr peer_addr(address_string);
        salticidae::PeerId pid{peer_addr};

        // Add the peerId to the vector of known ids.
        peers[i] = pid;
        //Assign RSA public key private key
        CryptoPP::RSA::PrivateKey privateKey = keyGenerator.GeneratePrivateKey();
        CryptoPP::RSA::PublicKey publicKey = keyGenerator.GeneratePublicKey(privateKey);
        publicKeysID[pid] = publicKey;

        // Create node/peer with its settings.
        nodes[i] = Node(ec, config, peer_addr, pid, privateKey, publicKey);

        // Register the message callbacks of the node.
        nodes[i].reg_handlers();
    }

    // Connect nodes' network to each other.
    for (size_t i = 0; i < NUM_NODES; i++) {
        for (size_t j = 0; j < NUM_NODES; j++) {
            if (i != j) {
                // Get network of each node.
                auto &nodeNet = nodes[i].peerNet;
                // Get NetAddr for each node.
                auto &peer_addr = nodes[j].peerAddr;
                // Get peerId for each node.
                auto &pid = nodes[j].peerId;

                // Add a peer for this node to be known.
                nodeNet->add_peer(pid);
                // Set the known peer's address.
                nodeNet->set_peer_addr(pid, peer_addr);
                // Connect the nodes together.
                nodeNet->conn_peer(pid);
            }
        }
    }

    // For each node add the testing client as a known peer.
    for (size_t i = 0; i < NUM_NODES; i++) {
        // Create temporary vector to hold other peers.
        std::vector<salticidae::PeerId> temp_peers;
        // Copy peers to the temporary vector that are not itself.
        std::copy_if(peers.begin(), peers.end(), std::back_inserter(temp_peers), [peers, i](const salticidae::PeerId& t) { return t != peers[i]; });

        #ifdef DEBUG
        cout << "temp replicas\n";
        for (const auto &replica: temp_peers) {
            cout << "\t" << replica.to_hex() << "\n";
        }
        #endif

        nodes[i].set_peers(temp_peers);
        nodes[i].set_publickeys(publicKeysID);
        //nodes[i].set_group_publicKey(groupPublicKey);
    }


        // Initialize TrustMap with random Sat and Unsat values
    for (size_t i = 0; i < NUM_NODES; i++) {
        for (size_t j = 0; j < NUM_NODES; j++) {
            if (i != j) {
                salticidae::PeerId localPeer = nodes[i].peerId;
                salticidae::PeerId remotePeer = nodes[j].peerId;

                // Generate random Sat and Unsat values
                int randSat = rand() % 11;  // Random number between 0 and 10
                int randUnsat = rand() % 11;

                // int randSat = 0;  
                // int randUnsat = 0;

                // Check if the pair already exists in trustMap
                auto key = (localPeer < remotePeer) ? std::make_pair(localPeer, remotePeer) : std::make_pair(remotePeer, localPeer);
                auto it = TrustManager::trustMap.find(key);

                if (it != TrustManager::trustMap.end()) {
                    // Pair already exists, update trust values.
                   
                } else {
                    // Pair doesn't exist, create a new TrustInfo.
                    TrustManager::TrustInfo trustInfo;
                    trustInfo.localPeer = localPeer;
                    trustInfo.remotePeer = remotePeer;
                    trustInfo.Sat = randSat;
                    trustInfo.Unsat = randUnsat;

                    // Insert the new pair into the map.
                    TrustManager::trustMap[key] = trustInfo;
                }
            }
        }
    }


        // Update Direct_Trust values
    for (size_t i = 0; i < NUM_NODES; i++) {
        salticidae::PeerId localPeer = nodes[i].peerId;
        TrustManager::updateDirectTrust(localPeer);
    }

     
    TrustManager::printInitialTrustMap() ;



        // Initialize globalTrustMap with random Sat and Unsat values
  
        for (size_t i = 0; i < NUM_NODES; i++) {
          
                salticidae::PeerId node = nodes[i].peerId;
                TrustManager::globalTrustMap[node] = 1.0 / NUM_NODES;

        }

                // Initialize globalTrustMap with random Sat and Unsat values
  
        for (size_t i = 0; i < NUM_NODES; i++) {
          
                salticidae::PeerId node = nodes[i].peerId;
                TrustManager::updateGlobalTrust(node);

        }
    
    TrustManager::printGlobalTrustMap() ;

    TrustManager::getConsensusGroup(D2) ;
    TrustManager::printConsensusGroup() ;

    TrustManager::getPrimaryGroup(M2) ;
    TrustManager::printPrimaryGroup() ;
   // RSAKeyGenerator::assignGroupKey();
//    std::vector<salticidae::PeerId> primary_group = TrustManager::primaryGroup;
//    for (size_t i = 0; i < NUM_NODES; i++)
//     {
//         if (std::find(primary_group.begin(), primary_group.end(), nodes[i].peerId) != primary_group.end())
//         {
//             nodes[i].set_group_privateKey(groupPrivateKey);
//         }
//         nodes[i].set_group_publicKey(groupPublicKey);
//     }
    RSAKeyGenerator::randomValueGenerator();
    RSAKeyGenerator::changeGroupKey();
}


