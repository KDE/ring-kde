cmake_minimum_required(VERSION 3.0)

project(jamisearch)

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
    add_definitions(-DJAMISEARCH_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamisearch_LIB_SRCS
    jamisearchplugin.cpp
    tipmodel.cpp
)

qt5_add_resources(jamisearch_LIB_SRCS
   jamisearch.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamisearch)

add_library(jamisearch ${jamisearch_LIB_SRCS} )

target_link_libraries( jamisearch
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
    KF5::ConfigCore
    KF5::ConfigGui
)

if(X11_FOUND)
    target_link_libraries( jamiaccountview
        Qt5::X11Extras
        ${X11_X11_LIB}
    )
endif()

# Configure the target config
set(jamisearch_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiSearchConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiSearch_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiSearchConfig.cmake.in" ${jamisearch_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiSearch/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamisearch_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiSearch/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamisearch
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiSearch>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamisearch PROPERTIES
    PUBLIC_HEADER
    "${jamisearch_EXPORT_HDRS}"
)

export(TARGETS jamisearch
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiSearchTargets.cmake"
)

install(TARGETS jamisearch
    EXPORT JamiSearchTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamisearch
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiSearchTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiSearch
    COMPONENT Devel
)
