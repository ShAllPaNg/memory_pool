#最小版本要求
cmake_minimum_required(VERSION 3.10)

#项目名称
project(all VERSION 1.0 LANGUAGES CXX)

#设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置编译选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

#子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/app/testPerf)
add_subdirectory(${PROJECT_SOURCE_DIR}/app/testFunc)
