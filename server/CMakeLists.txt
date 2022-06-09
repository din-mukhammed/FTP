cmake_minimum_required(VERSION 3.12)
project(MyProj VERSION 1.0.0)

add_library(
    server_lib
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/ftp_message.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/ftp_message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/server.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp"
)

add_executable(server src/main.cpp)
target_compile_features(server PRIVATE cxx_std_17)

target_link_libraries(server PUBLIC server_lib)