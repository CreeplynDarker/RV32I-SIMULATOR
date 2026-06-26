cmake_minimum_required(VERSION 3.16)
project(rv32i_sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rv32i_sim
        src/main.cpp
        src/memory.cpp
        src/decode.cpp
        src/simulator.cpp
        src/execute.cpp
        src/syscall.cpp
        src/disasm.cpp
)

target_include_directories(rv32i_sim PRIVATE src)