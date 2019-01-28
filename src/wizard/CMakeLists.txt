cmake_minimum_required(VERSION 3.0)

project(jamiwizard)

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
    add_definitions(-DJAMIWIZARD_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamiwizard_LIB_SRCS
    welcome.cpp
    policies.cpp
    plugin.cpp
)

qt5_add_resources(jamiwizard_LIB_SRCS
   wizard.qrc
)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamiwizard)

add_library(jamiwizard ${jamiwizard_LIB_SRCS} )

target_link_libraries( jamiwizard
    jamikdeintegration
    ringqt

    # Project
    ringqt
    ringqtquick

    # KF5
    KF5::ConfigCore
    KF5::ConfigGui

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
)

# Configure the target config
set(jamiwizard_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiWizardConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiWizard_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiWizardConfig.cmake.in" ${jamiwizard_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiWizard/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamiwizard_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiWizard/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamiwizard
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiWizard>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamiwizard PROPERTIES
    PUBLIC_HEADER
    "${jamiwizard_EXPORT_HDRS}"
)

export(TARGETS jamiwizard
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiWizardTargets.cmake"
)

install(TARGETS jamiwizard
    EXPORT JamiWizardTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamiwizard
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiWizardTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiWizard
    COMPONENT Devel
)
