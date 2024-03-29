# SPDX-FileCopyrightText: (C) 2022 Daniel Nicoletti <dantti12@gmail.com>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)
project(EscPosQt VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Include our cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.12.0 COMPONENTS Core REQUIRED)
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} 5.12.0 COMPONENTS Core5Compat REQUIRED)
endif()
#
# Options
#
option(ENABLE_MAINTAINER_CFLAGS "Enable maintainer CFlags" OFF)

set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE PATH "Output directory for libraries")

option(BUILD_SHARED_LIBS "Build in shared lib mode" ON)

#
# Custom C flags
#
if (ENABLE_MAINTAINER_CFLAGS)
    set (MAINTAINER_CFLAGS "-Werror -Wall -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Wformat -Winit-self")
    add_definitions(${MAINTAINER_CFLAGS}
        -DQT_DEPRECATED_WARNINGS
    )
endif ()

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050c00
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/EscPosQt
)

# cmake config files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/escposqt-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EscPosQt${QT_VERSION_MAJOR}Config.cmake
    @ONLY
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/escposqt-config-version.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EscPosQt${QT_VERSION_MAJOR}ConfigVersion.cmake
    @ONLY
)
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EscPosQt${QT_VERSION_MAJOR}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EscPosQt${QT_VERSION_MAJOR}ConfigVersion.cmake
    DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/EscPosQt${QT_VERSION_MAJOR}/
)

install(EXPORT EscPosTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EscPosQt${QT_VERSION_MAJOR}/
    FILE EscPos${PROJECT_VERSION_MAJOR}Qt${QT_VERSION_MAJOR}Targets.cmake
    NAMESPACE EscPosQt::
    COMPONENT Devel
)

add_subdirectory(src)
include(CPackConfig)
