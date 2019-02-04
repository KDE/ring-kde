cmake_minimum_required(VERSION 3.0)

project(jamiaudioplayer)

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
    add_definitions(-DJAMIJAMIAUDIOPLAYER_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(LIB_X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamiaudioplayer_LIB_SRCS
    plugin.cpp
)

qt5_add_resources(jamiaudioplayer_LIB_SRCS
   jamiaudioplayer.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamiaudioplayer)

add_library(jamiaudioplayer ${jamiaudioplayer_LIB_SRCS} )

target_link_libraries( jamiaudioplayer
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
)

if(LIB_X11_FOUND)
    target_link_libraries( jamiaudioplayer
        Qt5::X11Extras
        ${X11_X11_LIB}
    )
endif()

# Configure the target config
set(jamiaudioplayer_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiAudioPlayerConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiAudioPlayer_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiAudioPlayerConfig.cmake.in" ${jamiaudioplayer_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAudioPlayer/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamiaudioplayer_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAudioPlayer/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamiaudioplayer
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiAudioPlayer>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamiaudioplayer PROPERTIES
    PUBLIC_HEADER
    "${jamiaudioplayer_EXPORT_HDRS}"
)

export(TARGETS jamiaudioplayer
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiAudioPlayerTargets.cmake"
)

install(TARGETS jamiaudioplayer
    EXPORT JamiAudioPlayerTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamiaudioplayer
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiAudioPlayerTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAudioPlayer
    COMPONENT Devel
)
