cmake_minimum_required(VERSION 3.27)
project(main)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(main
    main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -mavx -O2)
