#!/bin/sh

if [ `uname` != "Linux" ]
then
  echo Wrong platform. Expecting uname to return 'Linux'.
  exit
fi

./testdeps.sh
if [ $?!=0 ]
then
  echo
  echo Required dependencies not present. Exiting.
  echo
  exit 1
fi

# Must copy ONE modification for Linux. (Hopefully this doesn't grow.)
cp third_party_mods/webrtc/trunk/build/common.gypi third_party/webrtc/trunk/build

# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync -r 839 --force

cd ../..
./rebuild_webrtc.sh clean
./rebuild_plugin.sh clean
# output to be found in third_party/webrtc/trunk/out/Debug/
