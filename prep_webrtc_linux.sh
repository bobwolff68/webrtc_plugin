#!/bin/sh
# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync -r 839 --force

echo Resetting/Rebuilding project files...
python trunk/build/gyp_chromium --depth=trunk ../../src/examples/cmdline_audio_peer/cmdline_audio_peer.gyp

cd trunk
make cmdline_audio_peer
# output to be found in third_party/webrtc/trunk/out/Debug/
