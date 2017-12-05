// Socket.cpp

#include "Socket.h"


Socket::Socket(mg_connection* nc){
    if (nc != nullptr)
        this->bind(nc);
}

void Socket::bind(mg_connection* nc){
    this->nc = nc;
    char addr[32];
    mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
    this->_addr = addr;
}

std::string Socket::addr() {
    return this->_addr;
}

void Socket::send(std::string event, JSON data) {
    JSON message = {{"event", event},  {"data", data}};
    auto result = message.dump();

    result.copy(this->send_buffer, result.length(), 0);
    mg_send_websocket_frame(this->nc, WEBSOCKET_OP_TEXT, this->send_buffer, result.length());
}

void Socket::receive(std::string raw_message) {
    try {
        auto message = JSON::parse(raw_message);
        auto event = message["event"].get<std::string>();
        auto data = message["data"];
        this->emit(event, data);
    } catch (std::invalid_argument) {
        std::cout << "[warn] message parsed failed" << std::endl;
        std::cout << "message: " << raw_message << std::endl;
    }
}
