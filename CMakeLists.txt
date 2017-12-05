cmake_minimum_required(VERSION 3.8)
project(WebsocketIO)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCE_FILES "*.cpp" "*.h" "json/*" "mongoose/*")
add_library(WebsocketIO ${SOURCE_FILES})

option(BUILD_EXAMPLE "Build WebsocketIO examples." ON)

if(BUILD_EXAMPLE)
    add_executable(client "example/client")
    target_link_libraries(client WebsocketIO)

    add_executable(server "example/server")
    target_link_libraries(server WebsocketIO)
endif()
