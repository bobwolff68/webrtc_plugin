#/**********************************************************\ 
# Auto-generated X11 project definition file for the
# GoCastPlayer project
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
    )

add_x11_plugin(${PROJECT_NAME} SOURCES)

# add library dependencies here; leave ${PLUGIN_INTERNAL_DEPS} there unless you know what you're doing!
find_library(LIBWEBRTC libwebrtc.a ../../../../deps/libjingle/trunk/out/${LIBWEBRTC_BUILD_CONFIG})
target_link_libraries(${PROJECT_NAME}
    ${PLUGIN_INTERNAL_DEPS}
    ${LIBWEBRTC}
    -lexpat
    -lX11
    -lXext
    -lssl
    -lcrypto
    -lXcomposite
    -lXrender
    -lrt
    )
