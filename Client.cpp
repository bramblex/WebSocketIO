// Client.cpp
#include "Client.h"

static std::map<mg_mgr*, Client*> WEBSOCKET_CLIENTS;

void Client::eventHandler(mg_connection* nc, int ev, void* ev_data){
    auto client = WEBSOCKET_CLIENTS[nc->mgr];

    switch (ev) {
    case MG_EV_CONNECT: {
        int status = *(static_cast<int *>(ev_data));
        if (status != 0) {
            client->emit("error", {
                             {"code", status},
                             {"message", "Connection error"}
                         });
        }
        break;
    }
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
        client->connected = 1;
        client->emit("connection", {});
        break;
    }
    case MG_EV_WEBSOCKET_FRAME: {

        struct websocket_message *wm = static_cast<struct websocket_message *>(ev_data);
        struct mg_str d = { reinterpret_cast<const char *>(wm->data), wm->size};
        char buf[1024 * 32];
        snprintf(buf, sizeof(buf), "%.*s", static_cast<int>(d.len), d.p);

        std::string message = buf;
        client->receive(message);

        break;
    }
    case MG_EV_CLOSE: {
        if (client->connected) {
            client->emit("disconnect", {});
        }
        client->done = 1;
        break;
    }
    }

}

Client::Client() : Client("127.0.0.1", 8000){
}

Client::Client(std::string host, int port) : Socket(nullptr){
    mg_mgr_init(&this->mgr, nullptr);
    auto url = "ws://" + host + ":" + std::to_string(port);
    char url_buf[36];
    url.copy(url_buf, url.length(), 0);
    url_buf[url.length()] = '\0';

    auto nc = mg_connect_ws(&this->mgr, Client::eventHandler, url_buf, "ws", nullptr);
    this->bind(nc);

    WEBSOCKET_CLIENTS[&this->mgr] = this;
}

Client::~Client(){
    WEBSOCKET_CLIENTS.erase(&this->mgr);
}

void Client::close(){ this->done = 1; }

void Client::poll(int timeout){
    while(!this->done) {
        mg_mgr_poll(&this->mgr, timeout);
    }
    mg_mgr_free(&this->mgr);
}
