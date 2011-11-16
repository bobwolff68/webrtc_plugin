#/**********************************************************\ 
# Auto-generated X11 project definition file for the
# WebrtcPlugin project
#\**********************************************************/

# X11 template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in X11/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    X11/[^.]*.cpp
    X11/[^.]*.h
    X11/[^.]*.cmake
    )

SOURCE_GROUP(X11 FILES ${PLATFORM})

# use this to add preprocessor definitions
add_definitions(
    -DPOSIX
    -DWEBRTC_LINUX
)

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    ../../../include/ThreadSingle.h
    WPLThreadSafeMessageQueue.h
    WPLThreadSafeMessageQueue.cpp
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

find_library(LIBWEBRTC libwebrtc_Release.a ../../../../third_party/webrtc/trunk/out/Release)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${LIBWEBRTC}
    -lexpat
    -lX11
    -lXext
    -lGL
    -ldl
    -lrt
    -lpthread
    )
