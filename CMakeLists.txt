CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

IF(POLICY CMP0053)
   CMAKE_POLICY(SET CMP0053 NEW)
ENDIF(POLICY CMP0053)

IF(POLICY CMP0048)
   CMAKE_POLICY(SET CMP0048 NEW)
ENDIF(POLICY CMP0048)

IF(POLICY CMP0017)
   CMAKE_POLICY(SET CMP0017 NEW)
ENDIF(POLICY CMP0017)

IF(POLICY CMP0028)
   CMAKE_POLICY(SET CMP0028 NEW)
ENDIF(POLICY CMP0028)

SET(PROJECT_VERSION "3.1.0")
PROJECT(ring-kde)

SET(QT_MIN_VERSION  "5.9.0" )
SET(KF5_DEP_VERSION "5.6.0" )
SET(AKO_DEP_VERSION "4.89.0")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    SET(OS_ANDROID 1)
ENDIF()

OPTION(ENABLE_SINGLE_INSTANCE  "Reuse instances of Ring-KDE when possible" ON)

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" )
LIST(APPEND CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)
LIST(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

# libringqt isn't a "stable" library. Clients often only work with a very specific
# commit and nothing else. To prevent using the wrong version, it is possible to
# add a `libringqt` directory in the source. If present, that version will be used.
IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libringqt/")
   SET(LibRingQt_SRC "${CMAKE_CURRENT_SOURCE_DIR}/libringqt/")
   SET(ringqt_FOUND true)
ELSE()
   FIND_PACKAGE ( LibRingQt QUIET CONFIG)

   IF (ringqt_FOUND)
      FIND_PACKAGE ( LibRingQtQuick REQUIRED CONFIG)
   ENDIF()
ENDIF()

FIND_PACKAGE (KQuickItemViews QUIET CONFIG)

# Download libringqt if it wasn't found to simplify the build process
IF(NOT ringqt_FOUND)
   INCLUDE(cmake/FetchRingQt.cmake)

   SET(LibRingQt_SRC ${CMAKE_CURRENT_BINARY_DIR}/libringqt/)
ENDIF()

# This package was developed for Ring-KDE. It is now a shared framework. Until
# it is more widespread and stable, a copy is still included in Ring-KDE.
IF(NOT KQuickItemViews_FOUND AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libkquickitemviews/")
   SET(LibKQuickItemViews_SRC "${CMAKE_CURRENT_SOURCE_DIR}/libkquickitemviews/")
   SET(libkquickitemviews_FOUND true)
   ADD_DEFINITIONS(-DKQUICKITEMVIEWS_USE_STATIC_PLUGIN=1)
ELSEIF(NOT KQuickItemViews_FOUND AND IS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libkquickitemviews/")
   SET(LibKQuickItemViews_SRC "${CMAKE_CURRENT_BINARY_DIR}/libkquickitemviews/")
   SET(libkquickitemviews_FOUND true)
   ADD_DEFINITIONS(-DKQUICKITEMVIEWS_USE_STATIC_PLUGIN=1)
ELSEIF(NOT KQuickItemViews_FOUND)
    message(FATAL_ERROR "Download KQuickItemViews from `git clone https://anongit.kde.org/kquickitemviews/` and move it to `path/to/ring-kde/libkquickitemviews`")
ENDIF()

# Use the build-in copy if it has been found
IF(LibRingQt_SRC)
   # Use the .a, not the .so;
   SET(ENABLE_STATIC true)

   LIST(APPEND CMAKE_MODULE_PATH "${LibRingQt_SRC}/cmake/")

   ADD_SUBDIRECTORY(${LibRingQt_SRC} build)

   INCLUDE_DIRECTORIES(${LibRingQt_SRC}/src)
ENDIF()

IF(LibKQuickItemViews_SRC)
   # Use the .a, not the .so;
   SET(ENABLE_STATIC true)

   ADD_SUBDIRECTORY(${LibKQuickItemViews_SRC} build)
ENDIF()


INCLUDE(ECMInstallIcons)
INCLUDE(ECMOptionalAddSubdirectory)

INCLUDE(KDEInstallDirs)
INCLUDE(KDECMakeSettings)
INCLUDE(FeatureSummary)

# On Android, the executable is treated as a library, this is not what's
# normally expected.
IF(NOT DEFINED OS_ANDROID)
  INCLUDE(KDECompilerSettings)
ENDIF()

# So far only Linux systems use this feature, so it will print noise on all
# platforms where the necessary infrastructure doesn't even exists, let alone
# in use.
IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   SET(KDE_SKIP_TEST_SETTINGS 1)
ENDIF()

# Block known broken C++11 compilers
IF (CMAKE_COMPILER_IS_GNUCC)
   EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   IF (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
      MESSAGE(STATUS "Found GCC version >= 4.8: " ${GCC_VERSION})
   ELSE()
      MESSAGE(FATAL_ERROR "Your version of GCC is too old, please install GCC 4.8 or later")
   ENDIF()
ENDIF()

SET(QT_USE_QT*)


FIND_PACKAGE(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED
  Core
  Widgets
  Gui
  Svg
  PrintSupport
  QuickControls2
  Quick
)

#FIND_PACKAGE(Qt5TextToSpeech ${QT_MIN_VERSION} QUIET)

#SET_PACKAGE_PROPERTIES(Qt5TextToSpeech PROPERTIES
#   PURPOSE "Speech support"
#)

IF (Qt5TextToSpeech_FOUND)
   ADD_DEFINITIONS(-DHAVE_SPEECH)
ENDIF()

FIND_PACKAGE(KF5 "${KF5_DEP_VERSION}" REQUIRED COMPONENTS
  Config
  GuiAddons
  DBusAddons
  I18n
  WindowSystem
  XmlGui
  Notifications
  IconThemes
  Crash
  NotifyConfig
  GlobalAccel
  Declarative
  Kirigami2
)

FIND_PACKAGE(KF5 "${AKO_DEP_VERSION}" QUIET COMPONENTS
  DocTools
  Akonadi
  AkonadiContact
  Contacts
)

IF ( KF5_AKONADI_FOUND AND  KF5_AKONADICONTACT_FOUND AND  KF5_CONTACTS_FOUND)
   ADD_DEFINITIONS("-DENABLE_AKONADI=1")
ENDIF()

INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake  )
# INCLUDE( ${QT_USE_FILE}   )
ADD_SUBDIRECTORY( data )
ADD_SUBDIRECTORY( src  )
ADD_SUBDIRECTORY( man  )

IF (KF5_DOCTOOLS_FOUND OR KF5_DOC_TOOLS_FOUND)
   ADD_SUBDIRECTORY( doc  )
ENDIF()

IF(${ENABLE_TEST} MATCHES true)
   ADD_SUBDIRECTORY( src/test)
ENDIF()

# macOS
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   SET(MACOSX_BUNDLE ON)
   SET(MACOSX_BUNDLE_BUNDLE_NAME "Ring-KDE")
   SET(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
   SET(CMAKE_MACOSX_RPATH ON)
   SET(CMAKE_SKIP_BUILD_RPATH FALSE)
   SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
   SET(CMAKE_INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}")
   SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

SET(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})

ADD_CUSTOM_TARGET(dist
      COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
      | gzip > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.gz
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

FEATURE_SUMMARY(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
