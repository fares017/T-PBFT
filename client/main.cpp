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

void message(std::vector<salticidae::PeerId> peers) {
    usleep(3000000); // in microseconds
    cout << "Sending message!\n";

    // Send message request with a = 25 and b = 75
    //net->send_msg(MsgRequest(25, 75), pid);
    //net->send_msg(MsgPreprepare("at"), pid);
    net->multicast_msg(MsgGroup("attackerrelease"), peers);
    // net->send_msg(MsgGroup("attackerrelease"), pid2);
    // net->send_msg(MsgGroup("attackerrelease"), pid3);
    //usleep(2000000); // in microseconds
    //exit(0);
}

// void ack_handler(MsgAck &&msg, const Net::conn_t &conn) {
//     cout << "Got ACK:\n";
//     cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
// }

// void string_handler(MsgString &&msg, const Net::conn_t &conn) {
//     cout << "Got message:\n";
//     cout << "\tMessage: " << msg.message << "\n";
//     cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";

//     conn->get_net()->send_msg(MsgAck(), salticidae::static_pointer_cast<Net::Conn>(conn));
// }



// Handler to display message that we get from the server.
void reply_handler(MsgReply &&msg, const Net::conn_t &conn) {
    cout << "Got reply from server:\n";
    cout << "\tFrom " << conn->get_peer_id().to_hex().substr(0, 10) << "\n";
    cout << "\tc: " << msg.c << "\n";
    cout << "\tis faulty?: " << msg.fault << "\n";
}

void ack_handler(MsgAck &&msg, const Net::conn_t &conn) {
     cout << "Got ACK: \n";
     cout << "\tFrom " << conn->get_peer_id().to_hex() << "\n";
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
    // std::string address_string_1 = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + 1);
    // std::string address_string_2 = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + 2);

    salticidae::NetAddr address(address_string);
    salticidae::PeerId pid{address};
    net->add_peer(pid);
    net->set_peer_addr(pid, address);
    net->conn_peer(pid);

    // Register handlers.
    //net->reg_handler(ack_handler);
    //net->reg_handler(string_handler);
    std::vector<salticidae::PeerId> peers;
    peers.resize(NUM_NODES);
    for (uint i = 0; i < NUM_NODES; i++) { 
        std::string address_string = ADDRESS_STRING + ":" + std::to_string(ADDRESS_PORT + i);
        salticidae::PeerId pid1 = connect_peer(address_string);
        peers[i] = pid1;

    }
    // Register the handler for handling reply type messages.
    net->reg_handler(reply_handler);
    net->reg_handler(ack_handler);


    // Address of this "client".
    std::string my_address_string = ADDRESS_STRING + ":" + std::to_string(CLIENT_PORT);
    salticidae::NetAddr my_addr(my_address_string);

    // Start and register client network.
    net->start();
    net->listen(my_addr);

    // salticidae::PeerId pid1 = connect_peer(address_string);
    // salticidae::PeerId pid2 = connect_peer(address_string_1);
    // salticidae::PeerId pid3 = connect_peer(address_string_2);

    std::thread thread_obj(message, peers);


    ec.dispatch();
    return 0;
}

// make && clear && ./main
