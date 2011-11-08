#!/bin/sh
#
# This script creates a few large .a files for simplifying linkage in projects which utilize webrtc.
# Note - webrtc has conflicts in .o filenames making a single monolithic .a file impossible at this
#        time. So, there is a part1 and a part2. Additionally, linkage of 2 others is required:
#        - libjingle.a
#        - libvpx.a
#

TOOL=libtool

if test [`uname`==Linux]
  then TOOL='libtool --mode=link'
fi

mkdir -p third_party/webrtc/trunk/xcodebuild/Debug
mkdir -p third_party/webrtc/trunk/xcodebuild/Release

echo ""
echo "Processing Debug libraries first."
echo ""
cd third_party/webrtc/trunk/xcodebuild/Debug

$TOOL -static -o libwebrtc_part1_Debug.a libvad.a libwebrtc_vplib.a libwebrtc_jpeg.a libexpat.a libjingle_app.a libjingle_p2p.a libjsoncpp.a libjpeg_turbo.a libCNG.a libG711.a libG722.a libiSAC.a libPCM16B.a libaudio_coding_module.a libNetEq.a libaudio_conference_mixer.a libaudio_device.a libaudio_processing.a libaudioproc_debug_proto.a libaec.a libaecm.a libagc.a libns.a libapm_util.a libmedia_file.a libudp_transport.a libwebrtc_i420.a libwebrtc_utility.a libwebrtc_video_coding.a libwebrtc_vp8.a libvideo_capture_module.a libvideo_processing.a libvideo_render_module.a librtp_rtcp.a libsystem_wrappers.a libvideo_engine_core.a

$TOOL -static -o libwebrtc_part2_Debug.a libvoice_engine_core.a libprotobuf_lite.a libiLBC.a libresampler.a libspl.a

echo ""
echo "Processing Release libraries next."
echo ""
cd ../Release

$TOOL -static -o libwebrtc_part1_Release.a libvad.a libwebrtc_vplib.a libwebrtc_jpeg.a libexpat.a libjingle_app.a libjingle_p2p.a libjsoncpp.a libjpeg_turbo.a libCNG.a libG711.a libG722.a libiSAC.a libPCM16B.a libaudio_coding_module.a libNetEq.a libaudio_conference_mixer.a libaudio_device.a libaudio_processing.a libaudioproc_debug_proto.a libaec.a libaecm.a libagc.a libns.a libapm_util.a libmedia_file.a libudp_transport.a libwebrtc_i420.a libwebrtc_utility.a libwebrtc_video_coding.a libwebrtc_vp8.a libvideo_capture_module.a libvideo_processing.a libvideo_render_module.a librtp_rtcp.a libsystem_wrappers.a libvideo_engine_core.a

$TOOL -static -o libwebrtc_part2_Release.a libvoice_engine_core.a libprotobuf_lite.a libiLBC.a libresampler.a libspl.a

