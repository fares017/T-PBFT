#include "node.h"

/** @file node.cpp
 * 
 * @brief Implementation of the Node class and its members.
*/

Node::Node(const salticidae::EventContext &ec, const Net::Config config, const salticidae::NetAddr paddr, const salticidae::PeerId pid) {
    // Assign a NetAddr and a PeerId.
    this->peerAddr = paddr;
    this->peerId = pid;

    // Create a network for this node.
    peerNet = std::make_unique<Net>(ec, config);

    // Start the network and listen to its assigned address.
    peerNet->start();
    peerNet->listen(peerAddr);
}

void Node::reg_handlers() {
    peerNet->reg_handler(salticidae::generic_bind(&Node::string_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::vote_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::ack_handler, this, _1, _2));

    peerNet->reg_handler(salticidae::generic_bind(&Node::operation_handler, this, _1, _2));

    peerNet->reg_unknown_peer_handler(salticidae::generic_bind(&Node::unknown_peer_handler, this, _1, _2));

    // peerNet->reg_conn_handler(salticidae::generic_bind(&Node::conn_handler, this, _1, _2));
    // peerNet->reg_error_handler(salticidae::generic_bind(&Node::error_handler, this, _1, _2, _3));
    // peerNet->reg_peer_handler(salticidae::generic_bind(&Node::peer_handler, this, _1, _2));
}

void Node::string_handler(MsgString &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got message:\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";

    peerNet->send_msg(MsgAck(), conn);
}

void Node::vote_handler(MsgVote &&msg, const Net::conn_t &conn) {
    cout << "\nNode " << get_id() << " got message:\n";
    cout << "\tMessage ID: " << std::to_string(msg.id) << "\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";

    if (pidmap.size() != peers.size()) {
        for (const auto &pid: peers) {
            pidmap[pid] = false;
        }
    }

    // Send ACK (for testing)
    // peerNet->send_msg(MsgAck(), conn);

    // cout << "Node " << get_id() << " knows " << peers.size() << " peers:\n";
    // for (const auto &replica: peers) {
    //     cout << "\t" << replica.to_hex() << "\n";
    // }

    // cout << "Node " << get_id() << " broadcasting to " << peers.size() << " peers\n";
    // for (const auto &replica: peers) {
    //     cout << "\t" << replica.to_hex() << "\n";
    //     peerNet->send_msg(MsgAck(), replica);
    // }

    pidmap[conn->get_peer_id()] = true;
    num_acks += 1;

    // peerNet->multicast_msg(msg, peers);
    // cout << "Node " << get_id() << " broadcasting to " << peers.size() << " peers\n";
    // for (const auto &replica: peers) {
    //     cout << "\t" << replica.to_hex() << "\n";
    //     peerNet->send_msg(msg, replica);
    // }

    for (const auto &replica: pidmap) {
        if (!replica.second) {
            cout << "*Node " << get_id() << " broadcasting to " << peers.size() << " peers\n";
            peerNet->multicast_msg(MsgVote(msg.id, msg.message), peers);
            return;
        }
    }

    if (num_acks == peers.size()) {
        cout << "->Node " << get_id() << " got enough messages!\n";
        // peerNet->send_msg(MsgAck(), );
    }
}

void Node::operation_handler(MsgOp &&msg, const Net::conn_t &conn) {
    peerNet->send_msg(MsgAck(), conn);

    for (const auto &pid: peers) {
        pidmap[pid] = false;
    }

    cout << "Node " << get_id() << " got operation request: \n";
    cout << "\tWith sequence id " << msg.sequence << "\n";
    cout << "\tAnd operation " << msg.operation << "\n";

    peerNet->multicast_msg(MsgVote(msg.sequence, msg.operation), peers);
}

void Node::ack_handler(MsgAck &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got ACK:\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
    num_acks += 1;
}

void Node::set_peers(std::vector<salticidae::PeerId> peers) {
    this->peers = peers;

    #ifdef DEBUG
    cout << "Node " << get_id() << " knows " << peers.size() << " peers:\n";
    for (const auto &replica: peers) {
        cout << "\t" << replica.to_hex() << "\n";
    }
    #endif
}

Node::Node() {}

void Node::unknown_peer_handler(const salticidae::NetAddr &addr, const salticidae::X509 *cert) {
    #ifdef DEBUG
    cout << "Unknown peer trying to connect with address: " << (std::string)addr;
    #endif

    salticidae::PeerId pid{addr};

    #ifdef DEBUG
    cout << " and PeerId " << pid.to_hex() << "\n";
    #endif

    peerNet->add_peer(pid);
    peerNet->set_peer_addr(pid, addr);

    #ifdef DEBUG
    cout << "Peer " << pid.to_hex() << " (" << (std::string)addr << ") added!\n";
    #endif
}

bool Node::conn_handler(const ConnPool::conn_t &conn, bool connected) {
    #ifdef DEBUG
    cout << "New Connection:\n";
    cout << "\tterminated? " << conn->is_terminated() << "\n";
    cout << "\taddress: " << (std::string)conn->get_addr() << "\n";
    cout << "\tmode: " << conn->get_mode() << "\n";
    #endif

    // if (connected) {
    //     if (conn->get_mode() == ConnPool::Conn::ACTIVE) {}
    // }

    return true;
}

void Node::peer_handler(const Net::conn_t &conn, bool connected) {
    #ifdef DEBUG
    cout << "Peer connected: " << connected << "\n";
    #endif
}

void Node::error_handler(std::__exception_ptr::exception_ptr exception, bool idk, int huh) {
    #ifdef DEBUG
    cout << "ERROR!\n";
    #endif
}
