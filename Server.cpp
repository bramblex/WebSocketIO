// Server.cpp
#include "Server.h"

static std::map<mg_mgr*, Server*> WEBSOCKET_SERVERS;

Server::Server(std::string document_root){
    document_root.copy(this->path_buffer,  document_root.length(), 0);
    this->path_buffer[document_root.length()] = '\0';

    this->is_runing = false;
    this->s_http_server_opts.document_root = this->path_buffer;
    this->s_http_server_opts.enable_directory_listing = "yes";

    WEBSOCKET_SERVERS[&this->mgr] = this;
}

Server::~Server(){
    for (auto it: this->sockets) {
        delete it.second;
    }
    this->close();

    WEBSOCKET_SERVERS.erase(&this->mgr);
}

int Server::isWebsocket(const mg_connection *nc) {
    return nc->flags & MG_F_IS_WEBSOCKET;
}

void Server::eventHandler(mg_connection *nc, int ev, void *ev_data) {
    Server* server = WEBSOCKET_SERVERS[nc->mgr];
    Socket* socket = nullptr;
    if (Server::isWebsocket(nc)) {
        socket = server->getOrCreate(nc);
    }

    switch (ev) {
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
        server->on_connection_handler(socket);
        socket->emit("connection", {});
        break;
    }
    case MG_EV_WEBSOCKET_FRAME: {
        struct websocket_message *wm = static_cast<struct websocket_message *>(ev_data);
        struct mg_str d = { reinterpret_cast<const char *>(wm->data), wm->size};
        char buf[1024 * 32];
        snprintf(buf, sizeof(buf), "%.*s", static_cast<int>(d.len), d.p);

        std::string message = buf;
        socket->receive(message);

        break;
    }
    case MG_EV_HTTP_REQUEST: {
        mg_serve_http(nc, static_cast<struct http_message *>(ev_data), server->s_http_server_opts);
        break;
    }
    case MG_EV_CLOSE: {
        if (socket != nullptr) {
            auto addr = socket->addr();
            socket->emit("disconnect", {});
            server->remove(nc);
        }
        break;
    }
    }
}

Socket* Server::getOrCreate(mg_connection* nc){
    auto socket = this->sockets[nc];
    if (socket == nullptr) {
        socket = new Socket(nc);
        this->sockets[nc] = socket;
    }
    return socket;
}

void Server::remove(mg_connection* nc){
    auto socket = this->sockets[nc];
    if (socket != nullptr) {
        delete socket;
    }
    this->sockets.erase(nc);
}

void Server::broadcast(std::string event, JSON data) {
    for (auto it : this->sockets) {
        it.second->send(event, data);
    }
}

void Server::listen(int s_http_port, int wait_timeout){
    this->is_runing = true;
    this->s_http_port = std::to_string(s_http_port) ;
    this->wait_timeout = wait_timeout;
    mg_mgr_init(&this->mgr, nullptr);
    this->nc = mg_bind(&this->mgr, this->s_http_port.c_str(), Server::eventHandler);
    mg_set_protocol_http_websocket(this->nc);

    while(this->is_runing) {
        this->poll();
    }
    mg_mgr_free(&mgr);
}

void Server::listen(int s_http_port) {
    this->listen(s_http_port, 200);
}

void Server::close(){
    this->is_runing = false;
}

void Server::poll(){
    mg_mgr_poll(&mgr, this->wait_timeout);
}

void Server::onConnection(std::function<void(Socket*)> on_connection_handler) {
    this->on_connection_handler = on_connection_handler;
}
