cmake_minimum_required(VERSION 3.0)

project(jamiaccountview)

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
    add_definitions(-DJAMIACCOUNTVIEW_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jamiaccountview_LIB_SRCS
    accountviewplugin.cpp
)

qt5_add_resources(jamiaccountview_LIB_SRCS
   accountview.qrc
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamiaccountview)

add_library(jamiaccountview ${jamiaccountview_LIB_SRCS} )

target_link_libraries( jamiaccountview
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
set(jamiaccountview_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiAccountViewConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiAccountView_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiAccountViewConfig.cmake.in" ${jamiaccountview_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAccountView/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamiaccountview_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAccountView/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamiaccountview
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiAccountView>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamiaccountview PROPERTIES
    PUBLIC_HEADER
    "${jamiaccountview_EXPORT_HDRS}"
)

export(TARGETS jamiaccountview
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiAccountViewTargets.cmake"
)

install(TARGETS jamiaccountview
    EXPORT JamiAccountViewTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamiaccountview
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiAccountViewTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiAccountView
    COMPONENT Devel
)
