// Server.h
#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

#include "mongoose/mongoose.h"
#include <map>
#include <string>

class Server {
private:
    std::map<mg_connection*, Socket*> sockets;

    char path_buffer[128];

    std::string s_http_port;
    mg_serve_http_opts s_http_server_opts;
    int wait_timeout;

    mg_mgr mgr;
    mg_connection* nc;

    Socket* getOrCreate(mg_connection*);
    void remove(mg_connection*);


    bool is_runing;

    static int isWebsocket(const mg_connection*);
    static void eventHandler(mg_connection*, int, void*);

    std::function<void(Socket*)> on_connection_handler = [](Socket*){};
    void poll();

public:
    Server(std::string document_root);
    ~Server();

    void listen(int, int);
    void listen(int);
    void close();

    void broadcast(std::string, JSON);

    void onConnection(std::function<void(Socket*)>);
};

#endif // SERVER_H
