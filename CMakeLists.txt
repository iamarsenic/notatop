cmake_minimum_required(VERSION 3.10)
project(NotATop C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -flto -O3 -pipe -Wall -Wextra")
set(CMAKE_BUILD_TYPE Debug)

add_executable(nat
	src/main.c
	src/read.c
	src/meminfo.c
)

find_package(Curses REQUIRED)
target_include_directories(nat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(nat PRIVATE ${CURSES_LIBRARIES} m)

install(TARGETS nat	DESTINATION bin)

add_custom_target(uninstall
	COMMAND xargs rm -fv < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
)
