cmake_minimum_required(VERSION 3.12)
project(MyProj VERSION 1.0.0)

add_library(
    client_lib
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/client.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp"
)

add_executable(client src/main.cpp)
target_compile_features(client PRIVATE cxx_std_17)

target_link_libraries(client PUBLIC client_lib)