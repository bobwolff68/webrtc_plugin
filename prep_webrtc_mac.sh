#!/bin/sh

if [ `uname` != "Darwin" ]
then
  echo Wrong platform. Expecting uname to return 'Darwin'
  exit
fi

./testdeps.sh
if [ $? != 0 ]
then
  echo
  echo Required dependencies not present. Exiting.
  echo
  exit 1
fi

# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync -r 1080 --force

#echo Replacing standard files with our modified versions from third_party_mods
cp -R ../../third_party_mods/webrtc ../../third_party
# Any mods for FireBreath
cp -R ../../third_party_mods/FireBreath ../../third_party
# Full blown _mods - not used yet. cp -R ../../third_party_mods/FireBreath ../../third_party
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
if [ $? != 0 ]
then
  echo
  echo *** BUILD FAILED ***
  echo
  exit 1
fi

./rebuild_plugin.sh clean
if [ $? != 0 ]
then
  echo
  echo *** BUILD FAILED ***
  echo
  exit 1
fi

echo
echo third_party/webrtc/trunk contains webrtc.xcodeproj for XCode.
echo
echo src/examples/cmdline_audio_peer/webrtc_projects.xcodeproj contains main project build.
echo
# resultant binary in third_party/webrtc/trunk/xcodebuild/Debug/webrtc_projects
