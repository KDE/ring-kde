CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

ADD_DEFINITIONS("-std=c++11")

PROJECT(libtimeline)

IF(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

IF(POLICY CMP0048)
   CMAKE_POLICY(SET CMP0048 NEW)
ENDIF(POLICY CMP0048)

IF(POLICY CMP0017)
   CMAKE_POLICY(SET CMP0017 NEW)
ENDIF(POLICY CMP0017)

IF(POLICY CMP0028)
   CMAKE_POLICY(SET CMP0028 NEW)
ENDIF(POLICY CMP0028)

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)

INCLUDE(ECMInstallIcons)
INCLUDE(ECMOptionalAddSubdirectory)

INCLUDE(KDEInstallDirs)
INCLUDE(KDECMakeSettings)
INCLUDE(KDECompilerSettings)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE ( LibRingClient QUIET )

FIND_PACKAGE(Qt5 CONFIG REQUIRED
    Core
    Gui
    Quick
    QuickWidgets
)

FIND_PACKAGE(KF5 REQUIRED COMPONENTS
  I18n
  Init
)

INCLUDE_DIRECTORIES(SYSTEM ${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES} ${ringclient_INCLUDE_DIR})

ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})

SET(GENERIC_LIB_VERSION "1.0.0")

#File to compile
SET( libtimeline_LIB_SRCS
   recentdock.cpp
   viewcontactdock.cpp
)

QT5_ADD_RESOURCES(libtimeline_LIB_SRCS
   recentdock.qrc
)

ADD_LIBRARY( libtimeline  STATIC ${libtimeline_LIB_SRCS} )

target_link_libraries( libtimeline
   ringclient

   Qt5::Widgets
   Qt5::Core
   Qt5::Gui
   Qt5::Quick
   Qt5::QuickWidgets
   Qt5::QuickControls2

   KF5::I18n
   KF5::WidgetsAddons
   KF5::ConfigCore
   KF5::ConfigGui
)

SET( libtimeline_LIB_HDRS
   recentdock.h
)
