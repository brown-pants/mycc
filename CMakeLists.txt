cmake_minimum_required(VERSION 3.10)

project(mycc)

file(GLOB_RECURSE SRC "./src/*.cpp" "./src/*.h")

add_executable(mycc ${SRC})