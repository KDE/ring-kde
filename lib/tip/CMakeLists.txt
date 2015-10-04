CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ADD_DEFINITIONS("-std=c++11")

PROJECT(libtip)

SET(QT_MIN_VERSION  "5.2.0")

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES} ${Qt5Core_INCLUDES})

SET(libtip_SRCS
   tipmanager.cpp
   tip.cpp
   tipanimationwrapper.cpp
)

ADD_LIBRARY( tip STATIC ${libtip_SRCS} )

TARGET_LINK_LIBRARIES( tip
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Core_LIBRARIES}
)
