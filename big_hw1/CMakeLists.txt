cmake_minimum_required(VERSION 3.27)
project(brute_force_node LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)

# Set properties
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PROJECT_LIB_NAME ${PROJECT_NAME}_lib)

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/main.cpp)

# Library with all logic (shared by main and tests)
add_library(${PROJECT_LIB_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_LIB_NAME} PUBLIC 
  ${CMAKE_SOURCE_DIR}/include
)

message(${CMAKE_SOURCE_DIR})

# Main executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
