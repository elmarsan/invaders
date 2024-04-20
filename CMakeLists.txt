cmake_minimum_required(VERSION 3.22)

project(invaders)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(lib/include)
 
add_executable(invaders_sdl main.cpp)
target_include_directories(invaders_sdl PRIVATE "lib/include")
target_link_directories(invaders_sdl PRIVATE "lib/lib")
target_link_libraries(invaders_sdl PRIVATE SDL2 SDL2main SDL2_image SDL2_mixer)

message("Building ${CMAKE_BUILD_TYPE}")
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")    
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/asset DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(GLOB dll_files "${CMAKE_CURRENT_SOURCE_DIR}/lib/lib/*.dll")
    file(COPY ${dll_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()