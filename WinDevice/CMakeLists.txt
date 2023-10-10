cmake_minimum_required(VERSION 3.12)

project(WinDevice VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置源代码文件编码为UTF-8
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

# 添加sdplog目录
include_directories(include/third_lib)

# 添加功能模块的源文件
set(PROJECT_SOURCES
    "src/Audio"
    "src/Video"
    "src/Utils"
)
