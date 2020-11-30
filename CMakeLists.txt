cmake_minimum_required(VERSION 3.17)
project(IMS)

set(CMAKE_CXX_STANDARD 14)

add_executable(IMS main.cpp Worker.cpp Worker.h DataCollector.cpp DataCollector.h)