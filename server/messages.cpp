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
MsgReply::MsgReply(const bool &result, const std::string &orderNumber) {
    serialized << htole((uint32_t)orderNumber.length());
    serialized << orderNumber;
    serialized << result;
}
MsgReply::MsgReply(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    orderNumber = std::string((const char *)s.get_data_inplace(len), len);
    s >> result;
}
//
//Prepare message
const opcode_t MsgPrepare::opcode;
MsgPrepare::MsgPrepare(const std::string &message, const std::string &orderNumber, const std::string &groupSign, const std::string &peerSign){
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << htole((uint32_t)orderNumber.length());
    serialized << orderNumber;
    serialized << htole((uint32_t)groupSign.length());
    serialized << groupSign;
    serialized << htole((uint32_t)peerSign.length());
    serialized << peerSign;
}
MsgPrepare::MsgPrepare(DataStream &&s){
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    uint32_t len2;
    s >> len2;
    len2 = letoh(len2);
    orderNumber = std::string((const char *)s.get_data_inplace(len2), len2);
    uint32_t len3;
    s >> len3;
    len3 = letoh(len3);
    groupSign = std::string((const char *)s.get_data_inplace(len3), len3);
    uint32_t len4;
    s >> len4;
    len4 = letoh(len4);
    peerSign = std::string((const char *)s.get_data_inplace(len4), len4);

}
//Precommit message
const opcode_t MsgCommit::opcode;
MsgCommit::MsgCommit(const uint &order, const std::string &message, const int &msghash){
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << order;
    serialized << msghash;
}
MsgCommit::MsgCommit(DataStream &&s){
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    s >> order;
    s >> msghash;
}
//Preprepare message
const opcode_t MsgPreprepare::opcode;
MsgPreprepare::MsgPreprepare(const std::string &message, const std::string signature, const std::string &orderNumber) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << htole((uint32_t)signature.length());
    serialized << signature;
    serialized << htole((uint32_t)orderNumber.length());
    serialized << orderNumber;
}
MsgPreprepare::MsgPreprepare(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    uint32_t len2;
    s >> len2;
    len2 = letoh(len2);
    signature = std::string((const char *)s.get_data_inplace(len2), len2);
    uint32_t len3;
    s >> len3;
    len3 = letoh(len3);
    orderNumber = std::string((const char *)s.get_data_inplace(len3), len3);
}
//MsgGroup to receive message from client
const opcode_t MsgGroup::opcode;
MsgGroup::MsgGroup(const std::string &message) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
}
MsgGroup::MsgGroup(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
}
//MsgPrimaryConsensus between primary group nodes to verify message.
const opcode_t MsgPrimaryConsensus::opcode;
MsgPrimaryConsensus::MsgPrimaryConsensus(const std::string &message, const std::string signature, const std::string &orderNumber) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized << htole((uint32_t)signature.length());
    serialized << signature;
    serialized << htole((uint32_t)orderNumber.length());
    serialized << orderNumber;
}
MsgPrimaryConsensus::MsgPrimaryConsensus(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    uint32_t len2;
    s >> len2;
    len2 = letoh(len2);
    signature = std::string((const char *)s.get_data_inplace(len2), len2);
    uint32_t len3;
    s >> len3;
    len3 = letoh(len3);
    orderNumber = std::string((const char *)s.get_data_inplace(len3), len3);
}
//MsgPrimaryVerified send back acknowledgment to that one node
const opcode_t MsgPrimaryVerified::opcode;
MsgPrimaryVerified::MsgPrimaryVerified(const bool &verified) {
    serialized << verified;
}
MsgPrimaryVerified::MsgPrimaryVerified(DataStream &&s) {
    s >> verified;
}
//MsgSend ask nodes to send reply after 2f
const opcode_t MsgSend::opcode;
MsgSend::MsgSend(const bool &sendReply, const std::string &orderNumber) {
    serialized << htole((uint32_t)orderNumber.length());
    serialized << orderNumber;
    serialized << sendReply;
}
MsgSend::MsgSend(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    orderNumber = std::string((const char *)s.get_data_inplace(len), len);
    s >> sendReply;
}

const opcode_t MsgNextRequest::opcode;
MsgNextRequest::MsgNextRequest(const bool &sendReply) {
    serialized << sendReply;
}
MsgNextRequest::MsgNextRequest(DataStream &&s) {
    s >> sendReply;
}

const opcode_t MsgNextAck::opcode;
MsgNextAck::MsgNextAck(const std::string &message, const salticidae::PeerId &connId) {
    serialized << htole((uint32_t)message.length());
    serialized << message;
    serialized<< connId;

}
MsgNextAck::MsgNextAck(DataStream &&s) {
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);
    s >> connId;
}

//---------------------VIEW CHANGE--------------------------------------------

//Prepare message
const opcode_t MsgRequestViewChange::opcode;
MsgRequestViewChange::MsgRequestViewChange(const std::string &message){
    serialized << htole((uint32_t)message.length());
    serialized << message;
}
MsgRequestViewChange::MsgRequestViewChange(DataStream &&s){
    uint32_t len;
    s >> len;
    len = letoh(len);
    message = std::string((const char *)s.get_data_inplace(len), len);

}