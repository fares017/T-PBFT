# P2P Message Serialization Framework

A framework for P2P (Peer-to-Peer) network communication, handling various message types and their serialization and deserialization.

## Table of Contents

- [Overview](#overview)
- [Components Description](#components-description)
  - [Node](#node)
  - [Handler](#handler)
  - [Messages](#messages)
- [Message Types](#message-types)
- [Building and Running](#building-and-running)
- [Contribution](#contribution)
- [License](#license)

## Overview

This framework provides the infrastructure for establishing peer connections, and for sending and receiving various types of messages. Utilizing the Salticidae library, it effectively manages network connections and data serialization and deserialization.

## Components Description

### Node

- Connection Management: A node can establish, maintain, and terminate connections with other peers in the network.
- Message Handling: Responsible for sending and receiving messages. It serializes the messages before sending and deserializes received messages.
- Network Participation: Actively participates in the network, adhering to the protocols defined for communication and cooperation among peers.

#### Constructor

The `Node` class constructor initializes a new `Node` object and sets up the network for the node.

```cpp
Node(const salticidae::EventContext &ec, const Net::Config config, const salticidae::NetAddr paddr, const salticidae::PeerId pid);
```

# Example Usage
```cpp

 salticidae::EventContext ec;
Net::Config config;
salticidae::NetAddr paddr("127.0.0.1:5000");
salticidae::PeerId pid;
Node node(ec, config, paddr, pid);
```

# Registering Handlers

- Call `reg_handlers()` after the Node object is created to ensure that the node can properly handle incoming messages and events.


# Handling Different Types of Messages

- These methods are registered as handlers and are called automatically upon receiving the corresponding message type. You do not need to call them manually.

- `string_handler()`: Handles string messages and sends an acknowledgment back.
- `vote_handler()`: Handles vote messages and performs broadcasting to peers if necessary.
- `operation_handler()`: Handles operation messages and performs a multicast of a vote message to peers.
- `ack_handler()`: Handles acknowledgment messages.

# Managing Peers

- `set_peers()`: Sets the list of peer IDs that the node is aware of.
```cpp
void set_peers(std::vector<salticidae::PeerId> peers);
std::vector<salticidae::PeerId> peerList = { /*... list of peers ...*/ };
node.set_peers(peerList);
```

### Handler

# Handler Class Documentation

The `Handler` class is responsible for managing and interconnecting a network of nodes. It sets up each node's network configuration, registers their handlers, and connects them with each other to form a peer-to-peer network.



## handler.h (Header File)

### Includes and Namespaces

- Necessary header files and namespaces for using Salticidae's network functionalities and other standard features are included.

### `Handler` Class Declaration

- A `Handler` object is constructed with an `EventContext` and `Config` objects. 
- Contains a private member `nodes`, a vector of `Node` objects.
- A public member function `get_nodes()` returns a pointer to the `nodes` vector.
- The constructor is declared which will handle the initialization and interconnection of `Node` objects.

## handler.cpp (Source File)

### Constructor

- Resizes the `nodes` vector to `NUM_NODES`, a predefined constant representing the number of nodes.
- Iterates over this range to create `Node` objects, initializing them with necessary parameters and registering their handlers.
- After creating the nodes, it iterates over them to interconnect them, i.e., make each node aware of every other node in the network by setting peer addresses and connecting to peers.
- Finally, it updates each node with the list of peers (excluding itself).

### Functionality

- **Node Creation**: Each node is created with its unique address and peer ID.
- **Handler Registration**: For each node, handlers are registered to manage different types of incoming messages.
- **Node Interconnection**: Each node is connected to every other node in the network.
- **Setting Peers**: Each node is updated with the list of its peers.

## How to Use

### Include handler.h in Your Main File

Include the `handler.h` file in your main file to use the `Handler` class.

```cpp
#include "handler.h"

int main() {
    // Initialize EventContext and Config objects.
    salticidae::EventContext ec;
    Net::Config config;

    // Create an instance of Handler.
    Handler handler(ec, config);

    // Access the nodes managed by handler.
    NodeVector* nodes = handler.get_nodes();

    // Perform operations on nodes...
}
```
- You can access the nodes managed by the Handler object using the get_nodes() method.
```cpp
NodeVector* nodes = handler.get_nodes();
```


### Messages

- Defined in `messages.cpp` and `messages.h`.
- Various message types with serialization and deserialization methods.
- Each message type is identifiable with a unique opcode.

## Message Types

Outlined below are the message types defined in `messages.h`:

- `MsgInvalid`: Denotes an invalid message.
- `MsgAck`: Utilized for sending acknowledgment messages.
- `MsgString`: Sends a basic string message to another peer.
- `MsgVote`: Transmits a vote message containing an ID and accompanying message.
- `MsgOp`: Sends an operation message consisting of a sequence and operation string.

Each message structure is equipped with serialization and deserialization methods for effective data conversion and exchange.

## Building and Running

Ensure the Salticidae library is installed. Execute the following commands for building and running the code:

```bash
# Clone the repository
git clone https://github.com/your-username/your-project-name.git

# Navigate to the directory
cd your-project-name

# Compile the code
g++ -std=c++11 -o main handler.cpp messages.cpp -lsalticidae

# Run the compiled code
./main
