CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

ADD_DEFINITIONS("-std=c++11")

PROJECT(desktopview)

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

SET(GENERIC_LIB_VERSION "1.0.0")

SET(desktopview_LIB_SRCS
    ${desktopview_LIB_SRCS}
    desktopviewplugin.cpp
)

QT5_ADD_RESOURCES(desktopview_LIB_SRCS
   desktopview.qrc
)

ADD_LIBRARY( desktopview STATIC ${desktopview_LIB_SRCS} )

target_link_libraries( desktopview
   Qt5::Core
   Qt5::Gui
   Qt5::Quick
   Qt5::QuickControls2
)
