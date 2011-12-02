#!/bin/bash
#
# This script creates a monolithic (large!) static library out of the required static libraries
# which are created when building the webrtc project. This library does NOT contain any test
# libraries. It was derived from those items required to build cmdline_audio_peer which was
# derived in nature from peerconnection_client
#
# ASSUMPTION - webrtc libraries have already been built by prep*.sh script and rebuild*.sh.
#

PLATFORM=`uname`

makearchive ()
{
rm -f libwebrtc_$1.a

if [ $PLATFORM = "Linux" ]
then
  for (( i=0 ; i<${#thelibs[@]} ; i++ ))
  do
    echo Processing ${thelibs[$i]}...
    ar t ${thelibs[$i]} | xargs ar qc libwebrtc_$1.a
  done

  echo Adding symbol table to archive.
  ar sv libwebrtc_$1.a
else
    libtool -static -arch_only i386 -o libwebrtc_$1.a ${thelibs[@]:0}
fi
}

if [ $PLATFORM = "Darwin" ]
then
thelibs=( libjingle_app.a libjsoncpp.a libvideo_capture_module.a libwebrtc_utility.a libaudio_coding_module.a libCNG.a libG711.a libG722.a libiSAC.a libPCM16B.a libNetEq.a libvad.a libsystem_wrappers.a libwebrtc_video_coding.a libwebrtc_i420.a libwebrtc_vp8.a libwebrtc_vplib.a libvideo_render_module.a libvideo_engine_core.a libwebrtc_jpeg.a libjpeg_turbo.a libmedia_file.a librtp_rtcp.a libudp_transport.a libvideo_processing.a libaudio_conference_mixer.a libaudio_device.a libaudio_processing.a libaudioproc_debug_proto.a libaec.a libapm_util.a libaecm.a libagc.a libns.a libjingle_p2p.a libvoice_engine_core.a libprotobuf_lite.a libiLBC.a libresampler.a libiSACFix.a libjingle.a libvpx.a libexpat.a libsignal_processing.a )
else
thelibs=( obj.target/third_party_mods/libjingle/libjingle_app.a obj.target/third_party_mods/libjingle/libjsoncpp.a obj.target/src/modules/libvideo_capture_module.a obj.target/src/modules/libwebrtc_utility.a obj.target/src/modules/libaudio_coding_module.a obj.target/src/modules/libCNG.a obj.target/src/modules/libG711.a obj.target/src/modules/libG722.a obj.target/src/modules/libiSAC.a obj.target/src/modules/libPCM16B.a obj.target/src/modules/libNetEq.a obj.target/src/common_audio/libvad.a obj.target/src/system_wrappers/source/libsystem_wrappers.a obj.target/src/modules/libwebrtc_video_coding.a obj.target/src/modules/libwebrtc_i420.a obj.target/src/modules/libwebrtc_vp8.a obj.target/src/common_video/libwebrtc_vplib.a obj.target/src/modules/libvideo_render_module.a obj.target/src/video_engine/libvideo_engine_core.a obj.target/src/common_video/libwebrtc_jpeg.a obj.target/third_party/libjpeg_turbo/libjpeg_turbo.a obj.target/src/modules/libmedia_file.a obj.target/src/modules/librtp_rtcp.a obj.target/src/modules/libudp_transport.a obj.target/src/modules/libvideo_processing.a obj.target/src/modules/libaudio_conference_mixer.a obj.target/src/modules/libaudio_device.a obj.target/src/modules/libaudio_processing.a obj.target/src/modules/libaudioproc_debug_proto.a obj.target/src/modules/libaec.a obj.target/src/modules/libapm_util.a obj.target/src/modules/libaecm.a obj.target/src/modules/libagc.a obj.target/src/modules/libns.a obj.target/third_party_mods/libjingle/libjingle_p2p.a obj.target/src/voice_engine/libvoice_engine_core.a obj.target/third_party/protobuf/libprotobuf_lite.a obj.target/src/modules/libiLBC.a obj.target/src/common_audio/libresampler.a obj.target/src/common_audio/libsignal_processing.a obj.target/src/modules/libiSACFix.a obj.target/third_party_mods/libjingle/libjingle.a obj.target/third_party/libvpx/libvpx.a )
fi

if [ $PLATFORM = "Linux" ]
then
LOCROOT='third_party/webrtc/trunk/out/'
else
LOCROOT='third_party/webrtc/trunk/xcodebuild/'
fi

mkdir -p $LOCROOT/Debug
mkdir -p $LOCROOT/Release

echo ""
echo "Processing Debug libraries first."
echo ""
cd $LOCROOT/Debug

makearchive Debug

echo ""
echo "Processing Release libraries next."
echo ""
cd ../Release

makearchive Release

