/** @file messages.cpp
 * 
 * This file contains the implementation for serialization and de-serialization for each message type.
 * 
 * @brief Implementation of the message types.
*/

#include "messages.h"

/*
    Invalid Message
    This message is used to describe that we received an invalid message.
*/
const opcode_t MsgInvalid::opcode;
MsgInvalid::MsgInvalid() {}
MsgInvalid::MsgInvalid(DataStream &&s) {}


/*
    Acknowledgement message
    This message is used to confirm to another peer that we got the last message.
*/
const opcode_t MsgAck::opcode;
MsgAck::MsgAck() {}
MsgAck::MsgAck(DataStream &&s) {}


/*
    Message to send a simple string
    Send a simple string message to another peer.
*/
const opcode_t MsgString::opcode;
MsgString::MsgString(const std::string &message) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
}
MsgString::MsgString(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
}

/*
    Message to send a test voting
*/
const opcode_t MsgVote::opcode;
MsgVote::MsgVote(const uint &id, const std::string &message) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << id;
}
MsgVote::MsgVote(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    s >> id;
}

/*
    Message to send a test operation
*/
const opcode_t MsgOp::opcode;
MsgOp::MsgOp(const uint &sequence, const std::string &operation) {
    serialized << htole((uint32_t)operation.length());
    serialized << operation;
    serialized << sequence;
}
MsgOp::MsgOp(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    operation = std::string((const char *)s.get_data_inplace(len), len);
    s >> sequence;
}
