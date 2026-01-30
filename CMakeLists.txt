cmake_minimum_required(VERSION 3.5)
project(fiber-core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加fiber目录
add_subdirectory(fiber)

# 添加测试目录
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# 添加压测目录
if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()