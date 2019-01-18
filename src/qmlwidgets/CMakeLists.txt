CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

ADD_DEFINITIONS("-std=c++11")

PROJECT(libqmlwidgets)

IF(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)

INCLUDE(ECMInstallIcons)
INCLUDE(ECMOptionalAddSubdirectory)

INCLUDE(KDEInstallDirs)
INCLUDE(KDECMakeSettings)
INCLUDE(KDECompilerSettings)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE ( LibRingQt QUIET )

FIND_PACKAGE(Qt5 CONFIG REQUIRED
    Core
    Gui

    Quick
    QuickWidgets
    QuickControls2
)

FIND_PACKAGE(KF5 REQUIRED COMPONENTS
    I18n
    Config
    ConfigWidgets
)

INCLUDE_DIRECTORIES(SYSTEM ${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES} ${ringqt_INCLUDE_DIR})

ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})

SET(GENERIC_LIB_VERSION "1.0.0")

#File to compile
SET( libqmlwidgets_LIB_SRCS
    bubble.cpp
    plugin.cpp
    messagebuilder.cpp
    contactbuilder.cpp
    treehelper.cpp
    multicall.cpp
    snapshotadapter.cpp
    timelinedots.cpp
    qmlaction.cpp
    qactionbinder.cpp
    symboliccolorizer.cpp
)

QT5_ADD_RESOURCES(libqmlwidgets_LIB_SRCS
   qmlwidgets.qrc
)

ADD_LIBRARY( libqmlwidgets  STATIC ${libqmlwidgets_LIB_SRCS} )

target_link_libraries( libqmlwidgets
    ringqt

    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::QuickControls2

    KF5::ConfigCore
    KF5::ConfigGui
    KF5::ConfigWidgets
    KF5::XmlGui
)

SET( libqmlwidgets_LIB_HDRS
    bubble.h
    plugin.h
    messagebuilder.h
)
