#!/bin/sh

echo Resetting/Rebuilding project files...

# The linux build

if [ `uname` = "Linux" ]
then
  cd third_party/webrtc
  python trunk/build/gyp_chromium --depth=trunk ../../src/examples/cmdline_audio_peer/cmdline_audio_peer.gyp

  cd trunk
  if [ "$1" = "clean" ]
  then
    rm -Rf out/Debug
    rm -Rf out/Release
  fi

  make cmdline_audio_peer
  if [ $? != 0 ]
  then
	echo ; echo 'make (Debug) cmdline_audio_peer' failed. Exiting early.
	echo
	exit 1
  fi
  make BUILDTYPE=Release cmdline_audio_peer
  if [ $? != 0 ]
  then
	echo ; echo 'make (Release) cmdline_audio_peer' failed. Exiting early.
	echo
	exit 1
  fi

  cd ../../..
fi

# Now for Darwin's build.

if [ `uname` = "Darwin" ]
then

cd third_party/webrtc
python trunk/build/gyp_chromium --depth=trunk ../../src/examples/cmdline_audio_peer/cmdline_audio_peer.gyp -Dclang=1
cd ../..

echo "Rebuilding webrtc (via cmdline_audio_peer dependencies)"

cd src/examples/cmdline_audio_peer

# TODO - check returncode from xcodebuild and halt the process on unsuccessful batch items.
if [ "$1" = "clean" ]
then
  xcodebuild -project cmdline_audio_peer.xcodeproj -target cmdline_audio_peer -configuration Debug clean
  xcodebuild -project cmdline_audio_peer.xcodeproj -target cmdline_audio_peer -configuration Release clean
fi

# Now do the build for Debug and release.
xcodebuild -project cmdline_audio_peer.xcodeproj -target cmdline_audio_peer -configuration Debug
if [ $? != 0 ]
then
	echo ; echo 'xcodebuild (Debug) cmdline_audio_peer' failed. Exiting early.
	echo
	exit 1
fi
xcodebuild -project cmdline_audio_peer.xcodeproj -target cmdline_audio_peer -configuration Release
if [ $? != 0 ]
then
	echo ; echo 'xcodebuild (Release) cmdline_audio_peer' failed. Exiting early.
	echo
	exit 1
fi
cd ../../..

fi

# Now create the monolithic libraries
./create_libwebrtc.sh

if [ `uname` = "Linux" ]
then
  LOC="third_party/webrtc/trunk/out/"
else
  LOC="third_party/webrtc/trunk/xcodebuild/"
fi

# Final output - list them

echo
echo "Debug binary listing..."
echo
ls -lh ${LOC}Debug/cmdline_audio_peer
echo
echo "Release binary listing..."
echo
ls -lh ${LOC}Release/cmdline_audio_peer

echo
echo "Debug webrtc library listing..."
echo
ls -lh ${LOC}Debug/libwebrtc_Debug.a
echo
echo "Release webrtc library listing..."
echo
ls -lh ${LOC}Release/libwebrtc_Release.a

