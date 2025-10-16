#最小版本要求
cmake_minimum_required(VERSION 3.10)

#项目名称
project(memoryPool_v1 VERSION 1.0 LANGUAGES CXX)

#设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置编译选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

#头文件
include_directories(${PROJECT_SOURCE_DIR}/..)

#源文件
file(GLOB V1_SOURCE "*.cpp")


#生成静态库
add_library(${PROJECT_NAME} STATIC ${V1_SOURCE})

#编译选项
target_compile_options(${PROJECT_NAME} PRIVATE -g)
