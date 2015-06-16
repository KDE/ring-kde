cmake_minimum_required(VERSION 2.8)
ADD_DEFINITIONS("-std=c++11")

SET(QT_MIN_VERSION  "5.2.0")
SET(KF5_DEP_VERSION "5.6.0")

set(accountdialog_SRCS
   dlgaccount.cpp
   accountserializationadapter.cpp
 )

FIND_PACKAGE(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED
  Core
  Widgets
  Gui
)


FIND_PACKAGE(KF5 "${KF5_DEP_VERSION}" REQUIRED COMPONENTS
  KIO
  I18n
)

FIND_PACKAGE ( LibRingClient REQUIRED ENABLE_STATIC=true )

INCLUDE_DIRECTORIES(SYSTEM
   ${Qt5Widgets_INCLUDES}
   ${Qt5Core_INCLUDES}
   ${CMAKE_BINARY_DIR}
   ${LIB_RING_CLIENT_INCLUDE_DIR}
   ${CMAKE_SOURCE_DIR}
)

SET(CMAKE_AUTOMOC TRUE)

QT5_WRAP_UI(accountdialog_SRCS accountpages/advanced.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/credentials.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/basic.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/network.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/codecs.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/ringtone.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/dlgdht.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/security.ui)
QT5_WRAP_UI(accountdialog_SRCS accountpages/tabs.ui)
QT5_WRAP_UI(accountdialog_SRCS accountdlg.ui)

# Add pages
SET(accountdialog_SRCS ${accountdialog_SRCS}
   widgets/autocombobox.cpp
   widgets/groupedtoolbutton.cpp
   widgets/certificateviewer.cpp
   widgets/securityprogress.cpp

   accountpages/advanced.cpp
   accountpages/credentials.cpp
   accountpages/basic.cpp
   accountpages/network.cpp
   accountpages/codecs.cpp
   accountpages/ringtone.cpp
   accountpages/security.cpp
   accountpages/pagebase.cpp
   accountpages/account.cpp
   accountpages/dlgdht.cpp
)

ADD_LIBRARY(accountdialog STATIC ${accountdialog_SRCS})

TARGET_LINK_LIBRARIES(accountdialog
   ${Qt5Widgets_LIBRARIES}
   ${Qt5Core_LIBRARIES}
   ${Qt5Gui_LIBRARIES}
   KF5::KIOWidgets
   KF5::I18n
   ${LIB_RING_CLIENT_LIBRARY}
)

#SET(LIB_CONFIG_INCLUDE ${CMAKE_BINARY_DIR}/src/conf/account/ PARENT_SCOPE)
#SET(LIB_CONFIG_WIDGETS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

#INSTALL(TARGETS accountdialog ${INSTALL_TARGETS_DEFAULT_ARGS} )
