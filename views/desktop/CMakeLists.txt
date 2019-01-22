cmake_minimum_required(VERSION 3.5)

add_definitions("-std=c++11")

project(desktopview)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(GENERIC_LIB_VERSION "1.0.0")

set(desktopview_LIB_SRCS
    desktopviewplugin.cpp
    dockmodel.cpp
    tipmodel.cpp
    qmlaction.cpp
    qactionbinder.cpp
)

qt5_add_resources(desktopview_LIB_SRCS
    desktopview.qrc
)

include_directories(SYSTEM ${ringqt_INCLUDE_DIR})

add_library( desktopview STATIC ${desktopview_LIB_SRCS} )

target_link_libraries( desktopview
    jamikdeintegration
    ringqt
    ringqtquick

    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::ConfigWidgets
)
