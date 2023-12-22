# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.12)

# 设置项目名称和版本，以及编程语言
project(WinDevice VERSION 0.1 LANGUAGES CXX)

# 允许 Windows 导出所有符号
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

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

# DirectX目录
set(Windows_Kits_DIR "C:\\Program Files (x86)\\Windows Kits\\10")
set(Windows_Kits_Version "10.0.19041.0")
set(DirectX_SDK_INCLUDE_DIR "${Windows_Kits_DIR}\\Include\\${Windows_Kits_Version}\\um")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit 架构
    set(CMAKE_LIBRARY_ARCHITECTURE x64)
    set(DirectX_SDK_LIB_DIR "${Windows_Kits_DIR}\\Lib\\${Windows_Kits_Version}\\um\\x64")
else()
    # 32-bit 架构
    set(CMAKE_LIBRARY_ARCHITECTURE x86)
    set(DirectX_SDK_LIB_DIR "${Windows_Kits_DIR}\\Lib\\${Windows_Kits_Version}\\um\\x86")
endif()
# 获取所有 DirectX 相关的库文件
file(GLOB DirectX_LIBS
        "${DirectX_SDK_LIB_DIR}/d3d9.lib"
        "${DirectX_SDK_LIB_DIR}/d3d10.lib"
        "${DirectX_SDK_LIB_DIR}/d3d11.lib"
        "${DirectX_SDK_LIB_DIR}/d3d12.lib"
        "${DirectX_SDK_LIB_DIR}/dxgi.lib"
        "${DirectX_SDK_LIB_DIR}/ddraw.lib"
)

# 使用通配符自动列举源文件
file(GLOB_RECURSE SOURCE_FILES "./src/*")
set(Third_Include_DIR "include/third_lib/")
# 添加功能模块的源文件
add_library(WinDevice SHARED
        ${SOURCE_FILES}
)
# 设置WinDevice库的包含目录
target_include_directories(WinDevice
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        PRIVATE
        ${DirectX_SDK_INCLUDE_DIR}
        ${Third_Include_DIR}
)
# 链接 d3d 的 lib
target_link_libraries(WinDevice PRIVATE ${DirectX_LIBS})

# 创建 output 目录
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/output)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug 模式下的配置
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
    set(LIBRARY_OUTPUT_PATH "${OUTPUT_DIR}/debug")
    # 添加测试
    add_executable(WinDeviceTest "src/main.cpp" ${SOURCE_FILES})
    target_include_directories(WinDeviceTest 
            PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
            PRIVATE ${Third_Include_DIR}  ${DirectX_SDK_INCLUDE_DIR})
    target_link_libraries(WinDeviceTest PRIVATE ${DirectX_LIBS})
else()
    # Release 模式下的配置
    set(LIBRARY_OUTPUT_PATH "${OUTPUT_DIR}/release")
endif()


# 拷贝头文件到构建目录的 include 文件夹下
file(GLOB_RECURSE EXPORTED_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h)
foreach(HEADER ${EXPORTED_HEADERS})
    get_filename_component(HEADER_DIR ${HEADER} DIRECTORY)
    get_filename_component(HEADER_NAME ${HEADER} NAME)
    string(REPLACE "${CMAKE_SOURCE_DIR}/src/" "" HEADER_RELATIVE_PATH ${HEADER_DIR})
    file(COPY ${HEADER} DESTINATION ${OUTPUT_DIR}/include/${HEADER_RELATIVE_PATH})
endforeach()

# 创建 output/third_lib 目录
set(OUTPUT_THIRD_LIB_DIR ${OUTPUT_DIR}/third_lib)
file(MAKE_DIRECTORY ${OUTPUT_THIRD_LIB_DIR})

# 拷贝 include/third_lib 目录下的文件到 output/third_lib 目录中
file(COPY ${CMAKE_SOURCE_DIR}/include/third_lib/ DESTINATION ${OUTPUT_THIRD_LIB_DIR})


target_compile_definitions(WinDevice PRIVATE -DWINDEVICE_EXPORTS)
