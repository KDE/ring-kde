CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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

FIND_PACKAGE ( KDE4 REQUIRED )
FIND_PACKAGE ( Qt4  REQUIRED )

INCLUDE ( KDE4Defaults )

set(GENERIC_LIB_VERSION "1.2.3")

INCLUDE_DIRECTORIES ( ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

#File to compile
set( qtsflphone_LIB_SRCS
  call.cpp
  account.cpp
  accountlist.cpp
  callmodel.cpp
  historymodel.cpp
  contact.cpp
  abstractcontactbackend.cpp
  videocodecmodel.cpp
  videomodel.cpp
  videorenderer.cpp
  videodevice.cpp
  credentialmodel.cpp
  audiocodecmodel.cpp
  instantmessagingmodel.cpp
  dbus/configurationmanager.cpp
  dbus/callmanager.cpp
  dbus/instancemanager.cpp
  dbus/videomanager.cpp
  sflphone_const.h
  contactproxymodel.cpp
)

# Build dbus interfaces
SET ( dbus_xml_introspecs_path ${CMAKE_CURRENT_SOURCE_DIR}/dbus/)

# configuration manager interface
SET ( configurationmanager_xml  ${dbus_xml_introspecs_path}/configurationmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${configurationmanager_xml}
   PROPERTIES 
   CLASSNAME ConfigurationManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

QT4_ADD_DBUS_INTERFACE(
   qtsflphone_LIB_SRCS
   ${configurationmanager_xml}
   configurationmanager_dbus_interface)

# call manager interface
SET ( callmanager_xml  ${dbus_xml_introspecs_path}/callmanager-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${callmanager_xml}
   PROPERTIES 
   CLASSNAME CallManagerInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 

QT4_ADD_DBUS_INTERFACE(
   qtsflphone_LIB_SRCS
   ${callmanager_xml}
   callmanager_dbus_interface)

# video manager interface
SET ( video_xml  ${dbus_xml_introspecs_path}/video_controls-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${video_xml}
   PROPERTIES
   CLASSNAME VideoInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h")

QT4_ADD_DBUS_INTERFACE(
   qtsflphone_LIB_SRCS
   ${video_xml}
   video_dbus_interface)

# instance interface
SET ( instance_xml  ${dbus_xml_introspecs_path}/instance-introspec.xml )

SET_SOURCE_FILES_PROPERTIES(
   ${instance_xml}
   PROPERTIES 
   CLASSNAME InstanceInterface
   INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/dbus/metatypes.h") 
   
QT4_ADD_DBUS_INTERFACE(
   qtsflphone_LIB_SRCS
   ${instance_xml}
   instance_dbus_interface)
 
kde4_add_library( qtsflphone  SHARED ${qtsflphone_LIB_SRCS} )
 
target_link_libraries( qtsflphone
  -lrt
  -lpthread
  ${QT_QTDBUS_LIBRARY}
  ${QT_QTCORE_LIBRARY}
)

set_target_properties( qtsflphone
  PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION}
)

set( qtsflphone_LIB_HDRS
  account.h
  accountlist.h
  call.h
  callmodel.h
  historymodel.h
  contact.h
  abstractcontactbackend.h
  videocodecmodel.h
  videomodel.h
  videodevice.h
  videorenderer.h
  credentialmodel.h
  audiocodecmodel.h
  sflphone_const.h
  instantmessagingmodel.h
  contactproxymodel.h
)

install( FILES ${qtsflphone_LIB_HDRS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/qtsflphone
  COMPONENT Devel
)
 
install( TARGETS qtsflphone  ${INSTALL_TARGETS_DEFAULT_ARGS} )
