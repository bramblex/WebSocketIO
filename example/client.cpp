
#include "../Client.h"
#include <iostream>
#include <string>

int main(){

    auto client = new Client("127.0.0.1", 8000);

    client->on("connection", [client](JSON){

        client->send("message", "Hello, this is cpp client.");

        client->on("message", [](JSON data){
            std::cout << "[message]" << data.dump() << std::endl;
        });

        client->on("login", [](JSON data){
            std::cout << "[login]" << data.dump() << std::endl;
        });

        client->on("leave", [](JSON data){
            std::cout << "[leave]" << data.dump() << std::endl;
        });
    });

    client->poll(200);
    return 0;
}
