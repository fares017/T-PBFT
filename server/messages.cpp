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


/*
    Newly added
*/

/*
    Message for request
*/
const opcode_t MsgRequest::opcode;
MsgRequest::MsgRequest(const int &a, const int &b) {
    serialized << a;
    serialized << b;
}
MsgRequest::MsgRequest(DataStream &&s) {
    s >> a;
    s >> b;
}

/*
    Message for forward
*/
const opcode_t MsgForward::opcode;
MsgForward::MsgForward(const int &a, const int &b) {
    serialized << a;
    serialized << b;
}
MsgForward::MsgForward(DataStream &&s) {
    s >> a;
    s >> b;
}

/*
    Message for result
*/
const opcode_t MsgResult::opcode;
MsgResult::MsgResult(const int &c) {
    serialized << c;
}
MsgResult::MsgResult(DataStream &&s) {
    s >> c;
}

/*
    Message for reply
*/
const opcode_t MsgReply::opcode;
MsgReply::MsgReply(const int &c, const bool &fault) {
    serialized << fault;
    serialized << c;
}
MsgReply::MsgReply(DataStream &&s) {
    s >> fault;
    s >> c;
}
//Prepare message
const opcode_t MsgPrepare::opcode;
MsgPrepare::MsgPrepare(const uint &order, const std::string &message, const int &msghash){
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << order;
    serialized << msghash;
}
MsgPrepare::MsgPrepare(DataStream &&s){
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    s >> order;
    s >> msghash;
}
//Precommit message
const opcode_t MsgPrecommit::opcode;
MsgPrecommit::MsgPrecommit(const uint &order, const std::string &message, const int &msghash){
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << order;
    serialized << msghash;
}
MsgPrecommit::MsgPrecommit(DataStream &&s){
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    s >> order;
    s >> msghash;
}
//Preprepare message
const opcode_t MsgPreprepare::opcode;
MsgPreprepare::MsgPreprepare(const std::string &message) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
}
MsgPreprepare::MsgPreprepare(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
}