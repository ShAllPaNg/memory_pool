#最小版本要求
cmake_minimum_required(VERSION 3.10)

#项目名称
project(testMPV1 VERSION 1.0 LANGUAGES CXX)

#设置c++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#设置编译选项
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

#头文件路径
include_directories(${PROJECT_SOURCE_DIR}/../..)


#静态链接库路径
link_directories(${PROJECT_SOURCE_DIR}/../../src)

#源文件
file(GLOB TEST_SRC "*.cpp")


#生成可执行文件
add_executable(${PROJECT_NAME} ${TEST_SRC})

#链接静态库
target_link_libraries(${PROJECT_NAME}
    memoryPool_v1
)


#编译选项
target_compile_options(${PROJECT_NAME} PRIVATE -g)
