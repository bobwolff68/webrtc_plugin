project(cmdline_audio_peer)

-- CURRENTLY USING GYP_CHROMIUM TO BUILD PROJECT --


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
     TestPeerConnectionObserver.cpp
     TestPeerConnectionObserver.h
     TestDefaults.h
     TestDefaults.cpp
     TestSocketServer.h
     TestSocketServer.cpp
     ThreadSingle.h)

set(WEBRTCDIR
    ../../../third_party/webrtc/trunk)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    list(APPEND SOURCES
         ${WEBRTCDIR}/third_party/libjingle/source/talk/session/phone/devicemanager_mac.mm)
endif()

include_directories(${WEBRTCDIR}/third_party/libjingle/source)
include_directories(${WEBRTCDIR}/third_party_mods/libjingle/source)
include_directories(${WEBRTCDIR}/src)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DPOSIX)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_definitions(-DLINUX)
    add_definitions(-DWEBRTC_TARGET_PC)
    add_definitions(-DWEBRTC_LINUX)
    add_definitions(-DWEBRTC_THREAD_RR)
    add_definitions(-DWEBRTC_RELATIVE_PATH)
    add_definitions(-DNO_HEAPCHECKER)
    add_definitions(-DCHROMIUM_BUILD)
    add_definitions(-DUSE_NSS=1)
    add_definitions(-DENABLE_P2P_APIS=1)
    add_definitions(-DENABLE_CONFIGURATION_POLICY)
    add_definitions(-DENABLE_INPUT_SPEECH)
    add_definitions(-DUSE_SKIA)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_definitions(-DWEBRTC_TARGET_MAC_INTEL)
    add_definitions(-DWEBRTC_MAC_INTEL)
    add_definitions(-DWEBRTC_MAC)
    add_definitions(-DWEBRTC_THREAD_RR)
    add_definitions(-DWEBRTC_CLOCK_TYPE_REALTIME)
    add_definitions(-DOSX)
    add_definitions(-DUSE_COCOA_THREADING)

    find_library(LIBJINGLE_APP
                 libjingle_app.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBJSONCPP
                 libjsoncpp.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVIDEO_CAPTURE_MODULE
                 libvideo_capture_module.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_UTILITY
                 libwebrtc_utility.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAUDIO_CODING_MODULE
		 libaudio_coding_module.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBCNG
                 libCNG.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBSPL
                 libspl.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBG711
                 libG711.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBG722
                 libG722.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBILBC
                 libiLBC.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBISAC
                 libiSAC.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBISACFIX
                 libiSACFix.a
                 ${WEBRTCDIR}/xcodebuild/Debug)
    
    find_library(LIBPCM16B
                 libPCM16B.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBNETEQ
                 libNetEq.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBRESAMPLER
                 libresampler.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVAD
                 libvad.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBSYSTEM_WRAPPERS
                 libsystem_wrappers.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_VIDEO_CODING
                 libwebrtc_video_coding.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_I420
                 libwebrtc_i420.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_VP8
                 libwebrtc_vp8.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVPX
                 libvpx.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_VPLIB
                 libwebrtc_vplib.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVIDEO_RENDER_MODULE
                 libvideo_render_module.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVIDEO_ENGINE_CORE
                 libvideo_engine_core.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBWEBRTC_JPEG
                 libwebrtc_jpeg.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBJPEG_TURBO
                 libjpeg_turbo.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBMEDIA_FILE
                 libmedia_file.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBRTP_RTCP
                 librtp_rtcp.a
                 ${WEBRTCDIR}/xcodebuild/Debug)
   
    find_library(LIBSRTP
                 libsrtp.a
                 ${WEBRTCDIR}/third_party/libsrtp)
 
    find_library(LIBUDP_TRANSPORT
                 libudp_transport.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVIDEO_PROCESSING
                 libvideo_processing.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBVOICE_ENGINE_CORE
                 libvoice_engine_core.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAUDIO_CONFERENCE_MIXER
                 libaudio_conference_mixer.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAUDIO_DEVICE
                 libaudio_device.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAUDIO_PROCESSING
                 libaudio_processing.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAEC
                 libaec.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAPM_UTIL
                 libapm_util.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAECM
                 libaecm.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBAGC
                 libagc.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBNS
                 libns.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBPROTOBUF_LITE
                 libprotobuf_lite.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBJINGLE_P2P
                 libjingle_p2p.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBJINGLE
                 libjingle.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(LIBEXPAT
                 libexpat.a
                 ${WEBRTCDIR}/xcodebuild/Debug)

    find_library(FWCORESERVICES CoreServices)
    find_library(FWCOREAUDIO CoreAudio)
    find_library(FWCOREVIDEO CoreVideo)
    find_library(FWQTKIT QTKit)
    find_library(FWOPENGL OpenGL)
    find_library(FWAUDIOTOOLBOX AudioToolbox)
    find_library(FWAPPLICATIONSERVICES ApplicationServices)
    find_library(FWFOUNDATION Foundation)
    find_library(FWAPPKIT AppKit)

    set(COMPILE_FLAGS
        "-m32 -arch i386")

    set(CMAKE_CXX_FLAGS
        "-fno-rtti")

    set(CMAKE_EXE_LINKER_FLAGS
        "-m32 -arch i386 -fno-rtti")   
else()
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_library(LIBJINGLE_APP
                 libjingle_app.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party_mods/libjingle)

    find_library(LIBJSONCPP
                 libjsoncpp.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party_mods/libjingle)

    find_library(LIBVIDEO_CAPTURE_MODULE
                 libvideo_capture_module.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBWEBRTC_UTILITY
                 libwebrtc_utility.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAUDIO_CODING_MODULE
		 libaudio_coding_module.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBCNG
                 libCNG.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBSPL
                 libspl.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/common_audio)

    find_library(LIBG711
                 libG711.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBG722
                 libG722.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBILBC
                 libiLBC.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBISAC
                 libiSAC.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBISACFIX
                 libiSACFix.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)
    
    find_library(LIBPCM16B
                 libPCM16B.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBNETEQ
                 libNetEq.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBRESAMPLER
                 libresampler.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/common_audio)

    find_library(LIBVAD
                 libvad.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/common_audio)

    find_library(LIBSYSTEM_WRAPPERS
                 libsystem_wrappers.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/system_wrappers/source)

    find_library(LIBWEBRTC_VIDEO_CODING
                 libwebrtc_video_coding.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBWEBRTC_I420
                 libwebrtc_i420.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBWEBRTC_VP8
                 libwebrtc_vp8.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBVPX
                 libvpx.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party/libvpx)

    find_library(LIBWEBRTC_VPLIB
                 libwebrtc_vplib.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/common_video)

    find_library(LIBVIDEO_RENDER_MODULE
                 libvideo_render_module.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBVIDEO_ENGINE_CORE
                 libvideo_engine_core.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/video_engine)

    find_library(LIBWEBRTC_JPEG
                 libwebrtc_jpeg.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/common_video)

    find_library(LIBJPEG_TURBO
                 libjpeg_turbo.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party/libjpeg_turbo)

    find_library(LIBMEDIA_FILE
                 libmedia_file.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBRTP_RTCP
                 librtp_rtcp.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)
   
    find_library(LIBUDP_TRANSPORT
                 libudp_transport.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBVIDEO_PROCESSING
                 libvideo_processing.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBVOICE_ENGINE_CORE
                 libvoice_engine_core.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/voice_engine)

    find_library(LIBAUDIO_CONFERENCE_MIXER
                 libaudio_conference_mixer.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAUDIO_DEVICE
                 libaudio_device.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAUDIO_PROCESSING
                 libaudio_processing.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAEC
                 libaec.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAPM_UTIL
                 libapm_util.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAECM
                 libaecm.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBAGC
                 libagc.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBNS
                 libns.a
                 ${WEBRTCDIR}/out/Debug/obj.target/src/modules)

    find_library(LIBPROTOBUF_LITE
                 libprotobuf_lite.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party/protobuf)

    find_library(LIBJINGLE_P2P
                 libjingle_p2p.a
                 ${WEBRTCDIR}/out/Debug/obj.target/third_party_mods/libjingle)

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
                      ${LIBJINGLE_APP}
                      ${LIBJSONCPP}
                      ${LIBVIDEO_CAPTURE_MODULE}
                      ${LIBWEBRTC_UTILITY}
                      ${LIBAUDIO_CODING_MODULE}
                      ${LIBCNG}
                      ${LIBSPL}
                      ${LIBG711}
                      ${LIBG722}
                      ${LIBILBC}
                      ${LIBISAC}
                      ${LIBISACFIX}
                      ${LIBPCM16B}
                      ${LIBNETEQ}
                      ${LIBRESAMPLER}
                      ${LIBVAD}
                      ${LIBSYSTEM_WRAPPERS}
                      ${LIBWEBRTC_VIDEO_CODING}
                      ${LIBWEBRTC_I420}
                      ${LIBWEBRTC_VP8}
                      ${LIBVPX}
                      ${LIBWEBRTC_VPLIB}
                      ${LIBVIDEO_RENDER_MODULE}
                      ${LIBVIDEO_ENGINE_CORE}
                      ${LIBWEBRTC_JPEG}
                      ${LIBJPEG_TURBO}
                      ${LIBMEDIA_FILE}
                      ${LIBRTP_RTCP})

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(cmdline_audio_peer
                          ${LIBSRTP})
endif()

target_link_libraries(cmdline_audio_peer
                      ${LIBUDP_TRANSPORT}
                      ${LIBVIDEO_PROCESSING}
                      ${LIBVOICE_ENGINE_CORE}
                      ${LIBAUDIO_CONFERENCE_MIXER}
                      ${LIBAUDIO_DEVICE}
                      ${LIBAUDIO_PROCESSING}
                      ${LIBAEC}
                      ${LIBAPM_UTIL}
                      ${LIBAECM}
                      ${LIBAGC}
                      ${LIBNS}
                      ${LIBPROTOBUF_LITE}
                      ${LIBJINGLE_P2P}
                      ${LIBJINGLE})

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(cmdline_audio_peer
                          ${LIBEXPAT})
endif()

target_link_libraries(cmdline_audio_peer
                      -lpthread)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(cmdline_audio_peer
                          -lexpat
                          -lX11
                          -lasound
                          -ldl
                          -lrt
                          -lm)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    target_link_libraries(cmdline_audio_peer
                          ${FWCORESERVICES}
                          ${FWCOREAUDIO}
                          ${FWCOREVIDEO}
                          ${FWQTKIT}
                          ${FWOPENGL}
                          ${FWAUDIOTOOLBOX}
                          ${FWAPPLICATIONSERVICES}
                          ${FWFOUNDATION}
                          ${FWAPPKIT})
endif()
