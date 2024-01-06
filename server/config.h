#ifndef FRAMEWORK_CONFIG_H
#define FRAMEWORK_CONFIG_H

#include <cstdint>
#include <string>

/** @file config.h
 * 
 * @brief Common configuration file.
*/

/**
 * Define the IP address to use for the nodes. In our case we want to use localhost.
*/
const std::string ADDRESS_STRING = "127.0.0.1";

// #define DEBUG

/**
 * @brief Define the starting port that is used for the nodes.
*/
#define ADDRESS_PORT 8123

/**
 * @brief Define the number of nodes/replicas to use. 
*/
#define NUM_NODES 10

/**
 * @brief Define the percentage of consensus groupe.
*/
#define D2 0.6

/**
 * @brief Define the percentage of primary groupe.
*/
#define M2 0.5


/**
 * @brief Define the port that is used for the client connecting to the replicas. 
*/
#define CLIENT_PORT 9000

#define MAX_MESSAGE_SIZE 65536

/**
 * Define the opcode size for the maximum number of unique messages. In our case 8-bit unsigned integer 0-255.
*/
using opcode_t = uint8_t;

#endif
