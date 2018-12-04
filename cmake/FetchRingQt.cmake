# In case dring and LibRingQt are not found, fetch them from git and curl to
# setup a minimal environment capable of building Ring-KDE. It also enable
# static libraries to isolate Ring-KDE from any potential LRC API break.
#
# Ring-GNOME also enable static libraries for the same reason. This module
# doesn't produce a "ready to use" binary. The daemon is still a separate
# process. Single binary Ring isn't officially supported on Linux and sending
# patches to keep it working upstream will only annoy its maintainer before
# being ignored.

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

find_package(Git REQUIRED)
include(ExternalProject)

# Fetch the XMLs
set(XML_PATH ${CMAKE_CURRENT_BINARY_DIR}/xml/)

if ((NOT ${ENABLE_LIBWRAP}) AND (NOT EXISTS ${XML_PATH}/cx.ring.Ring.CallManager.xml))

    message(STATUS "Downloading the GNU Ring API definition")

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.CallManager.xml
        ${XML_PATH}cx.ring.Ring.CallManager.xml INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.ConfigurationManager.xml
        ${XML_PATH}cx.ring.Ring.ConfigurationManager.xml INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.Instance.xml
        ${XML_PATH}cx.ring.Ring.Instance.xml INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.PresenceManager.xml
        ${XML_PATH}cx.ring.Ring.PresenceManager.xml INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.VideoManager.xml
        ${XML_PATH}cx.ring.Ring.VideoManager.xml INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/bin/dbus/cx.ring.Ring.VideoManager.xml
        ${XML_PATH}cx.ring.Ring.VideoManager.xml INACTIVITY_TIMEOUT 30)

    # Fetch the constants
    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/src/dring/account_const.h
        ${CMAKE_CURRENT_BINARY_DIR}/dring/account_const.h INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/src/dring/security_const.h
        ${CMAKE_CURRENT_BINARY_DIR}/dring/security_const.h INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/src/dring/media_const.h
        ${CMAKE_CURRENT_BINARY_DIR}/dring/media_const.h INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/src/dring/call_const.h
        ${CMAKE_CURRENT_BINARY_DIR}/dring/call_const.h INACTIVITY_TIMEOUT 30)

    file(DOWNLOAD https://raw.githubusercontent.com/savoirfairelinux/ring-daemon/master/src/dring/presence_const.h
        ${CMAKE_CURRENT_BINARY_DIR}/dring/presence_const.h INACTIVITY_TIMEOUT 30)
endif()

message(STATUS "Fetching LibRingQt from GitHub")

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libringqt)
    execute_process(COMMAND
        git clone https://github.com/Elv13/libringqt.git --branch master ${CMAKE_CURRENT_BINARY_DIR}/libringqt
    )
else()
    execute_process(COMMAND
        /bin/sh -c "cd ${CMAKE_CURRENT_BINARY_DIR}/libringqt && git fetch origin && git reset --hard origin/master"
    )
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libkquickitemviews)
    execute_process(COMMAND
        git clone  http://anongit.kde.org/kquickitemviews/--branch master ${CMAKE_CURRENT_BINARY_DIR}/libkquickitemviews
    )
endif()

# Build LibRingQt
SET(RING_XML_INTERFACES_DIR ${XML_PATH})

SET(ring_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/dring/)
