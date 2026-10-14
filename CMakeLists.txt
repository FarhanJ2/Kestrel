cmake_minimum_required(VERSION 3.12)
project(Kestrel)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
add_compile_options(-Wno-psabi)

# Prefer -lpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Required packages
find_package(phoenix6 REQUIRED)
find_package(Threads REQUIRED)

# NOTE: The units library can use either <fmt/format.h> or <iostream>, use iostream
add_definitions(-DUNIT_LIB_DISABLE_FMT -DUNIT_LIB_ENABLE_IOSTREAM)

# Source files are discovered automatically. CONFIGURE_DEPENDS re-checks the
# lists on every build, so adding or removing a file needs no edit here.

# kestrel_core: the pure logic in src/core. It must not depend on Phoenix or
# any other hardware library, so it can be unit tested without the bike.
file(GLOB_RECURSE KESTREL_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/core/*.cpp")
add_library(kestrel_core STATIC ${KESTREL_CORE_SOURCES})
target_include_directories(kestrel_core PUBLIC "${CMAKE_SOURCE_DIR}/src")

# Kestrel: the program that runs on the Pi (src/*.cpp and src/hw)
file(GLOB KESTREL_APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE KESTREL_HW_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/hw/*.cpp")
add_executable(${PROJECT_NAME} ${KESTREL_APP_SOURCES} ${KESTREL_HW_SOURCES})
target_link_libraries(${PROJECT_NAME} kestrel_core phoenix6 Threads::Threads)

# kestrel_tests: unit tests for kestrel_core (tests/*.cpp), run with `ctest --test-dir build`
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    file(GLOB_RECURSE KESTREL_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/tests/*.cpp")
    add_executable(kestrel_tests ${KESTREL_TEST_SOURCES})
    target_link_libraries(kestrel_tests kestrel_core GTest::gtest_main)
    add_test(NAME kestrel_tests COMMAND kestrel_tests)
else()
    message(STATUS "GoogleTest not found, skipping kestrel_tests")
endif()
