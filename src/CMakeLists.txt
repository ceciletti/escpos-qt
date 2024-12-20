# SPDX-FileCopyrightText: (C) 2022-2024 Daniel Nicoletti <dantti12@gmail.com>
# SPDX-License-Identifier: MIT

set(escposqt_SRC
    escposprinter.cpp
)

set(escposqt_HEADERS
    escposprinter.h
)

add_library(EscPosQt
    ${escposqt_SRC}
    ${escposqt_HEADERS}
)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(EscPosQt::Core ALIAS EscPosQt)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(TARGET EscPosQt PROPERTY DEBUG_POSTFIX "d")
endif()

target_compile_definitions(EscPosQt
  PRIVATE
    PLUGINS_PREFER_DEBUG_POSTFIX=$<CONFIG:Debug>
)

target_include_directories(EscPosQt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/escpos-qt${QT_VERSION_MAJOR}/EscPosQt>
)

set_target_properties(EscPosQt PROPERTIES
    EXPORT_NAME Core
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

target_link_libraries(EscPosQt
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    target_link_libraries(EscPosQt PRIVATE Qt6::Core5Compat)
endif()
set_property(TARGET EscPosQt PROPERTY PUBLIC_HEADER ${android_management_HEADERS})

generate_export_header(EscPosQt)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/escposqt_export.h
    DESTINATION include/escpos-qt${QT_VERSION_MAJOR}/EscPosQt
)

install(TARGETS EscPosQt
    EXPORT EscPosTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION bin COMPONENT runtime
    ARCHIVE DESTINATION lib COMPONENT devel
    PUBLIC_HEADER DESTINATION include/escpos-qt${QT_VERSION_MAJOR}/EscPosQt COMPONENT devel
)
