#!/bin/bash

#This script make CMake PO/MO generation work
cd po
if [ "$(grep -E "^ *add_subdirectory *\( *po *\)" ../CMakeLists.txt)" == "" ]; then 
   echo "Updating root CMakeLists.txt"
   sed 's/add_subdirectory( plasma )/add_subdirectory( plasma )\nadd_subdirectory( po     )/' -i ../CMakeLists.txt
fi

for DIR in $(find -type d); do 
   echo -e "file(GLOB _po_files *.po)\nGETTEXT_PROCESS_PO_FILES($( echo $DIR | cut -f2 -d'/') ALL INSTALL_DESTINATION \${LOCALE_INSTALL_DIR} \${_po_files} )" > $DIR/CMakeLists.txt 
done


cat  << EOF > CMakeLists.txt
find_package(Gettext REQUIRED)
if (NOT GETTEXT_MSGMERGE_EXECUTABLE)
MESSAGE(FATAL_ERROR "Please install msgmerge binary")
endif (NOT GETTEXT_MSGMERGE_EXECUTABLE)
if (NOT GETTEXT_MSGFMT_EXECUTABLE)
MESSAGE(FATAL_ERROR "Please install msgmerge binary")
endif (NOT GETTEXT_MSGFMT_EXECUTABLE)

EOF
for FILE in `ls`;do echo "add_subdirectory( $FILE )" | grep -v CMakeLists.txt >> CMakeLists.txt ;done 
