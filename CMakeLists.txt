cmake_minimum_required(VERSION 3.27)
project(main)

set(CMAKE_CXX_STANDARD 23)

# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fsanitize=thread")

include_directories(include)

add_executable(main
    main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -O2)
