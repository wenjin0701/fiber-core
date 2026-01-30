cmake_minimum_required(VERSION 3.5)

# 获取当前源文件所在目录的绝对路径
get_filename_component(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

# 设置源文件和头文件目录
set(SRC_DIR ${SRC_DIR}/src)
set(INCLUDE_DIR ${SRC_DIR}/../include)

# 将src目录下所有源文件添加到 SRC_LIST 变量中
aux_source_directory(${SRC_DIR} SRC_LIST)

# 添加头文件包含路径
include_directories(${INCLUDE_DIR})

# 创建库
add_library(fiber_lib ${SRC_LIST})

# 链接依赖库
target_link_libraries(fiber_lib -ldl -lpthread)

# 设置变量
set(src_fiber ${SRC_LIST} CACHE INTERNAL "Fiber source files")
set(inc_fiber ${INCLUDE_DIR} CACHE INTERNAL "Fiber include files")