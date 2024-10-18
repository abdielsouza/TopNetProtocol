project(TopNetProtocol)
cmake_minimum_required(VERSION 3.10)

include_directories(
    src/**
)

add_executable(TopNet src/toptest.c src/libtop.h src/libtop.c)