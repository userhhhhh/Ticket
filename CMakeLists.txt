cmake_minimum_required(VERSION 3.22)
project(Ticket)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")


include_directories(.)
include_directories(B_plus_tree)
include_directories(other)
include_directories(map)


#set(CMAKE_RUNTIME_OUTPUTDIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(code
        B_plus_tree/BPlusTree.cpp
        B_plus_tree/BPlusTree.h
        other/usersystem.h
        other/main.cpp
        other/command.hpp
        map/exceptions.hpp
        map/map.hpp
        map/utility.hpp
)
