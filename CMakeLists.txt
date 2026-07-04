cmake_minimum_required(VERSION 3.16)

project(locus)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.cpp
    src/query.cpp
    src/utils.cpp
    src/cli.cpp
    src/resolver.cpp
)

add_executable(locus ${SOURCES})

target_include_directories(
    locus
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)
