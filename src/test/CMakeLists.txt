cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0017)
   cmake_policy(SET CMP0017 NEW)
endif(POLICY CMP0017)

FIND_PACKAGE ( KDE4       REQUIRED            )
FIND_PACKAGE ( Qt4        REQUIRED QtCore QtGui QtXml QtDBus QtTest QtSVG QT_USE_QT* )

INCLUDE(   ${QT_USE_FILE}   )
include(KDE4Defaults)

ADD_DEFINITIONS("-std=c++0x")

message("Compiling tests")


SET(using_qtest 1)

#DBUS tests
set( dbus_SRCS dbus_test.cpp )

# kde4_add_unit_test( dbus
#    TESTNAME "DBus_connection"
#    ${dbus_SRCS}
# )

# target_link_libraries( dbus 
#    ${KDE4_KDECORE_LIBS}
#    ${QT_QTTEST_LIBRARY}
#    ${KDE4_KDEUI_LIBS}
#    ksflphone
#    qtsflphone
# )

#ACCOUNT tests
set( account_SRCS account_test.cpp )

# kde4_add_unit_test( account
#    TESTNAME "Account_serialization"
#    ${account_SRCS}
# )
# 
# target_link_libraries( account
#    ${KDE4_KDECORE_LIBS}
#    ${QT_QTTEST_LIBRARY}
#    ${KDE4_KDEUI_LIBS}
#    ksflphone
#    qtsflphone
# )

#Auto completion test
set( autocompletion_SRCS autocompletion_test.cpp )

kde4_add_unit_test( autocompletion
   TESTNAME "Auto completion"
   ${autocompletion_SRCS}
)

target_link_libraries( autocompletion
   ${KDE4_KDECORE_LIBS}
   ${QT_QTTEST_LIBRARY}
   ${KDE4_KDEUI_LIBS}
   ksflphone
   qtsflphone
)

#CALL tests
# set( call_SRCS call_test.cpp )
# 
# kde4_add_unit_test( call
#    TESTNAME "Basic_calls"
#    ${call_SRCS}
# )
# 
# 
# 
# target_link_libraries( call
#    ${KDE4_KDECORE_LIBS}
#    ${QT_QTTEST_LIBRARY}
#    ${KDE4_KDEUI_LIBS}
#    ksflphone
#    qtsflphone
# )
