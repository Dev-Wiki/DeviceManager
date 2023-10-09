cmake_minimum_required(VERSION 3.12)

project(WinDevice)

# 添加主项目的源文件
add_executable(WinDevice
    
)

# 设置头文件目录，包括spdlog头文件和功能模块的头文件目录
target_include_directories(WinDevice PRIVATE
    include/
    include/third_lib/
    src/Audio/
    src/Video/
    src/Utils/
)

# 添加功能模块的源文件
target_sources(WinDevice PRIVATE
    src/Audio/*.cpp
    src/Video/*.cpp
    src/Utils/*.cpp
)
