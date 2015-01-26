CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if(POLICY CMP0022)
   cmake_policy(SET CMP0022 NEW)
endif(POLICY CMP0022)

INCLUDE(GNUInstallDirs)

ADD_DEFINITIONS("-std=c++0x")

ADD_DEFINITIONS(
	${QT_DEFINITIONS} 
	-fexceptions
)

PROJECT(libkring)

SET ( KDE4_KABC_LIBS  -lkabc )


# add_subdirectory(dataengine)

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
SET(CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

FIND_PACKAGE ( KDE4 REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )
FIND_PACKAGE ( KdepimLibs REQUIRED )
FIND_PACKAGE ( LibRingClient REQUIRED )

INCLUDE ( KDE4Defaults )

set(GENERIC_LIB_VERSION "1.4.1")

include_directories(SYSTEM  ${QT_INCLUDES}  ${KDE4_INCLUDES} )
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${LIB_RING_CLIENT_INCLUDE_DIR})

#File to compile
set( libkring_LIB_SRCS
   helperfunctions.cpp
   akonadibackend.cpp
#    configurationskeleton.cpp
   tipmanager.cpp
   tip.cpp
   tipanimationwrapper.cpp
   macromodel.cpp
   macro.cpp
   akonadicontactcollectionmodel.cpp
   kdeprofilepersistor.cpp
   minimalhistorybackend.cpp
   itemmodelserialization.cpp
   bookmarkbackend.cpp
)

KDE4_ADD_KCFG_FILES(libkring_LIB_SRCS kcfg_settings.kcfgc)

kde4_add_library( libkring  STATIC ${libkring_LIB_SRCS} )

target_link_libraries( libkring
  ${LIB_RING_CLIENT_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${KDEPIMLIBS_AKONADI_KMIME_LIBS}
  ${KDEPIMLIBS_KABC_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
  ${KDEPIMLIBS_AKONADI_CONTACT_LIBS}
  ${KDE4_KDEUI_LIBS}
)

set_target_properties( libkring
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

set( libkring_LIB_HDRS
  akonadibackend.h
  helperfunctions.h
#   sortabledockcommon.h
  macromodel.h
  macro.h
  itemmodelserialization.h
)

INSTALL(FILES ring-kde.kcfg DESTINATION ${KCFG_INSTALL_DIR})

install( FILES ${libkring_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/libkring
  COMPONENT Devel
)

MESSAGE("INSTALLING IN!!!!! "  ${CMAKE_LIBRARY_ARCHITECTURE}   ${CMAKE_INSTALL_FULL_LIBDIR}   )
 
#This hack force Debian based system to return a non multi-arch path
#this is required to prevent the .deb libringclient.so from having an
#higher priority than the prefixed one.
STRING(REPLACE "${CMAKE_LIBRARY_ARCHITECTURE}" "" SANE_LIBRARY_PATH "${CMAKE_INSTALL_FULL_LIBDIR}" )
MESSAGE("INSTALLING IN!!!!! "  ${CMAKE_LIBRARY_ARCHITECTURE}   ${CMAKE_INSTALL_FULL_LIBDIR}   )

INSTALL( TARGETS libkring
  ARCHIVE DESTINATION ${SANE_LIBRARY_PATH}
  LIBRARY DESTINATION ${SANE_LIBRARY_PATH}
  DESTINATION ${SANE_LIBRARY_PATH}
)

