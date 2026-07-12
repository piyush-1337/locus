cmake_minimum_required(VERSION 3.16)

project(locus)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/query.cpp
    src/utils.cpp
    src/cli.cpp
    src/resolver.cpp
)

add_executable(locus
    src/main.cpp
    ${SOURCES}
)

target_include_directories(
    locus
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

include(GNUInstallDirs)
install(TARGETS locus
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# testing
option(BUILD_TESTS "Build the test suite and fetch GTest" OFF)

if (BUILD_TESTS)
    include(FetchContent)

    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()

    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(locus_test
        tests/main.cpp
        ${SOURCES}
    )

    target_link_libraries(
        locus_test
        GTest::gtest_main
    )

    target_include_directories(
        locus_test
        PRIVATE ${PROJECT_SOURCE_DIR}/include
    )

    include(GoogleTest)
    gtest_discover_tests(locus_test)
endif()
