cmake_minimum_required(VERSION 3.0)

project(jamiphotoselector)

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
    add_definitions(-DJAMIPHOTOSELECTOR_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamiphotoselector_LIB_SRCS
    photoselector.cpp
    photoplugin.cpp
)

qt5_add_resources(jamiphotoselector_LIB_SRCS
   photoselector.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamiphotoselector)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

add_library(jamiphotoselector ${jamiphotoselector_LIB_SRCS} )

target_link_libraries( jamiphotoselector
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2

    # KF5
    KF5::I18n
)

if(X11_FOUND)
    target_link_libraries( jamiphotoselector
        Qt5::X11Extras
        ${X11_X11_LIB}
    )
endif()

# Configure the target config
set(jamiphotoselector_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiPhotoSelectorConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiPhotoSelector_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiPhotoSelectorConfig.cmake.in" ${jamiphotoselector_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiPhotoSelector/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamiphotoselector_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiPhotoSelector/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamiphotoselector
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiPhotoSelector>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamiphotoselector PROPERTIES
    PUBLIC_HEADER
    "${jamiphotoselector_EXPORT_HDRS}"
)

export(TARGETS jamiphotoselector
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiPhotoSelectorTargets.cmake"
)

install(TARGETS jamiphotoselector
    EXPORT JamiPhotoSelectorTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamiphotoselector
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiPhotoSelectorTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiPhotoSelector
    COMPONENT Devel
)
