cmake_minimum_required(VERSION 3.1)

project(mahjong-win-prob)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -Wextra -fsplit-stack -g -DCHECK_HAND")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -fsplit-stack")
set(lib ${PROJECT_SOURCE_DIR}/src/necessary-and-unnecessary-tiles)

execute_process(COMMAND ${CMAKE_COMMAND} -E tar zxvf ${lib}/index_dw.tar.gz
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

option(THREE_PLAYER "Three player mode" off)

add_compile_definitions(INDEX_FILE_PATH="${PROJECT_BINARY_DIR}")

include_directories(${lib})

add_executable(main src/main.cpp src/win_prob.cpp ${lib}/calsht_dw.cpp)
add_executable(main2 src/main.cpp src/win_prob2.cpp ${lib}/calsht_dw.cpp)
target_compile_definitions(main2 PRIVATE WIN_PROB2)
