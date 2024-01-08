#ifndef FRAMEWORK_NODE_H
#define FRAMEWORK_NODE_H

#include <string>
#include <iostream>
#include <list>
#include <salticidae/network.h>
#include <functional>
#include <crypto++/rsa.h>
#include <openssl/evp.h> 
#include "config.h"
#include "messages.h"
#include "rsakey.h"
#include "TrustManager.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable> 
#include <unistd.h>


using std::cout;
using salticidae::_1;
using salticidae::_2;
using std::placeholders::_3;
using salticidae::ConnPool;
using Net = salticidae::PeerNetwork<opcode_t>;
using AddressNetworkVector = std::vector<std::pair<salticidae::NetAddr, std::unique_ptr<Net>>>;

/** @file node.h
 * 
 * @brief Header file for the Node class.
*/

/**
 * Node class represents a peer/replica in the network.
 * Handlers are defined as member functions and are called when a given message type gets received.
 * Since for each peer/replica we have a different Node instance, replica states can be
 * implemented by manipulating member variables on incoming messages.
 * 
 * @brief Class representing a replication node in the network.
 * 
 * @param ec The execution context for this node's network.
 * @param config The configuration for this node's network.
 * @param paddr The networking address for this node.
 * @param pid The peer id for this node, also used as an ID for this node.
 * @param privateKey RSA private key for the node
 * @param publicKey RSA public key for the node
*/
class Node {
    public:
        //! Networking for this node.
        std::unique_ptr<Net> peerNet;
        //! Network address of this node.
        salticidae::NetAddr peerAddr;
        //! PeerId for this node.
        salticidae::PeerId peerId;
        CryptoPP::RSA::PrivateKey privateKey;
        CryptoPP::RSA::PublicKey publicKey;
       // QueueRequest queueRequest;


        
        //To save requests in a queue 
        //std::queue<MsgGroup> requestQueue;
        // Declaration of the setter function
        //void setRequestQueue(const std::queue<MsgGroup>& newQueue);
        // // Mutex to protect access to the queue
        // std::mutex queueMutex;
        // // Condition variable to signal when a new item is added to the queue
        // std::condition_variable queueCondition; 
        // //Condition variable for the reply 
        // std::condition_variable replyCondition;
        
        // Thread to process items from the queue



        //! Constructor creating a new node.
        Node(const salticidae::EventContext &ec, const Net::Config config, const salticidae::NetAddr paddr, const salticidae::PeerId pid, const CryptoPP::RSA::PrivateKey prKey, const CryptoPP::RSA::PublicKey puKey);
        Node();

         // Function to start the timer
        void startTimer();

        // Function to pause the timer
        void pauseTimer();

        // Function to reset and reinitialize the timer
        void resetTimer();

        double getElapsedTimeInSeconds() const;

        /**
         * @brief Set the vector of known peers for this node.
         *
         * @param peers The vector of peers being the neighours of this peer.
        */
        void set_peers(std::vector<salticidae::PeerId> peers);

        //set vector for publickeys of peers
        void set_publickeys(std::unordered_map<salticidae::PeerId, CryptoPP::RSA::PublicKey> publicKeysID);
        //set primary group
        void set_primarygroup(std::vector<salticidae::PeerId> primary_group);
        //set consensus group
        void set_consensusgroup(std::vector<salticidae::PeerId> consensus_group);
        //set all nodes
        void set_allNodes(std::vector<salticidae::PeerId> allNodes);
        //set group private key
        // void set_group_privateKey(CryptoPP::RSA::PrivateKey groupPrivateKey);
        // //set group public key
        // void set_group_publicKey(CryptoPP::RSA::PublicKey groupPublicKey);

        /**
         * Member function to register all the message callback handlers.
         * Needs to be called after the construction of the different instances.
         * 
         * @brief Register message handlers.
        */
        void reg_handlers();


        /**
         * @brief Return the trimmed PeerId (in HEX) as an ID for this node.
        */
        std::string get_id() {
            return peerId.to_hex().substr(0, 10);
        }

        /**
         * @brief If "full=true", then return the complete PeerId (in HEX).
         * 
         * @param full If true, will return the whole PeerId.
        */
        std::string get_id(bool full) {
            if (full) {
                return peerId.to_hex();
            } else {
                return peerId.to_hex().substr(0, 10);
            }
        }

    private:
        /**
         * Set a vector of PeerId, which is used to describe other peers that are known to this one.
         * Peers in this vector describe the nodes we should broadcast messages to.
        */
       // static RSAKeyGenerator keyManager;

        std::vector<salticidae::PeerId> peers;
        std::vector<salticidae::PeerId> allNodes;
        std::vector<salticidae::PeerId> primary_group;
        std::vector<salticidae::PeerId> consensus_group;
        // CryptoPP::RSA::PrivateKey groupPrivateKey;
        // CryptoPP::RSA::PublicKey groupPublicKey;
        bool busy = false;
        
        

        std::unordered_map<salticidae::PeerId, CryptoPP::RSA::PublicKey> publicKeysID;
        uint8_t num_acks = 0;
        static int total_view_change_requests;

        std::unordered_map<salticidae::PeerId, bool> pidmap;
        int nodes = NUM_NODES; 

        // Member variables to store state accross callbacks.
        salticidae::PeerId client_id;
        std::string client_request;
        int a = 0;
        int b = 0;
        std::list<MsgPrepare> prepared_message;
        std::list<MsgCommit> commit_message;
        std::list<bool> verified_message;
        unsigned long prepared_messages = 0;
        std::list<std::pair<salticidae::PeerId, std::string>> requestQueue;
        
        /**
         * A handler is called when its specific message type gets received.
         * Operations resulting from receiving that message can then be done in the body of the member function.
         * 
         * @defgroup MessageHandlerGroup Message specific handlers
         * @brief Handlers for the different message types.
         * @param msg The message on which this member function was called.
         * @param conn The connection that sent this message.
         * 
         * An explained example implementation follows:
         * 
         * Code in the node.h file:
         * @code{.h}
         * // We define the example_handler that gets called when a MsgExample message gets received.
         * void example_handler(MsgExample &&msg, const Net::conn_t &conn);
         * @endcode
         * 
         * Code in the node.cpp file:
         * @code{.cpp}
         * void Node::example_handler(MsgExample &&msg, const Net::conn_t &conn) {
         *      // Do stuff here
         *      // For example print the message...:
         *      cout << "We got " << msg.someMessage;
         *      // ... and the number:
         *      cout << " and the number " << msg.someNumber << "\n";
         * }
         * @endcode
         * 
         * Finally we also have to register the handler, this is done in the Node::reg_handlers() (node.cpp) member function with:
         * @code{.cpp}
         * void Node::reg_handlers() {
         *      ...
         *      peerNet->reg_handler(salticidae::generic_bind(&Node::example_handler, this, _1, _2));
         *      ...
         * }
         * @endcode
         * 
         * The example_handler will now be called when we receive a MsgExample message. Since we are working with classes and class instances, we could
         * also update the state of the replica by updating Node member variables, for example increase a counter each time we get a specific message.
         * @{
        */

        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a MsgString message gets received.
        */
        
        void prepare_handler(MsgPrepare &&msg, const Net::conn_t &conn);
        void preprepare_handler(MsgPreprepare &&msg, const Net::conn_t &conn);
        //void group_handler(MsgGroup &&msg, const Net::conn_t &conn);
        void primary_consensus_handler(MsgPrimaryConsensus &&msg, const Net::conn_t &conn);
        void primary_verified_handler(MsgPrimaryVerified &&msg, const Net::conn_t &conn);
        void send_reply_handler(MsgSend &&msg, const Net::conn_t &conn);
        void group_request_handler(MsgGroup &&msg, const Net::conn_t &conn);
        //void process_queue();
        void process_group_request(const std::string msg, const salticidae::PeerId conn); 
        void process_next_request(const MsgNextRequest &&msg, const Net::conn_t &conn);
         //-----------View change--------------
        void request_viewchange_handler(MsgRequestViewChange &&msg, const Net::conn_t &conn);


        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a MsgVote message gets received.
        */
        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a MsgOp message gets received.
        */
        void operation_handler(MsgOp &&msg, const Net::conn_t &conn);
        /**
         * Simple handler printing that an ACK got received, which node sent it and which
         * node received the ACK message.
         * 
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a MsgAck message gets received.
        */
        void ack_handler(MsgAck &&msg, const Net::conn_t &conn);
        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a request message gets received.
        */


       /*
            Changes here
       */
        void request_handler(MsgRequest &&msg, const Net::conn_t &conn);
        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a forward message gets received.
        */
        void forward_handler(MsgForward &&msg, const Net::conn_t &conn);
        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a request message gets received.
        */
        void result_handler(MsgResult &&msg, const Net::conn_t &conn);
        /**
         * @ingroup MessageHandlerGroup
         * 
         * @brief Handler for when a request message gets received.
        */
        void reply_handler(MsgReply &&msg, const Net::conn_t &conn);
        /** @} */


        /**
         * %Handler called when an unknown peer tries to connect. The peer is unknown in that the underlying
         * salticidae framework does not know about the peer and Node.peers does not relate to this.
         * Since salticidae does not allow unknown peers to connect, we simply add it to the internally known
         * peers so that it can connect without being previously known.
         * 
         * @param addr The NetAddr that tries to connect.
         * @param cert The SSL certificate of the unknown peer trying to connect.
        */
        void unknown_peer_handler(const salticidae::NetAddr &addr, const salticidae::X509 *cert);

        /**
         * @brief Useful for debugging purposes.
         * 
         * @param exception_ptr Pointer to where the exception happened.
         * @param idk
         * @param huh
        */
        void error_handler(std::__exception_ptr::exception_ptr exception_ptr, bool idk, int huh);
        /**
         * @brief Useful for debugging purposes.
         * 
         * @param conn The connection of the peer trying to connect.
         * @param connected Whether the peer connected or disconnected.
         * true = connected
         * false = disconnected
        */
        void peer_handler(const Net::conn_t &conn, bool connected);

        /**
         * @brief %Handler for when a connection is made, not needed but useful for debugging.
         * 
         * @param conn The connection trying to connect.
         * @param connected Whether the connection connected or disconnected
         * @returns true
         * @see peer_handler
        */
        bool conn_handler(const ConnPool::conn_t &conn, bool connected);
};

#endif
