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
    peerNet->reg_handler(salticidae::generic_bind(&Node::string_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::vote_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::ack_handler, this, _1, _2));

    peerNet->reg_handler(salticidae::generic_bind(&Node::operation_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::prepare_handler, this, _1, _2));
   // peerNet->reg_handler(salticidae::generic_bind(&Node::commit_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::preprepare_handler, this, _1, _2));
    //peerNet->reg_handler(salticidae::generic_bind(&Node::group_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::group_request_handler, this, _1, _2)); 
    peerNet->reg_handler(salticidae::generic_bind(&Node::primary_consensus_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::primary_verified_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::send_reply_handler, this, _1, _2));
    peerNet->reg_handler(salticidae::generic_bind(&Node::process_next_request, this, _1, _2));
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
    RSAKeyGenerator keyGenerator;
    // EVP_PKEY* pKey = keyGenerator.ConvertPrivatetoEVP(privateKey);
    // MessageSigner signer;
    //signer.printRSAPrivateKey(pKey);
    std::string orderNumber = "1";
    std::string message = msg.message;
    //std::string signature = signer.signMessage(orderNumber, message, pKey);
    std::string combinedData = orderNumber + message;
    std::string signature = keyGenerator.SignMessage(combinedData, privateKey);
    //EVP_PKEY* pubKey = keyGenerator.ConvertPublictoEVP(publicKey);
    //signer.printRSAPublicKey(pubKey);
    //std::string combinedData = orderNumber + message;
    //bool verify = signer.verifySignature(combinedData, signature, pubKey);
    bool verify = keyGenerator.VerifySignature(combinedData, signature, publicKey);
    if(verify == true) {
        cout << "Message Verified \n"; 
    } else {
        cout << "Verification Failed, Node might be faulty \n";
    }
    cout << "Node " << get_id() << " got message:\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
    cout << "\tsignature: " << signature <<"\n";
    cout << "\tPublic Key 2: " << publicKeysID[peerId].GetValueNames() <<"\n";
   // peerNet->multicast_msg(MsgPrepare(order, msg.message, hashvalue), peers);
    peerNet->send_msg(MsgAck(), conn);
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

//Mutex to protect access to the queue
//std::mutex queueMutex;
// Condition variable to signal when a new item is added to the queue
//std::condition_variable queueCondition; 
//Condition variable for the reply 
//std::condition_variable replyCondition;


 //To save requests in a queue 
// std::queue<MsgGroup> requestQueue;


void Node::group_request_handler(MsgGroup &&msg, const Net::conn_t &conn) { 

    // Lock the queue to safely access it
    //std::unique_lock<std::mutex> lock(queueMutex); 

    // Add the received MsgGroup object to the queue
    //requestQueue.push(std::move(msg)); 

    //queueCondition.notify_one();  // Notify that a new request is available

    // Wait for a new request to be available
    //queueCondition.wait(lock, [this] { return !requestQueue.empty(); }); 

    // Retrieve the first item from the queue
    //MsgGroup receivedMsg = std::move(requestQueue.front());
    //requestQueue.pop();  // Remove the item from the queue

    //lock.unlock();  // Release the lock before processing 

    // Process the MsgGroup object
    //cout << "Processing MsgGroup! \n"; 
    //MsgGroup receivedMsg = msg; 
    //Node::process_group_request(receivedMsg, conn);  


    // Wait until the reply is sent
    //replyCondition.wait(lock, [this] { return replySent; });

    // Reset the flag for the next iteration
    //replySent = false;

    // Lock the queue again for the next iteration
    //lock.lock(); 
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
        //MsgGroup receivedMsg = client_conn_request;
        Node::process_group_request(client_conn_request, client_conn_id);
        //queueRequest::ifBusy = true;
        busy=true;
    }
     

    //replySent = queueRequest::ifBusy;
    //usleep(5000000);
    ///std::this_thread::sleep_for(std::chrono::seconds(5));
    
} 
// void Node::process_queue() {
//     if (!busy)
//     {
        
//     }
// }

//
void Node::process_group_request(const std::string msg, const salticidae::PeerId conn) { 
    // usleep(3000000); // in microseconds
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

// void Node::group_handler(MsgGroup &&msg, const Net::conn_t &conn) {
//     cout <<"Node " << get_id() << "got client request message from " << conn->get_peer_id().to_hex() << "\n";
//     RSAKeyGenerator keyManager;
//     std::string signature;
//     client_request = msg.message;
//     client_id = conn->get_peer_id();
//     //cout << primary_group[0].to_hex() << "\n";
//     //cout << peerId.to_hex() << "\n";
//     std::vector<salticidae::PeerId> primary = TrustManager::primaryGroup;
//     int val = RSAKeyGenerator::getRandomValue();
//     if (peerId == primary[val])
//     {
//         cout << "\tMessage: " << msg.message << "\n";
//         RSAKeyGenerator::incrementOrderNumber(); 
//         int currentOrderNumber = RSAKeyGenerator::getOrderNumber();
//         std::string orderNumber = std::to_string(currentOrderNumber);
//         std::string message = msg.message;
//         std::string combinedData = orderNumber + message;
//         signature = keyManager.SignMessage(combinedData, privateKey);
//         for (const auto &pid: primary) {
//             if (pid != peerId) {
//                 peerNet->send_msg(MsgPrimaryConsensus(message, signature, orderNumber), pid);
//             }
//         }
//         //peerNet->multicast_msg(MsgPrimaryConsensus(message, signature, orderNumber), primary_group);
//     }
//     cout << "\tMessage: " << msg.message << "\n";
//     peerNet->send_msg(MsgAck(), conn);
// }

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
        
        //peerNet->multicast_msg(MsgPrepare(message, orderNumber, msg.signature, peer_signature), consensus);

    } else {
        cout << "Verification failed \n";
    }
    //peerNet->multicast_msg(MsgPrepare(order, msg.message, hashvalue), peers);
    //peerNet->send_msg(MsgAck(), conn);
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
    if (prepared_message >= fault_tolerance) {
        cout<< "Total prepared messages: " << prepared_message << "messages \n";
        //Msg Reply will come here
        RSAKeyGenerator::clearPreparedMessages();
        std::vector<salticidae::PeerId> consensus = TrustManager::consensusGroup;
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
        
        //std::unique_lock<std::mutex> lock(queueMutex);

        // Set the flag indicating that the reply is sent
        //busy = true; 

        // Notify the waiting thread that the reply is sent
        //replyCondition.notify_one(); 
        //peerNet->multicast_msg(MsgSend(true), consensus);
        
    } 
    // Create PeerId for local and remote peers.
    salticidae::PeerId localPeer = peerId;
    salticidae::PeerId remotePeer = conn->get_peer_id();

    // Update trust information using TrustManager.
    TrustManager::updateTrust(localPeer, remotePeer, (verify_peer)); //add verify here
    // Print the trustMap
    //cout << "TrustMap contents:\n";
    // Print the trustMap
    //TrustManager::printTrustMap();

}

//send reply handler
void Node::send_reply_handler(MsgSend &&msg, const Net::conn_t &conn) {
    cout << "Node " << get_id() << " got send reply message from " << conn->get_peer_id().to_hex()<<"\n";
    //cout << "client id " << client_id.to_hex(); 
    peerNet->send_msg(MsgReply(msg.sendReply, msg.orderNumber), client_id); 
    RSAKeyGenerator::clearPreparedMessages();
    //After sending the reply 
    // Lock the mutex
    //std::unique_lock<std::mutex> lock(queueMutex);
    // Set the flag indicating that the reply is sent
    //busy = true; 

    // Notify the waiting thread that the reply is sent
   // replyCondition.notify_one(); 
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
        //queueRequest::randomValueGenerator();
       // queueRequest::ifBusy = false;
    //queueRequest.removeRequest();
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
    num_acks += 1;
}

void Node::process_next_request(const MsgNextRequest &&msg, const Net::conn_t &conn) {
    requestQueue.pop_front();
    busy = false;
    client_request = " ";
    bool check = requestQueue.empty();
    check = requestQueue.empty();
    cout << "Check queue: " << check << ".Size is:  " <<requestQueue.size() << "\n";
    if(check == false && busy == false) {
        std::pair<salticidae::PeerId, std::string> nextRequest(requestQueue.front());
        salticidae::PeerId client_conn_id = nextRequest.first;
        std::string client_conn_request = nextRequest.second;
        //MsgGroup receivedMsg = client_conn_request;
        Node::process_group_request(client_conn_request, client_conn_id);
        //queueRequest::ifBusy = true;
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

//void Node::commit_handler(MsgCommit &&msg, const Net::conn_t &conn){
    // cout << "Node " << get_id() << " got prepare message from " << conn->get_peer_id().to_hex()<<"\n";
    // std::hash<std::string> commithash;
    // int hashval = commithash(msg.message);
    // unsigned long fault_tol = ((2*nodes) +1) / 3;
    // if(hashval == msg.msghash) {
        // commit_message.push_back(msg);
        // if(commit_message.size() >= fault_tol) {
 //           MsgReply or Msg accept request
        // }
    // }
//         prepared_message.remove(msg);
// }
// void Node::preprepare_handler(MsgPreprepare &&msg, const Net::conn_t &conn) {
//     cout << "Primary Node " << get_id() << " got message:\n";
//     cout << "\tMessage: " << msg.message << "\n";
//     cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
//     std::hash<std::string> msghash;
//     int hashvalue = msghash(msg.message);
//     uint order = 1;
//     peerNet->multicast_msg(MsgPrepare(order, msg.message, hashvalue), peers);
//     peerNet->send_msg(MsgAck(), conn);
// }
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
      //  peerNet->send_msg(MsgReply(c, false), client);
    } else {
        cout << "\tc DOES NOT match! Sending failure reply to " << client.to_hex().substr(0, 10) << "\n";
       // peerNet->send_msg(MsgReply(0, true), client);
    }
}


// We technically do not need this since only the client should receive reply messages.
// void Node::reply_handler(MsgReply &&msg, const Net::conn_t &conn) {
//     cout << "Node " << get_id() << " got reply message: \n";
//     cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
//     cout << "\tWith c = " << msg.c << "\n";
//     cout << "\tAnd  fault = " << msg.fault << "\n";
// }

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
