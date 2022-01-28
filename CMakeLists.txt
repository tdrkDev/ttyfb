cmake_minimum_required(VERSION 3.22)
project(ttyfb)

# Includes
include_directories(inc)

# Source
set(SOURCE src/main.c src/ttyfb.c src/img.c src/lodepng.c src/ttyui.c)

# Libraries (1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Build
add_executable(ttyfb ${SOURCE})

# Libraries (2)
target_link_libraries(ttyfb PRIVATE Threads::Threads ${CURSES_LIBRARIES})
