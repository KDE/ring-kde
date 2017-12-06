CMAKE_MINIMUM_REQUIRED(VERSION 3.4)

ADD_DEFINITIONS("-std=c++11")

PROJECT(libphotoselector)

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
    Widgets
    Quick
    QuickWidgets
    QuickControls2
)

FIND_PACKAGE(KF5 REQUIRED COMPONENTS
    I18n
    Config
    ConfigWidgets
)

INCLUDE_DIRECTORIES(SYSTEM ${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES} ${ringclient_INCLUDE_DIR})

ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})

SET(GENERIC_LIB_VERSION "1.0.0")

#File to compile
SET( libphotoselector_LIB_SRCS
    photoselector.cpp
    photoplugin.cpp
)

QT5_ADD_RESOURCES(libphotoselector_LIB_SRCS
   photoselector.qrc
)

ADD_LIBRARY( libphotoselector  STATIC ${libphotoselector_LIB_SRCS} )

target_link_libraries( libphotoselector
    ringclient

    # KF5
    KF5::I18n

    # Qt5
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
    Qt5::QuickWidgets
    Qt5::QuickControls2
)
