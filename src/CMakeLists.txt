if(POLICY CMP0017)
   cmake_policy(SET CMP0017 NEW)
endif(POLICY CMP0017)

ADD_DEFINITIONS(
   ${KDE4_DEFINITIONS}
   ${QT_DEFINITIONS}
   -fexceptions
   -Wno-reorder
   -Wno-deprecated-declarations
   -DDATA_INSTALL_DIR="\\\"${DATA_INSTALL_DIR}\\\""
   -DSHARE_INSTALL_PREFIX="\\\"${SHARE_INSTALL_PREFIX}\\\""
)

ADD_DEFINITIONS("-std=c++0x")

add_subdirectory( lib  )
add_subdirectory( klib )

MESSAGE("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

IF(${CMAKE_BUILD_TYPE} MATCHES Release)
   MESSAGE("NO DEBUG OUTPUT")
   ADD_DEFINITIONS( -DQT_NO_DEBUG_OUTPUT)
ENDIF(${CMAKE_BUILD_TYPE} MATCHES Release)

SET ( KDE4_KABC_LIBS  -lkabc )

SET(
   sflphone_client_kde_SRCS
   main.cpp
   extendedaction.cpp
   sflphoneview.cpp
   sflphone.cpp
   sflphonecmd.cpp
   sflphoneapplication.cpp
   errormessage.cpp
   kspeechinterfacesingleton.cpp
   sflphoneaccessibility.cpp
   accountwizard.cpp
   widgets/sflphonetray.cpp
   widgets/calltreeitem.cpp
   widgets/historytreeitem.cpp
   actionsetaccountfirst.cpp
   conf/configurationdialog.cpp
   conf/dlggeneral.cpp
   conf/dlgdisplay.cpp
   conf/dlgaccounts.cpp
   conf/dlgaudio.cpp
   conf/dlgaddressbook.cpp
   conf/dlghooks.cpp
   conf/dlgaccessibility.cpp
   conf/dlgvideo.cpp
   widgets/dialpad.cpp
   widgets/contactitemwidget.cpp
   widgets/contactdock.cpp
   widgets/historydock.cpp
   widgets/bookmarkdock.cpp
   widgets/translucentbuttons.cpp
   widgets/categorydrawer.cpp
   widgets/conferencebox.cpp
   widgets/categorizedtreewidget.cpp
   widgets/videodock.cpp
   widgets/videowidget.cpp
   widgets/immanager.cpp
   widgets/imtab.cpp
   widgets/callviewoverlay.cpp
   widgets/callviewoverlaytoolbar.cpp
   widgets/tips/conftip.cpp
   widgets/tips/dialpadtip.cpp
   widgets/tips/riggingtip.cpp
   widgets/tips/connectionlosttip.cpp
   widgets/tips/tipcollection.cpp
   widgets/calltreeitemdelegate.cpp
#    widgets/acceleratedvideowidget.cpp
   callview.cpp
)


# generate rules for building source files from the resources
SET(QtApp_RCCS qrc/resources.qrc)
QT4_ADD_RESOURCES(QtApp_RCC_SRCS ${QtApp_RCCS})



# kde4_automoc(${sflphone_client_kde_SRCS})
SET(
   config_ui_files
   conf/dlggeneralbase.ui
   conf/dlgdisplaybase.ui
   conf/dlgaccountsbase.ui
   conf/dlgaudiobase.ui
   conf/dlgaddressbookbase.ui
   conf/dlghooksbase.ui
   conf/dlgaccessibility.ui
   conf/dlgvideobase.ui
)

# add_subdirectory( test   ) #Enable again some day, it cause compile problems for some users

IF(${ENABLE_VIDEO} MATCHES true)
   MESSAGE("VIDEO enabled")
   SET(ENABLE_VIDEO 1 CACHE BOOLEAN "Enable video")
   add_definitions( -DENABLE_VIDEO=true )
ENDIF(${ENABLE_VIDEO} MATCHES true)

KDE4_ADD_UI_FILES(sflphone_client_kde_SRCS ui/SFLPhoneView_base.ui  ${config_ui_files}  )
QT4_ADD_DBUS_INTERFACES(sflphone_client_kde_SRCS ${KDE4_DBUS_INTERFACES_DIR}/org.kde.KSpeech.xml)


KDE4_ADD_EXECUTABLE(sflphone-client-kde ${sflphone_client_kde_SRCS} ${QtApp_RCC_SRCS})

TARGET_LINK_LIBRARIES(sflphone-client-kde ksflphone qtsflphone  ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${KDE4_KIO_LIBS} ${KDEPIMLIBS_AKONADI_KMIME_LIBS} ${KDEPIMLIBS_AKONADI_LIBS} ${KDEPIMLIBS_AKONADI_CONTACT_LIBS} ${X11_LIBRARIES})

########### install files ###############

INSTALL(TARGETS sflphone-client-kde              DESTINATION  ${BIN_INSTALL_DIR}                           )
INSTALL( FILES icons/transferarraw.png           DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/transfertarrow.svg          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/confBlackWhite.svg          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/overlay_right_corner.svg    DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/overlay_left_corner.svg     DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/confBlackWhite.png          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/voicemail.png               DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/conf-small.png              DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/hangup_grayscale.svg        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/mutemic_grayscale.svg       DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/record_grayscale.svg        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/transfer_grayscale.svg      DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/hangup_grayscale.png        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/mutemic_grayscale.png       DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/record_grayscale.png        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/transfer_grayscale.png      DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/hold_grayscale.png          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/unhold_grayscale.png        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/refuse_grayscale.png        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/pickup_grayscale.png        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde      )
INSTALL( FILES icons/tips/keyboard.svg           DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/upArrow.svg            DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/downArrow.svg          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/dragAndDrop.svg        DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/rigging.svg            DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/reload.svg             DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/phoneDown.svg          DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/ring1.svg              DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/ring2.svg              DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
INSTALL( FILES icons/tips/ring3.svg              DESTINATION  ${DATA_INSTALL_DIR}/sflphone-client-kde/tips )
