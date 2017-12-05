
#include "../Server.h"

int main() {

    auto server = new Server("web");

    server->onConnection([server](Socket* socket){

        server->broadcast("login", { {"addr", socket->addr()} });

        socket->on("message", [server, socket](JSON data){
            server->broadcast("message", {{"addr", socket->addr()}, {"content", data}});
        });

        socket->on("disconnect", [server, socket](JSON){
            server->broadcast("leave", {{"addr", socket->addr()}});
        });

        socket->on("close-server", [server](JSON){
            server->close();
        });

    });

    server->listen(8000, 200);

    return 0;
}
