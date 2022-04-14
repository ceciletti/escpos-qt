# SPDX-FileCopyrightText: (C) 2022 Daniel Nicoletti <dantti12@gmail.com>
# SPDX-License-Identifier: MIT

set(escposqt_SRC
    escposprinter.cpp
)

set(escposqt_HEADERS
    escposprinter.h
)

set(escposqt_HEADERS_PRIVATE
#     common.h
)

add_library(EscPostQt
    ${escposqt_SRC}
    ${escposqt_HEADERS}
    ${escposqt_HEADERS_PRIVATE}
)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(EscPostQt::Core ALIAS EscPostQt)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(TARGET EscPostQt PROPERTY DEBUG_POSTFIX "d")
endif()

target_compile_definitions(EscPostQt
  PRIVATE
    PLUGINS_PREFER_DEBUG_POSTFIX=$<CONFIG:Debug>
)

target_include_directories(EscPostQt PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/escpos-qt${QT_VERSION_MAJOR}/EscPostQt>
)

set_target_properties(EscPostQt PROPERTIES
    EXPORT_NAME Core
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
)

target_link_libraries(EscPostQt
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    target_link_libraries(EscPostQt PRIVATE Qt6::Core5Compat)
endif()

set_property(TARGET EscPostQt PROPERTY PUBLIC_HEADER ${android_management_HEADERS})
install(TARGETS EscPostQt
    EXPORT EscPosTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION bin COMPONENT runtime
    ARCHIVE DESTINATION lib COMPONENT devel
    PUBLIC_HEADER DESTINATION include/escpos-qt${QT_VERSION_MAJOR}/EscPostQt COMPONENT devel
)
