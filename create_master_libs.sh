#!/bin/bash
#
# This script creates a few large .a files for simplifying linkage in projects which utilize webrtc.
# Note - webrtc has conflicts in .o filenames making a single monolithic .a file impossible at this
#        time. So, there is a part1 and a part2. Additionally, linkage of 2 others is required:
#        - libjingle.a
#        - libvpx.a
#

PLATFORM=`uname`

makearchive_Linux ()
{
rm -f libwebrtc_$1.a

  for (( i=0 ; i<${#thelibs[@]} ; i++ ))
  do
    echo Processing ${thelibs[$i]}...
    ar t obj${thelibs[$i]} | xargs ar qc libwebrtc_$1.a
  done

  ar sv libwebrtc_$1.a
}

thelibs=( .target/third_party_mods/libjingle/libjingle_app.a .target/third_party_mods/libjingle/libjsoncpp.a .target/src/modules/libvideo_capture_module.a .target/src/modules/libwebrtc_utility.a .target/src/modules/libaudio_coding_module.a .target/src/modules/libCNG.a .target/src/modules/libG711.a .target/src/modules/libG722.a .target/src/modules/libiSAC.a .target/src/modules/libPCM16B.a .target/src/modules/libNetEq.a .target/src/common_audio/libvad.a .target/src/system_wrappers/source/libsystem_wrappers.a .target/src/modules/libwebrtc_video_coding.a .target/src/modules/libwebrtc_i420.a .target/src/modules/libwebrtc_vp8.a .target/src/common_video/libwebrtc_vplib.a .target/src/modules/libvideo_render_module.a .target/src/video_engine/libvideo_engine_core.a .target/src/common_video/libwebrtc_jpeg.a .target/third_party/libjpeg_turbo/libjpeg_turbo.a .target/src/modules/libmedia_file.a .target/src/modules/librtp_rtcp.a .target/src/modules/libudp_transport.a .target/src/modules/libvideo_processing.a .target/src/modules/libaudio_conference_mixer.a .target/src/modules/libaudio_device.a .target/src/modules/libaudio_processing.a .target/src/modules/libaudioproc_debug_proto.a .target/src/modules/libaec.a .target/src/modules/libapm_util.a .target/src/modules/libaecm.a .target/src/modules/libagc.a .target/src/modules/libns.a .target/third_party_mods/libjingle/libjingle_p2p.a .target/src/voice_engine/libvoice_engine_core.a .target/third_party/protobuf/libprotobuf_lite.a .target/src/modules/libiLBC.a .target/src/common_audio/libresampler.a .target/src/common_audio/libspl.a .target/src/modules/libiSACFix.a .target/third_party_mods/libjingle/libjingle.a .target/third_party/libvpx/libvpx.a )

makearchive_Darwin ()
{
rm -f libwebrtc_part1_$1.a
rm -f libwebrtc_part2_$1.a

libtool -static -o libwebrtc_part1_$1.a libvad.a libwebrtc_vplib.a libwebrtc_jpeg.a libjingle_app.a libjingle_p2p.a libjsoncpp.a libjpeg_turbo.a libCNG.a libG711.a libG722.a libiSAC.a libPCM16B.a libaudio_coding_module.a libNetEq.a libaudio_conference_mixer.a libaudio_device.a libaudio_processing.a libaudioproc_debug_proto.a libaec.a libaecm.a libagc.a libns.a libapm_util.a libmedia_file.a libudp_transport.a libwebrtc_i420.a libwebrtc_utility.a libwebrtc_video_coding.a libwebrtc_vp8.a libvideo_capture_module.a libvideo_processing.a libvideo_render_module.a librtp_rtcp.a libsystem_wrappers.a libvideo_engine_core.a libexpat.a

libtool -static -o libwebrtc_part2_$1.a libvoice_engine_core.a libprotobuf_lite.a libiLBC.a libresampler.a libspl.a
}


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

makearchive_$PLATFORM Debug

echo ""
echo "Processing Release libraries next."
echo ""
cd ../Release

makearchive_$PLATFORM Release

