#ifndef FRAMEWORK_MESSAGES_H
#define FRAMEWORK_MESSAGES_H

#include <string>
#include<iostream>
#include <salticidae/msg.h>
#include <crypto++/rsa.h>
#include "config.h"


using salticidae::DataStream;
using salticidae::htole;
using salticidae::letoh;

/** @file messages.h
 * 
 * @brief Header file for the message types.
*/


/**
 * Each message has an opcode component that has to be unique, < uint8_t and be a message member.
 * Two overloads for construction:
 * - Serialize object:
 *      Constructor to go from the message and its argument(s) to the serialized version.
 *      I.e. how a string is encapsulated with its length etc.
 * - De-Serialize object:
 *      Constructor to go from the serialized message (from the network) to go to the object.
 * 
 * Both overloads have to be implemented for each message type.
 * 
 * @note The messages that are implemented here are used as a reference to show how it could be done
 * and do not have to be used for the implementation of a BFT protocol.
 * 
 * Example code to be put in the messages.h file.
 * @code{.h}
 * struct MsgExample {
 *      // Unique opcode for the message
 *      static const opcode_t opcode = 0x0;
 *      // Serialized object that will be populated when sending and receiving a message.
 *      DataStream serialized;
 * 
 *      // Constructor for message from object to serialized object i.e. sending a message.
 *      MsgExample(const std::string &someMessage, const int &someNumber);
 *      // Constructor for receiving a message i.e. de-serializing the message to the object and its components.
 *      MsgExample(DataStream &&s);
 * };
 * @endcode
 * 
 * The messages.cpp file implements the actual serialization procedure:
 * @code{.cpp}
 * const opcode_t MsgExample::opcode;
 * MsgExample::MsgExample(const std::string &someMessage, const int &someNumber) {
 *      // This can be used to serialize a string, the message length is best included first.
 *      // Convert byte order from host endianess to little-endian see this [link](https://man7.org/linux/man-pages/man3/endian.3.html).
 *      serialized << htole((uint32_t)someMessage.length());
 *      serialized << someMessage;
 *      // We can simply append the someNumber since we know how large integers are from the C++ reference.
 *      serialized << someNumber;
 * }
 * MsgString::MsgString(DataStream &&s) {
 *      // We have to extract the length of the message first...
 *      uint32_t len;
 *      s >> len;
 *      // Convert byte order from little-endian to host endianess see this [link](https://man7.org/linux/man-pages/man3/endian.3.html).
 *      len = letoh(len);
 *      // .. before we can extract the message itself.
 *      someMessage = std::string((const char *)s.get_data_inplace(len), len);
 *      // Since the integer size is known, we can simply extract our number.
 *      s >> someNumber;
 * }
 * @endcode
*/


/**
 * @defgroup MessageTypes Message types
 * @brief Different message types that are exchanged.
 * @{
*/

/**
 * @ingroup MessageTypes
 * 
 * @brief Invalid Message.
*/
struct MsgInvalid {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x0;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgInvalid();
    //! Serialize the object from a data stream to C++ datatypes.
    MsgInvalid(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Acknowledgement message.
*/
struct MsgAck {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x1;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgAck();
    //! Serialize the object from a data stream to C++ datatypes.
    MsgAck(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief String message.
*/
struct MsgString {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x2;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    std::string message;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgString(const std::string &message);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgString(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Vote message.
*/
struct MsgVote {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x3;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    std::string message;
    //! Member variable storing the ID of the vote.
    uint id;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgVote(const uint &id, const std::string &message);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgVote(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Operation message.
*/
struct MsgOp {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x4;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    std::string operation;
    uint sequence;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgOp(const uint &sequence, const std::string &operation);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgOp(DataStream &&s);
};


/*
    Newly added
*/

/**
 * @ingroup MessageTypes
 * 
 * @brief Request message.
*/
struct MsgRequest {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x5;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Store our two numbers
    int a;
    int b;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgRequest(const int &a, const int &b);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgRequest(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Forward message.
*/
struct MsgForward {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x6;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Store our two numbers
    int a;
    int b;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgForward(const int &a, const int &b);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgForward(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Result message.
*/
struct MsgResult {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x7;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Result number
    int c;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgResult(const int &c);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgResult(DataStream &&s);
};

/**
 * @ingroup MessageTypes
 * 
 * @brief Reply message.
*/
struct MsgReply {
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x8;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;

    //! Is the second server faulty?
    bool result;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgReply(const bool &result);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgReply(DataStream &&s);
};
/** @} */
//Msg Prepare
struct MsgPrepare {
    static const opcode_t opcode = 0x9;
    DataStream serialized;
    //Order number
    std::string orderNumber;
    //message
    std::string message;
    //signatures
    std::string groupSign;
    std::string peerSign;

    MsgPrepare(const std::string &message, const std::string &orderNumber, const std::string &groupSign, const std::string &peerSign);
    MsgPrepare(DataStream &&s);

};
/** @} */
//Msg Precommit
struct MsgCommit {
    static const opcode_t opcode = 0x10;
    DataStream serialized;
    //Order number
    uint order;
    //message
    std::string message;
    //hash
    int msghash;

    MsgCommit(const uint &order, const std::string &message, const int &msghash);
    MsgCommit(DataStream &&s);

};
/** @} */
//Msg Prepare
struct MsgPreprepare{
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x11;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    std::string message;
    std::string signature;
    std::string orderNumber;

    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgPreprepare(const std::string &message, const std::string signature, const std::string &orderNumber);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgPreprepare(DataStream &&s);
};
/** @} */
//MsgGroup
struct MsgGroup{
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x12;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    std::string message;
    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgGroup(const std::string &message);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgGroup(DataStream &&s);
};
//Primary Group verification
struct MsgPrimaryConsensus{
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x13;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    std::string message;
    std::string signature;
    std::string orderNumber;
    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgPrimaryConsensus(const std::string &message, const std::string signature, const std::string &orderNumber);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgPrimaryConsensus(DataStream &&s);
};
//primary group verified
struct MsgPrimaryVerified{
    //! Set unique opcode for this message.
    static const opcode_t opcode = 0x14;
    //! Member variable storing the serialized version of the message.
    DataStream serialized;
    //! Member variable storing the actual message as a string.
    bool verified;
    //! Serialize the object from C++ datatypes to a data stream we can send over the network.
    MsgPrimaryVerified(const bool &verified);
    //! Serialize the object from a data stream to C++ datatypes.
    MsgPrimaryVerified(DataStream &&s);
};
//Msg to Ask Every consensus node to send reply;
struct MsgSend {
    static const opcode_t opcode = 0x15;
    DataStream serialized;
    bool sendReply;

    MsgSend(const bool &sendReply);
    MsgSend(DataStream &&s);
};
#endif
