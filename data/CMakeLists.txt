
add_subdirectory(icons)

INSTALL( FILES ring-kde.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
INSTALL( FILES ring-kdeui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ring-kde )
INSTALL( FILES ring-kde.notifyrc  DESTINATION ${DATA_INSTALL_DIR}/ring-kde)

