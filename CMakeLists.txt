cmake_minimum_required(VERSION 3.19)
project(so_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(utils)

add_executable(so_project
        main.c
        constants.h
        cpu/cpu.c
        cpu/cpu.h
        utils/list.c
        utils/list.h
        kernel/kernel.c
        kernel/kernel.h
        memory/memory.c
        memory/memory.h
        process/process.c
        process/process.h
        process/instruction.c
        process/instruction.h
        sched/sched.c
        sched/sched.h
        semaphore/semaphore.c
        semaphore/semaphore.h
        disk/disk.c
        disk/disk.h
        terminal/terminal.h
        terminal/terminal.c
        )
