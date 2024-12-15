cmake_minimum_required(VERSION 3.29)
project(KursProga1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES "arm64")

add_executable(KursProga1 main.cpp
        Tree.h
        Nodes.h
        Menu.h
)
