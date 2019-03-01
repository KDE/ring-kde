CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(sharedassets)

IF(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
ENDIF(POLICY CMP0063)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

FIND_PACKAGE ( LibRingQt QUIET )

FIND_PACKAGE(Qt5 CONFIG REQUIRED
    Core
)

SET(GENERIC_LIB_VERSION "1.0.0")

QT5_ADD_RESOURCES(sharedassets_LIB_SRCS
   assets.qrc
)

ADD_LIBRARY( sharedassets  STATIC ${sharedassets_LIB_SRCS} )

target_link_libraries( sharedassets PRIVATE )

# https://gitlab.kitware.com/cmake/cmake/issues/17357
export(TARGETS sharedassets
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiSharedAssetsTargets.cmake"
)

install(TARGETS sharedassets
    EXPORT JamiSharedAssetsTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT sharedassets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiSharedAssetsTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiSharedAssets
    COMPONENT Devel
)
