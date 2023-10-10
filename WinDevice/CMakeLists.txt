# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.12)

# 设置项目名称和版本，以及编程语言
project(WinDevice VERSION 0.1 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加源代码文件编码为UTF-8的选项，可选择启用或禁用
option(ENABLE_UTF8_SOURCE "Enable UTF-8 source code encoding" ON)

# 如果启用了UTF-8源码编码选项
if (ENABLE_UTF8_SOURCE)
    # 如果是MSVC编译器
    if (MSVC)
        # 设置MSVC编译器的UTF-8源码编码选项
        add_compile_options("/source-charset:utf-8")
        # 如果是MinGW编译器
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        # 设置MinGW编译器的UTF-8源码编码选项
        add_compile_options("-fexec-charset=UTF-8" "-finput-charset=UTF-8")
    endif()
endif()

# 使用通配符自动列举源文件
file(GLOB PROJECT_SOURCES
    src/*.cpp
    src/*.h
    src/Audio/*.cpp
    src/Audio/*.h
    src/Video/*.cpp
    src/Video/*.h
    src/Utils/*.cpp
    src/Utils/*.h
)

# 添加功能模块的源文件
add_library(WinDevice ${PROJECT_SOURCES})

# 设置WinDevice库的公共包含目录
target_include_directories(WinDevice PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
)

# 设置WinDevice库的私有包含目录，包括第三方库和源码目录
set(THIRD_LIB_DIR ${CMAKE_SOURCE_DIR}/include/third_lib)
target_include_directories(WinDevice PRIVATE
    include/third_lib
    src
)