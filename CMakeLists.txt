cmake_minimum_required(VERSION 4.0)
project(Assembler)

set(CMAKE_CXX_STANDARD 99)

add_executable(Assembler
        src/connorrAssembler.c
        src/connorrAssembler.c
        src/trace.h
        src/map/map.c
        src/constants/constants.h
        src/convert/convert.c
        src/convert/convert.h
        src/map/map.h
        src/constants/constants.c
        src/run/run.c
        src/run/run.h
)
