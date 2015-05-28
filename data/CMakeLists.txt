
ADD_SUBDIRECTORY(icons)

INSTALL( FILES ring-kde.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR}           )
INSTALL( FILES ring-kdeui.rc     DESTINATION  ${KXMLGUI_INSTALL_DIR}/ring-kde   )
INSTALL( FILES ring-kde.notifyrc DESTINATION  ${KXMLGUI_INSTALL_DIR}/ring-kde   )
INSTALL( FILES ring-kde.desktop  DESTINATION  ${DATA_INSTALL_DIR}/ring-kde      )