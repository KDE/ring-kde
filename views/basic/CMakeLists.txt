cmake_minimum_required(VERSION 3.0)

project(basicview)

if(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
endif(POLICY CMP0063)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

include(ECMOptionalAddSubdirectory)
include(CMakePackageConfigHelpers)

include(KDECMakeSettings)

# When used with `add_subdirectory`, assume it is a static Qt plugin
get_directory_property(USES_ADD_SUBDIRECTORY PARENT_DIRECTORY)
if(USES_ADD_SUBDIRECTORY)
   set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)

if(NOT BUILD_SHARED_LIBS)
    set(ENABLE_STATIC_PLUGIN 1) # Used in Config.cmake.in
    add_definitions(-DQT_PLUGIN)
    add_definitions(-DQT_STATICPLUGIN=1)
    add_definitions(-DBASICVIEW_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( basicview_LIB_SRCS
    basicviewplugin.cpp
)

qt5_add_resources(basicview_LIB_SRCS
    basic.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.basicview)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

add_library(basicview ${basicview_LIB_SRCS} )

target_link_libraries( basicview
    # Project
#     ringqt
#     ringqtquick
#     jamikdeintegration

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
)

# Configure the target config
set(basicview_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/BasicViewConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(BasicView_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BasicViewConfig.cmake.in" ${basicview_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/BasicView/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${basicview_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/BasicView/
    COMPONENT Devel
)

# Create the target
target_include_directories(basicview
    PUBLIC
        $<INSTALL_INTERFACE:include/BasicView>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(basicview PROPERTIES
    PUBLIC_HEADER
    "${basicview_EXPORT_HDRS}"
)

export(TARGETS basicview
    FILE "${PROJECT_BINARY_DIR}/cmake/BasicViewTargets.cmake"
)

install(TARGETS basicview
    EXPORT BasicViewTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT basicview
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT BasicViewTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/BasicView
    COMPONENT Devel
)
