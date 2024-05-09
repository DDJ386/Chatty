cmake_minimum_required (VERSION 3.0)

project(Chatty)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

# 设置源文件目录
set(SRCDIR src)

# 设置头文件目录
include_directories(include)

# 查找所有的 .c 文件
file(GLOB C_FILES ${SRCDIR}/*.c)

file(GLOB CMD ${SRCDIR}/commands/*.c)

# 查找所有的 .cpp 文件
file(GLOB CPP_FILES ${SRCDIR}/*.cpp)

# 添加可执行文件
add_executable(main ${C_FILES} ${CPP_FILES} ${CMD})
