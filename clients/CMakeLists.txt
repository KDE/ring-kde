
# Mobile don't have command line option and single instance boilerplate, the
# mobile client has less code that can go wrong.
# if(OS_ANDROID)
    add_subdirectory(mobile)
# else()
#     add_subdirectory(desktop)
# endif()
