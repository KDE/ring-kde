cmake_minimum_required(VERSION 3.0)

project(jamivideoview)

if(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
endif(POLICY CMP0063)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)
include(CMakePackageConfigHelpers)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

SET(CMAKE_C_VISIBILITY_PRESET hidden)
SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

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
    add_definitions(-DJAMIJAMIVIDEOVIEW_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamivideoview_LIB_SRCS
    plugin.cpp
    imageprovider.cpp
    snapshotadapter.cpp
)

qt5_add_resources(jamivideoview_LIB_SRCS
   jamivideo.qrc
)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamivideoview)

add_library(jamivideoview ${jamivideoview_LIB_SRCS} )

target_link_libraries( jamivideoview
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
)

# Configure the target config
set(jamivideoview_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiVideoViewConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiVideoView_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiVideoViewConfig.cmake.in" ${jamivideoview_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiVideoView/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamivideoview_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiVideoView/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamivideoview
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiVideoView>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamivideoview PROPERTIES
    PUBLIC_HEADER
    "${jamivideoview_EXPORT_HDRS}"
)

export(TARGETS jamivideoview
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiVideoViewTargets.cmake"
)

install(TARGETS jamivideoview
    EXPORT JamiVideoViewTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamivideoview
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiVideoViewTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiVideoView
    COMPONENT Devel
)
