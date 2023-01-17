cmake_minimum_required(VERSION 3.25)
project(deb-extractor C)

set(CMAKE_C_STANDARD 11)

add_executable(deb-extractor main.c deb.c deb.h)
