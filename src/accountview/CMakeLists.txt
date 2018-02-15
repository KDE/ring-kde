CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

ADD_DEFINITIONS("-std=c++11")

PROJECT(accountview)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

SET(GENERIC_LIB_VERSION "1.0.0")

#File to compile
SET( accountview_LIB_SRCS
   accountviewplugin.cpp
)

QT5_ADD_RESOURCES(accountview_LIB_SRCS
    accountview.qrc
)

ADD_LIBRARY( accountview STATIC ${accountview_LIB_SRCS} )

target_link_libraries( accountview
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
    KF5::ConfigWidgets
)
