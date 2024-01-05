#include "node.h"
#include "TrustManager.h"  // Include the TrustManager header
#include <string>


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
    peerNet->reg_handler(salticidae::generic_bind(&Node::prepare_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::precommit_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::preprepare_handler, this, _1, _2));
    // Register handlers for the different message types that we have now.
    peerNet->reg_handler(salticidae::generic_bind(&Node::request_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::forward_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::result_handler, this, _1, _2));
    // peerNet->reg_handler(salticidae::generic_bind(&Node::reply_handler, this, _1, _2));

    peerNet->reg_unknown_peer_handler(salticidae::generic_bind(&Node::unknown_peer_handler, this, _1, _2));

    peerNet->reg_conn_handler(salticidae::generic_bind(&Node::conn_handler, this, _1, _2));
    // peerNet->reg_error_handler(salticidae::generic_bind(&Node::error_handler, this, _1, _2, _3));
    // peerNet->reg_peer_handler(salticidae::generic_bind(&Node::peer_handler, this, _1, _2));
}

void Node::string_handler(MsgString &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got message:\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
    std::hash<std::string> msghash;
    int hashvalue = msghash(msg.message);
    uint order = 1;
    peerNet->multicast_msg(MsgPrepare(order, msg.message, hashvalue), peers);
    peerNet->send_msg(MsgAck(), conn);
}

void Node::prepare_handler(MsgPrepare &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got pre-prepare message from " << conn->get_peer_id().to_hex() << "\n";

    std::hash<std::string> messagehash;
    int hashvalue = messagehash(msg.message);


    // Create PeerId for local and remote peers.
    salticidae::PeerId localPeer = peerId;
    salticidae::PeerId remotePeer = conn->get_peer_id();

    // Update trust information using TrustManager.
    TrustManager::updateTrust(localPeer, remotePeer, (hashvalue == msg.msghash));



    if (hashvalue == msg.msghash) {

        // Broadcast or multicast as needed.
        peerNet->multicast_msg(MsgPrecommit(msg.order, msg.message, msg.msghash), peers);
    } 


}



void Node::precommit_handler(MsgPrecommit &&msg, const Net::conn_t &conn){
    cout << "Node " << get_id() << " got prepare message from " << conn->get_peer_id().to_hex()<<"\n";
    prepared_message.push_back(msg);
    unsigned long fault_tolerance = ((2*nodes) - 2) / 3;
    if(prepared_message.size() > fault_tolerance){
        for(const auto &pid : peers) {
            cout<<"Sending message "<<pid.to_hex()<<"\n";
        }
        //prepared_message.remove(msg);

                // Print the trustMap
        cout << "TrustMap contents:\n";
        // Print the trustMap
        TrustManager::printTrustMap();
    } 
}

void Node::preprepare_handler(MsgPreprepare &&msg, const Net::conn_t &conn) {
    cout << "Primary Node " << get_id() << " got message:\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
    std::hash<std::string> msghash;
    int hashvalue = msghash(msg.message);
    uint order = 1;
    peerNet->multicast_msg(MsgPrepare(order, msg.message, hashvalue), peers);
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


/*
    Changes here
*/
void Node::request_handler(MsgRequest &&msg, const Net::conn_t &conn) {
    // Display message content to the user.
    cout << "Node " << get_id() << " got request message: \n";
    cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\tWith a = " << msg.a << "\n";
    cout << "\tAnd  b = " << msg.b << "\n";

    // Set the id from the client for this node or else it is lost.
    client_id = conn->get_peer_id();

    // Extract integers from the message and fill the internal state with them.
    a = msg.a;
    b = msg.b;

    // Since there is only one other peer it is index 0.
    // Send then the Forward message to the second server.
    peerNet->send_msg(MsgForward(msg.a, msg.b), peers[0]);
}

void Node::forward_handler(MsgForward &&msg, const Net::conn_t &conn) {
    // Extract message contents, this time as local variables.
    int a = msg.a;
    int b = msg.b;

    // Let the user know of the message contents.
    cout << "Node " << get_id() << " got forward message: \n";
    cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\tWith a = " << a << "\n";
    cout << "\tAnd  b = " << b << "\n";

    // Compute the resulting c to send back to the first server.
    int c = a + b;

    // Send we send back to the first server, from where we got it from, we can use the conn argument.
    cout << "\t-> Node " << get_id() << " answering to " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\t\tWith c = " << c << "\n";

    // Send the Result message to the first server.
    peerNet->send_msg(MsgResult(c), conn);
}

void Node::result_handler(MsgResult &&msg, const Net::conn_t &conn) {
    // Extract received c.
    int c = msg.c;
    cout << "Node " << get_id() << " got result message: \n";
    cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\tWith c = " << c << "\n";

    // Get the client that connected via the client_id member variable.
    salticidae::PeerId client = client_id;
    // Set the client_id member variable to itself to signify that no client recently connect.
    // aka reset the internal state.
    client_id = peerId;

    cout << "\tChecking c...\n";

    // Compute c from the member variable that we stored at the very beginning.
    int check_c = a + b;
    if (c == check_c) {
        // Reset a and b member variables.
        a = 0;
        b = 0;


        cout << "\tc matches! Sending good reply to " << client.to_hex().substr(0, 10) << "\n";
        // The c is correctly computed so we send it to the client that requested the computation.
        peerNet->send_msg(MsgReply(c, false), client);
    } else {
        cout << "\tc DOES NOT match! Sending failure reply to " << client.to_hex().substr(0, 10) << "\n";
        peerNet->send_msg(MsgReply(0, true), client);
    }
}


// We technically do not need this since only the client should receive reply messages.
void Node::reply_handler(MsgReply &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got reply message: \n";
    cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\tWith c = " << msg.c << "\n";
    cout << "\tAnd  fault = " << msg.fault << "\n";
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
