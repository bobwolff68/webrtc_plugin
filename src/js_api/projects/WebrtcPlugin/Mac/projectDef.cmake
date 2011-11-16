#/**********************************************************\ 
# Auto-generated Mac project definition file for the
# WebrtcPlugin project
#\**********************************************************/

# Mac template platform definition CMake file
# Included from ../CMakeLists.txt

# remember that the current source dir is the project root; this file is in Mac/
file (GLOB PLATFORM RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    Mac/[^.]*.cpp
    Mac/[^.]*.h
    Mac/[^.]*.cmake
    )

# use this to add preprocessor definitions
add_definitions(
    -DPOSIX
    -DOSX
    -DWEBRTC_MAC
)


SOURCE_GROUP(Mac FILES ${PLATFORM})

set (SOURCES
    ${SOURCES}
    ${PLATFORM}
    ../../../include/ThreadSingle.h
    WPLThreadSafeMessageQueue.h
    WPLThreadSafeMessageQueue.cpp
    )

set(PLIST "Mac/bundle_template/Info.plist")
set(STRINGS "Mac/bundle_template/InfoPlist.strings")
set(LOCALIZED "Mac/bundle_template/Localized.r")

add_mac_plugin(${PROJECT_NAME} ${PLIST} ${STRINGS} ${LOCALIZED} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!

find_library(LIBWEBRTC libwebrtc_Release.a ../../../../third_party/webrtc/trunk/xcodebuild/Release)
find_library(LIBSRTP libsrtp.a ../../../../third_party/webrtc/trunk/third_party/libsrtp)
find_library(FWCORESERVICES CoreServices)
find_library(FWCOREAUDIO CoreAudio)
find_library(FWCOREVIDEO CoreVideo)
find_library(FWQTKIT QTKit)
find_library(FWOPENGL OpenGL)
find_library(FWAUDIOTOOLBOX AudioToolbox)
find_library(FWAPPLICATIONSERVICES ApplicationServices)
find_library(FWFOUNDATION Foundation)
find_library(FWAPPKIT AppKit)

target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${LIBWEBRTC}
    ${LIBSRTP}
    ${FWCORESERVICES}
    ${FWCOREAUDIO}
    ${FWCOREVIDEO}
    ${FWQTKIT}
    ${FWOPENGL}
    ${FWAUDIOTOOLBOX}
    ${FWAPPLICATIONSERVICES}
    ${FWFOUNDATION}
    ${FWAPPKIT}
    -lpthread
    )
