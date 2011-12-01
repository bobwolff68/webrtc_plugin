#/**********************************************************\ 
# 
# Auto-generated CMakeLists.txt for the WebrtcPlugin project
#
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

Project(${PLUGIN_NAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.cmake
    )

include_directories(${PLUGIN_INCLUDE_DIRS})
include_directories(../../../../third_party/webrtc/trunk/third_party/libjingle/source)
include_directories(../../../../third_party/webrtc/trunk/third_party_mods/libjingle/source)
include_directories(../../../include)
include_directories(../../../../third_party/webrtc/trunk/src)

add_definitions(-DWEBRTC_RELATIVE_PATH) 

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    WPLPeerConnectionClient.h
    WPLPeerConnectionClient.cpp
    WPLPeerConnectionObserver.h
    WPLPeerConnectionObserver.cpp
    WPLSocketServer.h
    WPLSocketServer.cpp
    WPLCall.h
    WPLCall.cpp
    ../../../include/rtc_common.h
    )

# This will include Win/projectDef.cmake, X11/projectDef.cmake, Mac/projectDef 
# depending on the platform
include_platform()
