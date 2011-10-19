project(cmdline_audio_peer)

cmake_minimum_required(VERSION 2.6)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    bin)

set(SOURCES
    main.cpp)

list(APPEND SOURCES
     TestPeerConnectionClient.h
     TestPeerConnectionClient.cpp
     TestDefaults.h
     TestDefaults.cpp
     TestSocketServer.h
     TestSocketServer.cpp
     ThreadSingle.h)

set(WEBRTCDIR
    ../../../third_party/webrtc/trunk)

include_directories(${WEBRTCDIR}/third_party/libjingle/source)
include_directories(${WEBRTCDIR}/third_party_mods/libjingle/source)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DPOSIX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DLINUX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    find_library(LIBJINGLE
                 libjingle.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(FWCORESERVICES CoreServices)

    set(COMPILE_FLAGS
        "-m32 -arch i386")

    set(CMAKE_CXX_FLAGS
        "-fno-rtti")

    set(CMAKE_EXE_LINKER_FLAGS
        "-m32 -arch i386 -fno-rtti")   
else()
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        find_library(LIBJINGLE
                     libjingle.a
                     ${WEBRTCDIR}/out/Debug/obj.target/third_party_mods/libjingle)

        set(CMAKE_CXX_FLAGS
            "-fno-rtti")

        set(CMAKE_EXE_LINKER_FLAGS
            "-fno-rtti")
    endif()
endif()

add_executable(cmdline_audio_peer
               ${SOURCES})

target_link_libraries(cmdline_audio_peer
                      ${LIBJINGLE}
                      -lpthread)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(cmdline_audio_peer
                          ${FWCORESERVICES})
endif()
