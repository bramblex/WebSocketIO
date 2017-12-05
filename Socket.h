// Socket.h
#ifndef SOCKET_H
#define SOCKET_H

#include "EventEmitter.h"
#include "mongoose/mongoose.h"
#include <map>

#include "json/json.hpp"
using JSON = nlohmann::json;

class Socket : public EventEmitter<std::string, void(JSON)> {
private:
    friend class Server;
    friend class Client;

    mg_connection* nc;
    std::string _addr;

    char send_buffer[1024 * 32];

    Socket(mg_connection* nc);
    void bind(mg_connection* nc);
    void receive(std::string);

public:
    std::string addr();
    void send(std::string, JSON);
};


#endif // SOCKET_H
