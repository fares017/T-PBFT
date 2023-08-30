#include <string>
#include <iostream>
#include "salticidae/event.h"
#include "salticidae/network.h"

#include "../server/config.h"
#include "../server/messages.h"

using Net = salticidae::PeerNetwork<uint8_t>;
using salticidae::DataStream;
using salticidae::htole;
using salticidae::letoh;

using std::cout;

std::unique_ptr<Net> net;

// void message(salticidae::PeerId pid) {
void message(salticidae::PeerId pid1, salticidae::PeerId pid2) {
    usleep(3000000); // in microseconds
    cout << "Sending message!\n";
    // net->send_msg(MsgString("Heyyy"), pid1);
    // net->send_msg(MsgVote(1, "Hello There!"), pid1);
    net->send_msg(MsgOp(1, "A"), pid1);
    // net->send_msg(MsgString("message for other node here!"), pid2);

    // usleep(2000000); // in microseconds
    // exit(0);
}

void ack_handler(MsgAck &&msg, const Net::conn_t &conn) {
    cout << "Got ACK:\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
}

void string_handler(MsgString &&msg, const Net::conn_t &conn) {
    cout << "Got message:\n";
    cout << "\tMessage: " << msg.message << "\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";

    conn->get_net()->send_msg(MsgAck(), salticidae::static_pointer_cast<Net::Conn>(conn));
}

salticidae::PeerId connect_peer(std::string address_string) {
    // Add "managing" node as a peer.
    salticidae::NetAddr address(address_string);
    salticidae::PeerId pid{address};
    net->add_peer(pid);
    net->set_peer_addr(pid, address);
    net->conn_peer(pid);

    return pid;
}

int main() {
    Net::Config config;
    salticidae::EventContext ec;
    // config.ping_period(2);
    config.max_msg_size(MAX_MESSAGE_SIZE);
    config.max_recv_buff_size(MAX_MESSAGE_SIZE);

    net = std::make_unique<Net>(ec, config);

    // Address of "managing" node.
    std::string address_string = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT);
    std::string address_string_1 = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + 1);


    salticidae::NetAddr address(address_string);
    salticidae::PeerId pid{address};
    net->add_peer(pid);
    net->set_peer_addr(pid, address);
    net->conn_peer(pid);

    // Register handlers.
    net->reg_handler(ack_handler);
    net->reg_handler(string_handler);


    // Address of this "client".
    std::string my_address_string = ADDRESS_STRING + ":" + std::to_string(CLIENT_PORT);
    salticidae::NetAddr my_addr(my_address_string);

    // Start and register client network.
    net->start();
    net->listen(my_addr);

    salticidae::PeerId pid1 = connect_peer(address_string);
    salticidae::PeerId pid2 = connect_peer(address_string_1);

    std::thread thread_obj(message, pid1, pid2);

    ec.dispatch();
    return 0;
}

// make && clear && ./main
