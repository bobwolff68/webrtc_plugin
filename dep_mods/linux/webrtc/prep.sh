#### BUILD WEBRTC LIBS ####

makearchive ()
{
rm -f libwebrtc_$1.a
  for (( i=0 ; i<${#thelibs[@]} ; i++ ))
  do
    echo Processing ${thelibs[$i]}...
    ar t ${thelibs[$i]} | xargs ar qc libwebrtc_$1.a
  done

  echo Adding symbol table to archive.
  ar sv libwebrtc_$1.a
}

trunk/build/gyp_chromium --depth=trunk trunk/webrtc.gyp
cd trunk
if [[ $1 = "clean" ]]; then
    rm -rf out/Debug
    rm -rf out/Release
fi

make peerconnection_client
make peerconnection_client BUILDTYPE=Release

#### BUILD MONOLITHIC LIBRARY ####

thelibs=( obj.target/third_party_mods/libjingle/libjingle_app.a obj.target/third_party_mods/libjingle/libjsoncpp.a obj.target/src/modules/libvideo_capture_module.a obj.target/src/modules/libwebrtc_utility.a obj.target/src/modules/libaudio_coding_module.a obj.target/src/modules/libCNG.a obj.target/src/modules/libG711.a obj.target/src/modules/libG722.a obj.target/src/modules/libiSAC.a obj.target/src/modules/libPCM16B.a obj.target/src/modules/libNetEq.a obj.target/src/common_audio/libvad.a obj.target/src/system_wrappers/source/libsystem_wrappers.a obj.target/src/modules/libwebrtc_video_coding.a obj.target/src/modules/libwebrtc_i420.a obj.target/src/modules/libwebrtc_vp8.a obj.target/src/common_video/libwebrtc_vplib.a obj.target/src/modules/libvideo_render_module.a obj.target/src/video_engine/libvideo_engine_core.a obj.target/src/common_video/libwebrtc_jpeg.a obj.target/third_party/libjpeg_turbo/libjpeg_turbo.a obj.target/src/modules/libmedia_file.a obj.target/src/modules/librtp_rtcp.a obj.target/src/modules/libudp_transport.a obj.target/src/modules/libvideo_processing.a obj.target/src/modules/libaudio_conference_mixer.a obj.target/src/modules/libaudio_device.a obj.target/src/modules/libaudio_processing.a obj.target/src/modules/libaudioproc_debug_proto.a obj.target/src/modules/libaec.a obj.target/src/modules/libapm_util.a obj.target/src/modules/libaecm.a obj.target/src/modules/libagc.a obj.target/src/modules/libns.a obj.target/third_party_mods/libjingle/libjingle_p2p.a obj.target/src/voice_engine/libvoice_engine_core.a obj.target/third_party/protobuf/libprotobuf_lite.a obj.target/src/modules/libiLBC.a obj.target/src/common_audio/libresampler.a obj.target/src/common_audio/libsignal_processing.a obj.target/src/modules/libiSACFix.a obj.target/third_party_mods/libjingle/libjingle.a obj.target/third_party/libvpx/libvpx.a )

mkdir -p out/Debug
mkdir -p out/Release

cd out/Debug
makearchive Debug

cd ../Release
makearchive Release

cd ../../..

