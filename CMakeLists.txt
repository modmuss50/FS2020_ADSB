cmake_minimum_required(VERSION 3.17)
project(FS2020_ADSB)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FS2020_ADSB main.cpp SimConnect.h tcp_server.h tcp_server.cpp)
target_link_libraries(FS2020_ADSB "C:\\MSFS SDK\\SimConnect SDK\\lib\\SimConnect.lib")
target_link_libraries(FS2020_ADSB wsock32 ws2_32)