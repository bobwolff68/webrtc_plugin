#!/bin/sh

if [ `uname` != "Linux" ]
then
  echo Wrong platform. Expecting uname to return 'Linux'.
  exit
fi

# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync -r 839 --force

cd ../..
./rebuild_webrtc.sh clean

# output to be found in third_party/webrtc/trunk/out/Debug/
