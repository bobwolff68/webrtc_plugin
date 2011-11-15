#!/bin/sh

echo Resetting/Rebuilding project files...

# The linux build
mkdir -p src/js_api/build
rm -f src/js_api/build/CMakeCache.txt

if [ `uname` = "Linux" ]
then
  third_party/FireBreath/prepmake.sh src/js_api/projects src/js_api/build
# TODO - make in release mode.

  cd src/js_api/build

  if [ "$1" = "clean" ]
  then
    make clean
  fi

  make WebrtcPlugin
  if [ $? != 0 ]
  then
	echo ; echo 'make (Debug) WebrtcPlugin' failed. Exiting early.
	echo
	exit 1
  fi
#  make BUILDTYPE=Release cmdline_audio_peer
#  if [ $? != 0 ]
#  then
#	echo ; echo 'make (Release) cmdline_audio_peer' failed. Exiting early.
#	echo
#	exit 1
#  fi

  cd ../../..

echo
echo Place symlink to src/js_api/build/bin/WebrtcPlugin/npWebrtcPlugin.so
echo output in ~/.mozilla/plugins
echo
fi

# Now for Darwin's build.

if [ `uname` = "Darwin" ]
then

echo "Rebuilding webrtc (via cmdline_audio_peer dependencies)"

third_party/FireBreath/prepmac_GoCast.sh src/js_api/projects src/js_api/build

cd src/js_api/build

if [ "$1" = "clean" ]
then
  xcodebuild -project FireBreath.xcodeproj -target WebrtcPlugin -configuration Debug clean
  xcodebuild -project FireBreath.xcodeproj -target WebrtcPlugin -configuration Release clean
fi

# Now do the build for Debug and release.
xcodebuild -project FireBreath.xcodeproj -target WebrtcPlugin -configuration Debug
if [ $? != 0 ]
then
	echo ; echo 'xcodebuild (Debug) WebrtcPlugin' failed. Exiting early.
	echo
	exit 1
fi
xcodebuild -project FireBreath.xcodeproj -target WebrtcPlugin -configuration Release
if [ $? != 0 ]
then
	echo ; echo 'xcodebuild (Release) WebrtcPlugin' failed. Exiting early.
	echo
	exit 1
fi
cd ../../..

echo
echo "Place symlink to src/js_api/build/[Debug|Release]/WebrtcPlugin.plugin"
echo output in ~/Library/Internet Plug-Ins/
echo

fi

