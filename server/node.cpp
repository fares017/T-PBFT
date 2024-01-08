#include "node.h"

/** @file node.cpp
 * 
 * @brief Implementation of the Node class and its members.
*/
int Node::total_view_change_requests = 0;
std::chrono::steady_clock::time_point startTime;
std::chrono::steady_clock::duration elapsedTime;
bool timerPaused = false;
//std::queue<std::pair<salticidae::PeerId, std::string>> requestQueue;
//QueueRequest queueRequest;

Node::Node(const salticidae::EventContext &ec, const Net::Config config, const salticidae::NetAddr paddr, const salticidae::PeerId pid, const CryptoPP::RSA::PrivateKey prKey, const CryptoPP::RSA::PublicKey puKey) {
    // Assign a NetAddr and a PeerId.
    this->peerAddr = paddr;
    this->peerId = pid;
    this->privateKey = prKey;
    this->publicKey = puKey;
    //this->queueRequest;
   // QueueRequest queueRequest;
    // Create a network for this node.
    peerNet = std::make_unique<Net>(ec, config);

    // Start the network and listen to its assigned address.
    peerNet->start();
    peerNet->listen(peerAddr);
}

void Node::reg_handlers() {
    peerNet->reg_handler(salticidae::generic_bind(&Node::ack_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::prepare_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::preprepare_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::group_request_handler, this, _1, _2)); 
    peerNet->reg_handler(salticidae::generic_bind(&Node::primary_consensus_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::primary_verified_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::send_reply_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::process_next_request, this, _1, _2));

    peerNet->reg_unknown_peer_handler(salticidae::generic_bind(&Node::unknown_peer_handler, this, _1, _2));

    peerNet->reg_conn_handler(salticidae::generic_bind(&Node::conn_handler, this, _1, _2));
}


void Node::set_publickeys(std::unordered_map<salticidae::PeerId, CryptoPP::RSA::PublicKey> publicKeysID) {
    this->publicKeysID = publicKeysID;
}
void Node::set_primarygroup(std::vector<salticidae::PeerId> primary_group) {
    this->primary_group = primary_group;
}
void Node::set_consensusgroup(std::vector<salticidae::PeerId> consensus_group) {
    this->consensus_group = consensus_group;
}
void Node::set_allNodes(std::vector<salticidae::PeerId> allNodes) {
    this->allNodes = allNodes;
}

void Node::startTimer() {
    if (timerPaused) {
        startTime = std::chrono::steady_clock::now();
        timerPaused = false;
    }
}

void Node::pauseTimer() {
    if (!timerPaused) {
        // Calculate the elapsed time and store it
        elapsedTime += std::chrono::steady_clock::now() - startTime;
        timerPaused = true;
    }
}

void Node::resetTimer() {
    elapsedTime = std::chrono::steady_clock::duration::zero();
    timerPaused = false;
}


double Node::getElapsedTimeInSeconds() const {
    // Calculate the total elapsed time in seconds
    auto totalElapsedTime = std::chrono::steady_clock::now() - startTime;

    // Convert to seconds
    return std::chrono::duration_cast<std::chrono::duration<double>>(totalElapsedTime).count();
}

void Node::group_request_handler(MsgGroup &&msg, const Net::conn_t &conn) { 
    cout <<"Node " << get_id() << "got client request message from " << conn->get_peer_id().to_hex() << "\n";
    std::pair<salticidae::PeerId, std::string> client_request(conn->get_peer_id(), msg.message);
    requestQueue.push_back(client_request);
    peerNet->send_msg(MsgAck(), conn->get_peer_id());
    //}
    bool check = requestQueue.empty();
    cout << "check : " << check << std::endl;
    if (check == false && busy == false) {
        std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
        salticidae::PeerId client_conn_id = nextRequest.first;
        std::string client_conn_request = nextRequest.second;
        cout << client_conn_request << " is message \n";
        cout << client_conn_id.to_hex() << "is conn id \n";
        Node::process_group_request(client_conn_request, client_conn_id);
        busy=true;
    }
} 
void Node::process_group_request(const std::string msg, const salticidae::PeerId conn) { 
    resetTimer();
    startTimer();
        cout <<"Node " << get_id() << " processing client request message from " << conn.to_hex() << "\n";  
        RSAKeyGenerator keyManager;
        std::string signature;
        client_request = msg;
        client_id = conn;
        std::vector<salticidae::PeerId> primary = TrustManager::primaryGroup;
        int val = RSAKeyGenerator::getRandomValue();
        // cout << "Random value is " << val << "\n";
        if (peerId == primary[val])
        {
            cout << "\tMessage: " << client_request << "\n";
            RSAKeyGenerator::incrementOrderNumber(); 
            int currentOrderNumber = RSAKeyGenerator::getOrderNumber();
            std::string orderNumber = std::to_string(currentOrderNumber);
            //std::string message = msg.message;
            std::string combinedData = orderNumber + client_request;
            signature = keyManager.SignMessage(combinedData, privateKey);
            for (const auto &pid: primary) {
                if (pid != peerId) {
                    peerNet->send_msg(MsgPrimaryConsensus(client_request, signature, orderNumber), pid);
                }
            }
        }
        cout << "\tMessage: " << client_request << "\n";
        //peerNet->send_msg(MsgAck(), conn); 
}

void Node::primary_consensus_handler(MsgPrimaryConsensus &&msg, const Net::conn_t &conn) {
    cout <<"Node " << get_id() << "got signed message from " << conn->get_peer_id().to_hex() << "\n";
    RSAKeyGenerator keyManager;
    std::string orderNumber = msg.orderNumber;
    salticidae::PeerId connect = conn->get_peer_id();
    CryptoPP::RSA::PublicKey pubKey = publicKeysID[connect];
    std::string combinedData = orderNumber + client_request;
    //cout<< "client request " << client_request << "\n";
    bool verify = keyManager.VerifySignature(combinedData, msg.signature, pubKey);
    if (verify == true)
    {
        peerNet->send_msg(MsgPrimaryVerified(verify), conn);
    } else {
        cout << "Verification failed, node might be faulty \n";
        peerNet->send_msg(MsgPrimaryVerified(false), conn);
    }    
}
void Node::primary_verified_handler(MsgPrimaryVerified &&msg, const Net::conn_t &conn) {
    cout <<"Node " << get_id() << "got verification message from " << conn->get_peer_id().to_hex() << "\n";
    verified_message.push_back(msg.verified);
    int verifyTrue = 0;
    int verifyFalse = 0;
    RSAKeyGenerator keyManager;
    std::vector<salticidae::PeerId> primary = TrustManager::primaryGroup;
    int primary_size = primary.size();
    if (verified_message.size() == primary.size() - 1) {
        for (bool v : verified_message) {
            if (v == true) {
                verifyTrue++;
            } else {
                verifyFalse++;
            }
        }
    }
    if (verifyTrue >= verifyFalse && verifyTrue != 0) {
        //add send msg
        cout << "Verified \n"; 
        int currentOrderNumber = RSAKeyGenerator::getOrderNumber();
        cout << currentOrderNumber << " is the order number \n"; 
        currentOrderNumber = RSAKeyGenerator::getOrderNumber();
        std::string orderNumber = std::to_string(currentOrderNumber);
        std::string combinedData = orderNumber + client_request;
        try {
            CryptoPP::RSA::PrivateKey groupPrivateKey = RSAKeyGenerator::getGroupPrivateKey(peerId);
            std::string signature = keyManager.SignMessage(combinedData, groupPrivateKey);
            std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
            for (const auto &pid: consensus) {
                if (std::find(primary.begin(), primary.end(), pid) == primary.end()) {
                    peerNet->send_msg(MsgPreprepare(client_request, signature, orderNumber), pid);
                }
            }
            verified_message.clear();
        } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        }
    } else if (verifyFalse == primary_size-1 ) {
        requestQueue.pop_front();
        requestQueue.size();
        client_request = "";
        busy = false;
        bool check = requestQueue.empty();
        peerNet->multicast_msg(MsgNextRequest(true), peers);
        cout << "Check queue 2: " << check << ".Size is:  " <<requestQueue.size()<< "\n";
        if(!check && !busy) {
            std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
            salticidae::PeerId client_conn_id = nextRequest.first;
            std::string client_conn_request = nextRequest.second;
        // MsgGroup receivedMsg = client_conn_request;
            Node::process_group_request(client_conn_request, client_conn_id);
            //queueRequest::ifBusy = true;
            busy=true;
        }
    }

}

void Node::preprepare_handler(MsgPreprepare &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got pre-prepare message from " << conn->get_peer_id().to_hex()<<"\n";
    pauseTimer();
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
    RSAKeyGenerator keyManager;
    std::string message = msg.message;
    std::string orderNumber = msg.orderNumber;
    std::string combinedData = orderNumber + client_request;
    CryptoPP::RSA::PublicKey groupPublicKey = RSAKeyGenerator::getGroupPublicKey();
    bool verify = keyManager.VerifySignature(combinedData, msg.signature, groupPublicKey);
    cout << "bool value verify : " << verify << std::endl;
    if (verify == true) {
        cout << "Verified, sending to peers \n";
        std::string peer_signature = keyManager.SignMessage(combinedData, privateKey);
        std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
        for (const auto &pid: consensus) {
            if(pid != peerId) {
                peerNet->send_msg(MsgPrepare(message, orderNumber, msg.signature, peer_signature), pid);
            }
        }
    } else {
        cout << "Verification failed \n";
    }
}

void Node::prepare_handler(MsgPrepare &&msg, const Net::conn_t &conn){
    cout << "Node " << get_id() << " got prepare message from " << conn->get_peer_id().to_hex()<<"\n";
    RSAKeyGenerator keyManager;
    std::string combinedData = msg.orderNumber + client_request;
    CryptoPP::RSA::PublicKey pubKey = publicKeysID[conn->get_peer_id()];
    bool verify_peer = keyManager.VerifySignature(combinedData, msg.peerSign, pubKey);
    CryptoPP::RSA::PublicKey groupPublicKey = RSAKeyGenerator::getGroupPublicKey();
    bool verify_group = keyManager.VerifySignature(combinedData, msg.groupSign, groupPublicKey);
    if(verify_group == true && verify_peer == true) {
        //cout << "verified \n";
        RSAKeyGenerator::incrementPreparedMessages();
        // cout << RSAKeyGenerator::getPreparedMessages() << "\n";
    } else if (verify_group == true && verify_peer == false) {
        cout << conn->get_peer_id().to_hex() <<" might be faulty \n";
    } else if (verify_group == false && verify_peer == true) {
        cout << "Primary group node might be faulty \n";
    } else {
        cout << conn->get_peer_id().to_hex() << " and Primary group node both might be faulty \n"; 
    }
    int prepared_message = RSAKeyGenerator::getPreparedMessages();
    int fault_tolerance = 2 * ((((D2*nodes) - 1) / 3)+((1-D2) * nodes));
    std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
    std::vector<salticidae::PeerId> primary = TrustManager::primaryGroup;
    int consensus_size = consensus.size();
    int primary_size = primary.size();
    int max_prepared = (consensus_size - primary_size) * (consensus_size -1);
    if (prepared_message >= fault_tolerance) {
        cout<< "Total prepared messages: " << prepared_message << "messages \n";
        //Msg Reply will come here
        RSAKeyGenerator::clearPreparedMessages();
        //std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
        for( const auto &pid :consensus) {
            try
            {
                if(pid != peerId) {
                     peerNet->send_msg(MsgSend(true, msg.orderNumber), pid);
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            cout<<"sent \n";
        }
        peerNet->send_msg(MsgReply(true, msg.orderNumber), client_id);
        // Create PeerId for local and remote peers.
        salticidae::PeerId localPeer = peerId;
        salticidae::PeerId remotePeer = conn->get_peer_id();
        // Update trust information using TrustManager.
        TrustManager::updateTrust(localPeer, remotePeer, (verify_peer)); //add verify here
    } else if (prepared_message == max_prepared){
        RSAKeyGenerator::clearPreparedMessages();
        requestQueue.pop_front();
        requestQueue.size();
        client_request = "";
        busy = false;
        bool check = requestQueue.empty();
        peerNet->multicast_msg(MsgNextRequest(true), peers);
        cout << "Check queue 2: " << check << ".Size is:  " <<requestQueue.size()<< "\n";
        if(!check && !busy) {
            std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
            salticidae::PeerId client_conn_id = nextRequest.first;
            std::string client_conn_request = nextRequest.second;
        // MsgGroup receivedMsg = client_conn_request;
            Node::process_group_request(client_conn_request, client_conn_id);
            //queueRequest::ifBusy = true;
            busy=true;
        }
    }
    

}

//send reply handler
void Node::send_reply_handler(MsgSend &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got send reply message from " << conn->get_peer_id().to_hex()<<"\n";
    peerNet->send_msg(MsgReply(msg.sendReply, msg.orderNumber), client_id); 
    RSAKeyGenerator::clearPreparedMessages();
}

void Node::ack_handler(MsgAck &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got ACK:\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
         // Update Direct Trust Value
        for (size_t i = 0; i < NUM_NODES; i++) {
            salticidae::PeerId localPeer = allNodes[i];
            TrustManager::updateDirectTrust(localPeer);
        }
        //Update Global Trust Value
        for (size_t i = 0; i < NUM_NODES; i++) {
            salticidae::PeerId node = allNodes[i];
            TrustManager::updateGlobalTrust(node);
        }

        TrustManager::printGlobalTrustMap() ;
        TrustManager::getConsensusGroup(D2) ;
        TrustManager::printConsensusGroup() ;
        TrustManager::getPrimaryGroup(M2) ;
        TrustManager::printPrimaryGroup() ;
        RSAKeyGenerator::changeGroupKey();
        RSAKeyGenerator::clearPreparedMessages();
        RSAKeyGenerator::randomValueGenerator();
    requestQueue.pop_front();
    client_request = "";
    busy = false;
    bool check = requestQueue.empty();
    peerNet->multicast_msg(MsgNextRequest(true), peers);
    if(!check && !busy) {
        std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
        salticidae::PeerId client_conn_id = nextRequest.first;
        std::string client_conn_request = nextRequest.second;
       // MsgGroup receivedMsg = client_conn_request;
        Node::process_group_request(client_conn_request, client_conn_id);
        //queueRequest::ifBusy = true;
        busy=true;
    }
    num_acks += 1;
}

void Node::process_next_request(const MsgNextRequest &&msg, const Net::conn_t &conn) {
    requestQueue.pop_front();
    busy = false;
    client_request = " ";
    bool check = requestQueue.empty();
    if(check == false && busy == false) {
        std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
        salticidae::PeerId client_conn_id = nextRequest.first;
        std::string client_conn_request = nextRequest.second;
        Node::process_group_request(client_conn_request, client_conn_id);
        busy=true;
    }
}
void next_request_ack(const MsgNextAck &&msg, const Net::conn_t &conn) {

}
//---------------------View change -----------------

void Node::request_viewchange_handler(MsgRequestViewChange &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got view_change request message from " << conn->get_peer_id().to_hex()<<"\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
   
    std::string message = msg.message;
    total_view_change_requests++;
    int fault_tolerance = (2*(NUM_NODES)+1) / 3;
    if (total_view_change_requests >= fault_tolerance) {
        //std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
        cout << "View change triggered  \n";
        pauseTimer();
         TrustManager::getPrimaryGroup(M2);

         peerNet->multicast_msg(MsgGroup(message), peers);

    } else {
        cout << "Asking others for view change \n";
    }
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
