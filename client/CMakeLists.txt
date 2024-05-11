cmake_minimum_required (VERSION 3.0)

project(Chatty)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

set(SRCDIR src)

include_directories(include)

file(GLOB C_FILES ${SRCDIR}/*.c)

file(GLOB CMD ${SRCDIR}/commands/*.c)

file(GLOB CPP_FILES ${SRCDIR}/*.cpp)

# 添加可执行文件
add_executable(main ${C_FILES} ${CPP_FILES} ${CMD})
