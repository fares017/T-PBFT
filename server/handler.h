#ifndef FRAMEWORK_HANDLER_H
#define FRAMEWORK_HANDLER_H

#include <iostream>

#include <salticidae/network.h>

#include "config.h"
#include "messages.h"
#include "node.h"

using std::cout;
using Net = salticidae::PeerNetwork<opcode_t>;
using NodeVector = std::vector<Node>;

/** @file handler.h
 * 
 * @brief Header file for the Handler class.
*/


/**
 * Handler class creating the interconnection between the different nodes
 * and creating the number of Node instances asked for.
 * 
 * @brief Class handling the creation and interconnect of the replicas.
 * 
 * @param ec The execution context for the networks of the nodes.
 * @param config The configuration for the networks of the nodes.
*/
class Handler {
    public:
        //! Constructor creating a handler.
        Handler(const salticidae::EventContext &ec, const Net::Config config);

        /**
         * @brief Return the nodes that the handler manages.
         * 
         * @returns nodes
        */
        NodeVector *get_nodes() {
            return &nodes;
        }

    private:
        /**
         * @brief Nodes that this handler manages.
        */
        NodeVector nodes;
};

#endif
