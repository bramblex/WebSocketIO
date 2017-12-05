// Client.h
#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.h"
#include "mongoose/mongoose.h"

#include <string>
#include <map>
#include <functional>

class Client : public Socket {
private:
    int done = 0;
    int connected = 0;

    mg_mgr mgr;

    static void eventHandler(mg_connection* nc, int ev, void* ev_data);

public:
    Client();
    Client(std::string host, int port);
    ~Client();
    void close();
    void poll(int timeout);
};

#endif // CLIENT_H
