cmake_minimum_required(VERSION 3.1)

project(mahjong-win-prob)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -Wextra -fsplit-stack -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -fsplit-stack")

add_subdirectory(src/necessary-and-unnecessary-tiles)

add_executable(main1 src/main.cpp src/win_prob1.cpp src/settile.cpp)
target_link_libraries(main1 calsht_dw)
add_executable(main2 src/main.cpp src/win_prob2.cpp src/settile.cpp)
target_compile_definitions(main2 PRIVATE WIN_PROB2=true)
target_link_libraries(main2 calsht_dw)
