cmake_minimum_required(VERSION 3.19)
project(so_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(utils)

add_executable(so_project
        main.c
        cpu/cpu.c
        cpu/cpu.h
        tools/list.c
        tools/list.h
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
        terminal/terminal.c
        terminal/terminal.h
        print/print.c
        print/print.h
        file/file.c
        file/file.h
        tools/math.h
        terminal/log.c
        terminal/log.h
        tools/ostools.h
        )
