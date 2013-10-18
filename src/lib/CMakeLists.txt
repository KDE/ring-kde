CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_DEFINITIONS("-std=c++0x")
# ADD_DEFINITIONS("-std=c++0x")

ADD_DEFINITIONS(
	${QT_DEFINITIONS} 
	-fexceptions
)

PROJECT(qtsflphone)

#target_link_libraries(qtsflphone ${QT_QTCORE_LIBRARY} ${KDE4_KDEUI_LIBS})

SET(LOCAL_CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
SET(CMAKE_MODULE_PATH "${LOCAL_CMAKE_MODULE_PATH}")

IF(${ENABLE_VIDEO} MATCHES true)
   MESSAGE("VIDEO enabled")
   SET(ENABLE_VIDEO 1 CACHE BOOLEAN "Enable video")
   add_definitions( -DENABLE_VIDEO=true )
ENDIF(${ENABLE_VIDEO} MATCHES true)

IF(${ENABLE_QT5} MATCHES true)
   FIND_PACKAGE(Qt5Core)
   FIND_PACKAGE(Qt5DBus)
   ADD_DEFINITIONS(-DQT_DISABLE_DEPRECATED_BEFORE=0)
ELSE()
   FIND_PACKAGE ( Qt4  REQUIRED )
ENDIF(${ENABLE_QT5} MATCHES true)

set(GENERIC_LIB_VERSION "1.2.3")

include_directories(SYSTEM ${QT_INCLUDES} )
INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR})

# Build dbus interfaces
SET ( dbus_xml_introspecs_path ${CMAKE_CURRENT_SOURCE_DIR}/dbus/)

#File to compile
set( qtsflphone_LIB_SRCS
  #Data objects
  call.cpp
  account.cpp
  contact.cpp
  videorenderer.cpp
  videodevice.cpp
  phonenumber.cpp
  videocodec.cpp

  #Models
  accountlistmodel.cpp
  callmodel.cpp
  historymodel.cpp
  abstractcontactbackend.cpp
  abstractbookmarkmodel.cpp
  videocodecmodel.cpp
  videomodel.cpp
  credentialmodel.cpp
  audiocodecmodel.cpp
  instantmessagingmodel.cpp
  contactproxymodel.cpp
  useractionmodel.cpp
  presencestatusmodel.cpp
  phonedirectorymodel.cpp
  historytimecategorymodel.cpp
  numbercategorymodel.cpp
  keyexchangemodel.cpp
  tlsmethodmodel.cpp
  numbercompletionmodel.cpp
  categorizedaccountmodel.cpp

  #Communication
  dbus/configurationmanager.cpp
  dbus/callmanager.cpp
  dbus/instancemanager.cpp
  dbus/videomanager.cpp
  dbus/presencemanager.cpp

  #Visitors
  visitors/accountlistcolorvisitor.cpp
  visitors/phonenumberselector.cpp
  visitors/numbercategoryvisitor.cpp
  visitors/pixmapmanipulationvisitor.cpp

  #Other
  sflphone_const.h
  categorizedcompositenode.cpp
)

set( qtsflphone_LIB_HDRS
  account.h
  accountlistmodel.h
  call.h
  callmodel.h
  historymodel.h
  contact.h
  abstractcontactbackend.h
  abstractbookmarkmodel.h
  videocodecmodel.h
  videomodel.h
  videorenderer.h
  credentialmodel.h
  audiocodecmodel.h
  instantmessagingmodel.h
  contactproxymodel.h
  useractionmodel.h
  presencestatusmodel.h
  phonenumber.h
  phonedirectorymodel.h
  historytimecategorymodel.h
  numbercategorymodel.h
  videocodec.h
  keyexchangemodel.h
  tlsmethodmodel.h
  numbercompletionmodel.h
  categorizedaccountmodel.h
)

set( qtsflphone_extra_LIB_HDRS
  videodevice.h
  typedefs.h
  sflphone_const.h
  visitors/accountlistcolorvisitor.h
  visitors/phonenumberselector.h
)

# presence manager interface
SET ( presencemanager_xml  ${dbus_xml_introspecs_path}/presencemanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${presencemanager_xml}
   PROPERTIES 
   CLASSNAME PresenceManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h")

IF(${ENABLE_QT5} MATCHES true)
   QT5_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${presencemanager_xml}
      presencemanager_dbus_interface
   )
ELSE()
   QT4_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${presencemanager_xml}
      presencemanager_dbus_interface
   )

ENDIF(${ENABLE_QT5} MATCHES true)

# configuration manager interface
SET ( configurationmanager_xml  ${dbus_xml_introspecs_path}/configurationmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${configurationmanager_xml}
   PROPERTIES 
   CLASSNAME ConfigurationManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h")

IF(${ENABLE_QT5} MATCHES true)
   QT5_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${configurationmanager_xml}
      configurationmanager_dbus_interface
   )
ELSE()
   QT4_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${configurationmanager_xml}
      configurationmanager_dbus_interface
   )

ENDIF(${ENABLE_QT5} MATCHES true)

# call manager interface
SET ( callmanager_xml  ${dbus_xml_introspecs_path}/callmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${callmanager_xml}
   PROPERTIES 
   CLASSNAME CallManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

IF(${ENABLE_QT5} MATCHES true)
   QT5_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${callmanager_xml}
      callmanager_dbus_interface
   )
ELSE()
   QT4_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${callmanager_xml}
      callmanager_dbus_interface
   )
ENDIF(${ENABLE_QT5} MATCHES true)


# video manager interface
SET ( video_xml  ${dbus_xml_introspecs_path}/video_controls-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${video_xml}
   PROPERTIES
   CLASSNAME VideoInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h")

IF(${ENABLE_QT5} MATCHES true)
   QT5_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${video_xml}
      video_dbus_interface
   )
ELSE()
   QT4_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${video_xml}
      video_dbus_interface
   )
ENDIF(${ENABLE_QT5} MATCHES true)


# instance interface
SET ( instance_xml  ${dbus_xml_introspecs_path}/instance-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${instance_xml}
   PROPERTIES 
   CLASSNAME InstanceInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

IF(${ENABLE_QT5} MATCHES true)
   QT5_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${instance_xml}
      instance_dbus_interface
   )
ELSE()
   QT4_ADD_DBUS_INTERFACE(
      qtsflphone_LIB_SRCS
      ${instance_xml}
      instance_dbus_interface
   )
ENDIF(${ENABLE_QT5} MATCHES true)

# ADD_DEFINITIONS("-w")
IF(${ENABLE_QT5} MATCHES true)
   QT5_WRAP_CPP(LIB_HEADER_MOC ${qtsflphone_LIB_HDRS})
ELSE()
   QT4_WRAP_CPP(LIB_HEADER_MOC ${qtsflphone_LIB_HDRS})
ENDIF(${ENABLE_QT5} MATCHES true)
 
add_library( qtsflphone  SHARED ${qtsflphone_LIB_SRCS} ${LIB_HEADER_MOC} )
# REMOVE_DEFINITIONS("-w")

IF(${ENABLE_QT5} MATCHES true)
   QT5_USE_MODULES(qtsflphone Core DBus)
ENDIF(${ENABLE_QT5} MATCHES true)

target_link_libraries( qtsflphone
  -lrt
  -lpthread
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
)

set_target_properties( qtsflphone
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_VERSION}
)

SET(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
SET(INSTALL_TARGETS_DEFAULT_ARGS ${CMAKE_INSTALL_PREFIX}/lib)

install( FILES ${qtsflphone_LIB_HDRS} ${qtsflphone_extra_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/qtsflphone
  COMPONENT Devel
)
 
install( TARGETS qtsflphone
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS}
)
