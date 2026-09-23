cmake_minimum_required(VERSION 3.10)
project(notatop C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

include_directories(include)

add_executable(nat 
    src/main.c 
    src/thermal.c 
    src/fan.c
    src/ui.c
    src/meminfo.c 
    src/logger.c
    src/read.c
)

find_package(Curses REQUIRED)

target_include_directories(nat PRIVATE ${CURSES_INCLUDE_DIRS})

target_link_libraries(nat PRIVATE ${CURSES_LIBRARIES})

