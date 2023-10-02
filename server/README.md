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

#### Example Usage
```cpp

 salticidae::EventContext ec;
Net::Config config;
salticidae::NetAddr paddr("127.0.0.1:5000");
salticidae::PeerId pid;
Node node(ec, config, paddr, pid);
```

#### Registering Handlers

- Call `reg_handlers()` after the Node object is created to ensure that the node can properly handle incoming messages and events.


#### Handling Different Types of Messages

- These methods are registered as handlers and are called automatically upon receiving the corresponding message type. You do not need to call them manually.

- `string_handler()`: Handles string messages and sends an acknowledgment back.
- `vote_handler()`: Handles vote messages and performs broadcasting to peers if necessary.
- `operation_handler()`: Handles operation messages and performs a multicast of a vote message to peers.
- `ack_handler()`: Handles acknowledgment messages.

#### Managing Peers

- `set_peers()`: Sets the list of peer IDs that the node is aware of.
```cpp
void set_peers(std::vector<salticidae::PeerId> peers);
std::vector<salticidae::PeerId> peerList = { /*... list of peers ...*/ };
node.set_peers(peerList);
```

### Handler

#### Handler Class Documentation

The `Handler` class is responsible for managing and interconnecting a network of nodes. It sets up each node's network configuration, registers their handlers, and connects them with each other to form a peer-to-peer network.



#### handler.h (Header File)

#### Includes and Namespaces

- Necessary header files and namespaces for using Salticidae's network functionalities and other standard features are included.

#### `Handler` Class Declaration

- A `Handler` object is constructed with an `EventContext` and `Config` objects. 
- Contains a private member `nodes`, a vector of `Node` objects.
- A public member function `get_nodes()` returns a pointer to the `nodes` vector.
- The constructor is declared which will handle the initialization and interconnection of `Node` objects.


#### Constructor

- Resizes the `nodes` vector to `NUM_NODES`, a predefined constant representing the number of nodes.
- Iterates over this range to create `Node` objects, initializing them with necessary parameters and registering their handlers.
- After creating the nodes, it iterates over them to interconnect them, i.e., make each node aware of every other node in the network by setting peer addresses and connecting to peers.
- Finally, it updates each node with the list of peers (excluding itself).

#### Functionality

- **Node Creation**: Each node is created with its unique address and peer ID.
- **Handler Registration**: For each node, handlers are registered to manage different types of incoming messages.
- **Node Interconnection**: Each node is connected to every other node in the network.
- **Setting Peers**: Each node is updated with the list of its peers.

#### How to Use


- Include the `handler.h` file in your main file to use the `Handler` class.

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

#### Message Types

Outlined below are the message types defined in `messages.h`:

- `MsgInvalid`: Denotes an invalid message.
  - **Description:** This message is used to describe that an invalid message was received.
  - **Opcode:** `0x0`
- `MsgAck`: Utilized for sending acknowledgment messages.
  - **Description:** This message is used to confirm to another peer that the last message was received.
  - **Opcode:** `0x1`
- `MsgString`: Sends a basic string message to another peer.
  - **Description:** Send a simple string message to another peer.
  - **Opcode:** `0x2`
- `MsgVote`: Transmits a vote message containing an ID and accompanying message.
  - **Description:** Message to send a test voting.
  - **Opcode:** `0x3`
- `MsgOp`: Sends an operation message consisting of a sequence and operation string.
  - **Description:** Message to send a test operation.
  - **Opcode:** `0x4`

Each message structure is equipped with serialization and deserialization methods for effective data conversion and exchange.

#### Usage

1. **Include the Header File**: To use any message type, include the `messages.h` file in your source file.

    ```cpp
    #include "messages.h"
    ```

2. **Create an Instance of Message Class**: Instantiate the desired message class by providing the necessary arguments to the constructor, if any.

    ```cpp
    MsgString msg("Hello World");
    ```

    This will create a `MsgString` object with the message "Hello World".

3. **Serialization**: The message object automatically gets serialized in the constructor. The serialized message can be accessed through the `serialized` member of the message object.

    ```cpp
    DataStream serializedMsg = msg.serialized;
    ```

4. **De-Serialization**: To de-serialize a message, create a `DataStream` object with the serialized message and pass it to the constructor of the message class.

    ```cpp
    MsgString deserializedMsg(std::move(serializedMsg));
    ```

#### Example

Below is an example showcasing how to use the `MsgString` class to send and receive a string message.

```cpp
#include "messages.h"

int main() {
    // Create a MsgString object with a message.
    MsgString msg("Hello World");

    // Access the serialized message.
    DataStream serializedMsg = msg.serialized;

    // Send serializedMsg over the network...

    // On receiving end, de-serialize the message.
    MsgString receivedMsg(std::move(serializedMsg));

    // Access the de-serialized message.
    std::string message = receivedMsg.message;
}

