cmake_minimum_required(VERSION 4.1)
project(ProcessThreadManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Library sources
set(LIB_SOURCES
        src/ProcessManager.cpp
        src/ThreadPool.cpp
        src/IPC.cpp
        src/Synchronization.cpp
)

# Create static library
add_library(ptmanager STATIC ${LIB_SOURCES})

target_include_directories(ptmanager PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Test executable
add_executable(test_manager test/main.cpp)
target_link_libraries(test_manager PUBLIC ptmanager pthread rt)
