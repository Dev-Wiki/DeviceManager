cmake_minimum_required(VERSION 3.5)

project(DeviceManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置源代码文件编码为UTF-8
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Utils/LogManager.cpp
        Utils/LogManager.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DeviceManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DeviceManager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DeviceManager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DeviceManager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DeviceManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set(WinDevice_INCLUDE_DIR "../WinDevice/output/include/")
set(WinDevice_THIRD_INCLUDE_DIR "../WinDevice/output/third_lib/")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WinDevice_LIB_DIR "../WinDevice/output/debug/")
else()
    set(WinDevice_LIB_DIR "../WinDevice/output/release/")
endif()
# 使用 find_library 查找动态库文件
find_library(WinDevice_LIB
    NAMES WinDevice
    HINTS ${WinDevice_LIB_DIR}
)
# 添加头文件目录
include_directories(${WinDevice_INCLUDE_DIR} ${WinDevice_THIRD_INCLUDE_DIR})
# 将找到的库链接到你的目标
target_link_libraries(DeviceManager PRIVATE ${WinDevice_LIB})

# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     # Debug 模式下，添加 WinDevice 项目作为子项目
#     add_subdirectory(../WinDevice WinDevice)
#     # 链接WinDevice项目
#     target_include_directories(DeviceManager PRIVATE
#         ../WinDevice/src
#         include/third_lib
#     )
#     target_link_libraries(DeviceManager PRIVATE WinDevice)
# else()
#     # Release 模式下，链接 WinDevice 生成的库文件
#     target_link_libraries(DeviceManager PRIVATE WinDevice)
#     # 在 Release 模式下，添加 WinDevice 的头文件路径
#     target_include_directories(DeviceManager PRIVATE
#         ../WinDevice/lib
#     )
# endif()


set_target_properties(DeviceManager PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS DeviceManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DeviceManager)
endif()
