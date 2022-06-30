cmake_minimum_required(VERSION 3.22)
project(so_project C)

set(CMAKE_C_STANDARD 11)

include_directories(utils)

add_executable(so_project
        main.c
        utils/list.c
        utils/list.h
        synth/synth.c
        synth/synth.h
        synth/instr_parser.c
        synth/instr_parser.h)
