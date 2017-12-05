# WebSocketIO

WebsocketIO 是一个 soket.io 风格 WebScoket C++ 实现。

## 例子

一个简单的 WebSocket 的连天室服务端

```cpp
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
```

一个简单的接收客户端

```cpp
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
```

网页端收发消息代码

```JavaScript
const socket = new WebSocketIO({port: 8000})

const vm = new Vue({
    el: "#app",
    data: {
        text_content: "hello, this is a message box",
        input_text: "type some text"
    },
    methods: {
        submit() {
            socket.send("message", this.input_text)
            this.input_text = ''
        }
    }
})

socket.on("connection", () => {
    socket.on("login", ({addr}) => {
        vm.text_content += `\n[${addr}] logined.`
    })

    socket.on("leave", ({addr}) => {
        vm.text_content += `\n[${addr}] left.`
    })

    socket.on("message", ({addr, content}) => {
        vm.text_content += `\n[${addr}] say: ${content}`
    })
})
```

具体用法参见 example 文件夹。
