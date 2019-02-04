cmake_minimum_required(VERSION 3.0)

project(jamicallview)

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
    add_definitions(-DJAMICALLVIEW_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamicallview_LIB_SRCS
    callviewplugin.cpp
)

qt5_add_resources(jamicallview_LIB_SRCS
   callview.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamicallview)

add_library(jamicallview ${jamicallview_LIB_SRCS} )

target_link_libraries( jamicallview
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
)

if(X11_FOUND)
    target_link_libraries( jamiaccountview
        Qt5::X11Extras
        ${X11_X11_LIB}
    )
endif()

# Configure the target config
set(jamicallview_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiCallViewConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiCallView_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiCallViewConfig.cmake.in" ${jamicallview_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiCallView/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamicallview_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiCallView/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamicallview
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiCallView>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamicallview PROPERTIES
    PUBLIC_HEADER
    "${jamicallview_EXPORT_HDRS}"
)

export(TARGETS jamicallview
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiCallViewTargets.cmake"
)

install(TARGETS jamicallview
    EXPORT JamiCallViewTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamicallview
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiCallViewTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiCallView
    COMPONENT Devel
)
