cmake_minimum_required(VERSION 3.20)
project(NotATop LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Werror)

add_executable(nat 
    src/main.c 
    src/thermal.c 
    src/fan.c 
    src/ui.c 
    src/meminfo.c 
    src/logger.c 
    src/read.c 
    src/cpufreq.c 
    src/loadaverage.c
)

include(FetchContent)

FetchContent_Declare(
    ncurses
    URL https://ftp.gnu.org/gnu/ncurses/ncurses-6.5.tar.gz
)

set(NCURSES_ENABLE_WIDE_CHAR ON CACHE BOOL "" FORCE) 
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)       

FetchContent_MakeAvailable(ncurses)

target_include_directories(nat SYSTEM PRIVATE ${ncurses_SOURCE_DIR}/include)

target_link_libraries(nat PRIVATE ncurses form tinfo)
