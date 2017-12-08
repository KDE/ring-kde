CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

ADD_DEFINITIONS("-std=c++11")

PROJECT(contactview)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

SET(GENERIC_LIB_VERSION "1.0.0")

#File to compile
SET( contactview_LIB_SRCS
   contactviewplugin.cpp
   mainpage.cpp
   contactphoto.cpp
)

QT5_ADD_RESOURCES(contactview_LIB_SRCS
    contactview.qrc
)

ADD_LIBRARY( contactview STATIC ${contactview_LIB_SRCS} )

target_link_libraries( contactview
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickControls2
    KF5::ConfigWidgets
)
