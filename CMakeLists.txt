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

SET(PROJECT_VERSION "2.91")
PROJECT(ring-kde)

SET(QT_MIN_VERSION  "5.7.0" )
SET(KF5_DEP_VERSION "5.6.0" )
SET(AKO_DEP_VERSION "4.89.0")

OPTION(ENABLE_SINGLE_INSTANCE  "Reuse instances of Ring-KDE when possible" ON)

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" )
LIST(APPEND CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

FIND_PACKAGE(ECM 1.1.0 REQUIRED NO_MODULE)
LIST(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

# Ring-LRC isn't a "stable" library. Clients often only work with a very specific
# commit and nothing else. To prevent using the wrong version, it is possible to
# add a `ring-lrc` directory in the source. If present, that version will be used.
IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ring-lrc/")
   SET(LibRingClient_SRC "${CMAKE_CURRENT_SOURCE_DIR}/ring-lrc/")
   SET(ringclient_FOUND true)
ELSE()
   FIND_PACKAGE ( LibRingClient QUIET CONFIG)
ENDIF()

# Download Ring-LRC if it wasn't found to simplify the build process
IF(NOT ringclient_FOUND)
   INCLUDE(cmake/FetchRing-LRC.cmake)

   SET(LibRingClient_SRC ${CMAKE_CURRENT_BINARY_DIR}/ring-lrc/)
ENDIF()

# Use the build-in copy if it has been found
IF(LibRingClient_SRC)
   # Use the .a, not the .so;
   SET(ENABLE_STATIC true)

   LIST(APPEND CMAKE_MODULE_PATH "${LibRingClient_SRC}/cmake/")

   ADD_SUBDIRECTORY(${LibRingClient_SRC})

   INCLUDE_DIRECTORIES(${LibRingClient_SRC}/src)
ENDIF()

INCLUDE(ECMInstallIcons)
INCLUDE(ECMOptionalAddSubdirectory)

INCLUDE(KDEInstallDirs)
INCLUDE(KDECMakeSettings)
INCLUDE(KDECompilerSettings)
INCLUDE(FeatureSummary)

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
  Init
  WindowSystem
  XmlGui
  Notifications
  IconThemes
  Crash
  NotifyConfig
  GlobalAccel
  Declarative
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
ADD_SUBDIRECTORY( lib  )
ADD_SUBDIRECTORY( data )
ADD_SUBDIRECTORY( src  )
ADD_SUBDIRECTORY( man  )

IF (KF5_DOCTOOLS_FOUND OR KF5_DOC_TOOLS_FOUND)
   ADD_SUBDIRECTORY( doc  )
ENDIF()

IF(${ENABLE_TEST} MATCHES true)
   ADD_SUBDIRECTORY( src/test)
ENDIF()

# Handling of translation files
KI18N_INSTALL( po )

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
