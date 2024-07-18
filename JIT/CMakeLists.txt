cmake_minimum_required(VERSION 3.10)
project(AsmJitExample)

# 设置 AsmJit 源码路径
set(ASMJIT_SOURCE_DIR "./asmjit")

# 设置 编译得到的可执行文件路径
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置 源代码列表
set (SRC_LIST ${PROJECT_SOURCE_DIR}/main.cpp)

# 设置命令行参数 最后两个参数表示开启O3，删除这两个参数就表示不开启O3
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

# 包含头文件
include_directories(${ASMJIT_SOURCE_DIR}/src)

# 寻找编译出的动态链接库
#find_library(ASMJIT_LIB asmjit HINTS ${ASMJIT_SOURCE_DIR}/build/)

# 生成的可执行文件叫main，使用的源代码来自 SRC_LIST
add_executable(main ${SRC_LIST})

include("${ASMJIT_SOURCE_DIR}/CMakeLists.txt")
target_Link_Libraries(main asmjit ${ASMJIT_DEPS})

# 链接 AsmJit 库
#target_link_libraries(main ${ASMJIT_LIB})