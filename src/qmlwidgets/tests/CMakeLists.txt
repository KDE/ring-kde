CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
ADD_DEFINITIONS("-std=c++11")

PROJECT(modelviewtester)

SET(QT_MIN_VERSION  "5.7.0")
set(CMAKE_BUILD_TYPE DEBUG)

IF(POLICY CMP0063)
    CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickWidgets
    Widgets
    QuickControls2
)

INCLUDE_DIRECTORIES(SYSTEM ${Qt5Core_INCLUDES})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/qmlwidgets/)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

SET(modelviewtester_SRC
modelviewtester.cpp
testmain.cpp
qmlwidgets/flickableview.cpp
qmlwidgets/hierarchyview.cpp
qmlwidgets/qmlaction.cpp
qmlwidgets/quicklistview.cpp
qmlwidgets/quicktreeview.cpp
qmlwidgets/simpleflickable.cpp
qmlwidgets/treeview2.cpp
)

QT5_ADD_RESOURCES(modelviewtester_SRC
    modeltest.qrc
)

set(CMAKE_AUTOMOC ON)

ADD_EXECUTABLE( modelviewtester ${modelviewtester_SRC} )

TARGET_LINK_LIBRARIES( modelviewtester
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Quick
)
