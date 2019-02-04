cmake_minimum_required(VERSION 3.0)

project(jaminotification)

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
    add_definitions(-DJAMINOTIFICATION_USE_STATIC_PLUGIN=1)
endif()

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick QuickControls2
)

if(X11_FOUND)
    find_package(Qt5 QUIET OPTIONAL_COMPONENTS Qt5::X11Extras)
endif()

set(GENERIC_LIB_VERSION "1.0.0")

#File to compile
set( jaminotification_LIB_SRCS
    plugin.cpp
    notification.cpp
    focuslistener.cpp
)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jaminotification)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

add_library(jaminotification ${jaminotification_LIB_SRCS} )

target_link_libraries( jaminotification
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2

    # KF5
    KF5::Notifications
    KF5::I18n
)

if(X11_FOUND)
    target_link_libraries( jaminotification
        Qt5::X11Extras
        ${X11_X11_LIB}
    )
endif()

# Configure the target config
set(jaminotification_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiNotificationConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiNotification_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiNotificationConfig.cmake.in" ${jaminotification_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiNotification/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jaminotification_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiNotification/
    COMPONENT Devel
)

# Create the target
target_include_directories(jaminotification
    PUBLIC
        $<INSTALL_INTERFACE:include/JamiNotification>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jaminotification PROPERTIES
    PUBLIC_HEADER
    "${jaminotification_EXPORT_HDRS}"
)

export(TARGETS jaminotification
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiNotificationTargets.cmake"
)

install(TARGETS jaminotification
    EXPORT JamiNotificationTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jaminotification
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiNotificationTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiNotification
    COMPONENT Devel
)
