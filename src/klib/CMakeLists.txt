CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

ADD_DEFINITIONS("-std=c++0x")

PROJECT(libkring)

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)
SET (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

INCLUDE(ECMInstallIcons)
INCLUDE(ECMOptionalAddSubdirectory)

INCLUDE(KDEInstallDirs)
INCLUDE(KDECMakeSettings)
INCLUDE(KDECompilerSettings)

FIND_PACKAGE ( LibRingClient REQUIRED )

FIND_PACKAGE(Qt5 CONFIG REQUIRED Core Widgets Gui)

FIND_PACKAGE(KF5 REQUIRED COMPONENTS
  Config
  GuiAddons
  I18n
  Init
  WindowSystem
  WidgetsAddons
  XmlGui
)

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES} ${LIB_RING_CLIENT_INCLUDE_DIR})

ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})

SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

SET(GENERIC_LIB_VERSION "1.4.1")

#File to compile
SET( libkring_LIB_SRCS
   helperfunctions.cpp
#    akonadibackend.cpp
   tipmanager.cpp
   tip.cpp
   tipanimationwrapper.cpp
   macromodel.cpp
   macro.cpp
#    akonadicontactcollectionmodel.cpp
   kdeprofilepersistor.cpp
   minimalhistorybackend.cpp
   itemmodelserialization.cpp
   bookmarkbackend.cpp
)

KCONFIG_ADD_KCFG_FILES (libkring_LIB_SRCS kcfg_settings.kcfgc)

ADD_LIBRARY( libkring  STATIC ${libkring_LIB_SRCS} )

target_link_libraries( libkring
  ${LIB_RING_CLIENT_LIBRARY}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${Qt5Gui_LIBRARIES}
  KF5::I18n
  KF5::WidgetsAddons
  KF5::ConfigCore
)

SET( libkring_LIB_HDRS
#   akonadibackend.h
  helperfunctions.h
  macromodel.h
  macro.h
  itemmodelserialization.h
)

INSTALL(FILES ring-kde.kcfg DESTINATION ${KCFG_INSTALL_DIR})

