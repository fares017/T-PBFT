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

    // Iterate over the number of nodes and create nodes.
    for (uint i = 0; i < NUM_NODES; i++) {
        std::string address_string = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + i);
        salticidae::NetAddr peer_addr(address_string);
        salticidae::PeerId pid{peer_addr};

        // Add the peerId to the vector of known ids.
        peers[i] = pid;

        // Create node/peer with its settings.
        nodes[i] = Node(ec, config, peer_addr, pid);
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
    }
}

