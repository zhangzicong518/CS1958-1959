cmake_minimum_required(VERSION 3.16)
project(minivim)

set(CMAKE_CXX_STANDARD 14)

add_executable(minivim main.cpp header.h src.cpp)

# make your compiler aware of header directory
include_directories(${PROJECT_SOURCE_DIR}/include)
# find & include ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARY})