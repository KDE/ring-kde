# Shared C++ QML components
ADD_SUBDIRECTORY( qmlwidgets )

#Build KDE specific files
ADD_SUBDIRECTORY( jamikdeintegration )

# Build the QML based first run Wizard
ADD_SUBDIRECTORY( wizard )

# Build the QML based interactive video canvas
ADD_SUBDIRECTORY( callview )

# Build the advanced phone dialer
ADD_SUBDIRECTORY( dialview )

# Build the views and widgets related to the social timeline feature
ADD_SUBDIRECTORY( timeline )

# Build the contact and profile photo editor dialog
ADD_SUBDIRECTORY( photoselector )

# Build the elements used to notify the users of events
ADD_SUBDIRECTORY( canvasindicators )

# Build the contact manager subsystem
ADD_SUBDIRECTORY( contactview )

# Build the account manager subsystem
ADD_SUBDIRECTORY( accountview )

# Build the main desktop component
ADD_SUBDIRECTORY( jamivideoview )

# Build the main desktop component
ADD_SUBDIRECTORY( jamitroubleshooting )

# Build the call and event history
ADD_SUBDIRECTORY( jamihistoryview )

# Build the chat widgets
ADD_SUBDIRECTORY( jamichatview )

# Build the shared timeline widgets
ADD_SUBDIRECTORY( jamitimelinebase )

# Build the audio player widget
ADD_SUBDIRECTORY( jamiaudioplayer )


if(ENABLE_NOTIFICATIONS)
    # Build the account manager subsystem
    ADD_SUBDIRECTORY( jaminotification )
endif()
