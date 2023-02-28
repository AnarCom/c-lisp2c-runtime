cmake_minimum_required(VERSION 3.16)
project(GarbageCollector C)

set(CMAKE_C_STANDARD 11)

add_executable(GarbageCollector main.c GarbageCollector.h GarbageCollector.c main.h runtime.c runtime.h)
