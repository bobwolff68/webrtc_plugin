#!/bin/sh

if [ `uname` != "Darwin" ]
then
  echo Wrong platform. Expecting uname to return 'Darwin'
  exit
fi

# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync -r 888 --force

#echo Replacing standard files with our modified versions from third_party_mods
cp -R ../../third_party_mods/webrtc ../../third_party
# Any mods for FireBreath
cp -R ../../third_party_mods/FireBreath ../../third_party
# patch up the libsrtp by config'ing it in preparation for later build steps.
# Only necessary (still safe though) when NOT copying full-blown third_party_mods (like when up-revving)
cp -R ../../third_party_mods/webrtc/trunk/third_party/libsrtp ../../third_party/webrtc/trunk/third_party/
cd trunk/third_party/libsrtp
./configure CFLAGS="-m32 -arch i386" LDFLAGS="-m32 -arch i386"
make
# back up to third_party/webrtc
cd ../../..

# back up to root
cd ../..
./rebuild_webrtc.sh clean
./rebuild_plugin.sh clean

echo third_party/webrtc/trunk contains webrtc.xcodeproj for XCode.
echo NOTE: Be sure to MODERNIZE all projects.
echo This can be done by selecting target 'All (webrtc project)' and
echo "clicking on the warnings (command-4) then clicking on each warning"
echo and modernizing these projects.
# resultant binary in third_party/webrtc/trunk/xcodebuild/Debug/cmdline_audio_peer
