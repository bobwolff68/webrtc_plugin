#!/bin/sh
# TODO - Need to check that gclient exists.
# TODO - Make sure wget is present
mkdir -p third_party/webrtc
cd third_party/webrtc

echo Getting webrtc from its repo into third_party/webrtc
gclient config http://webrtc.googlecode.com/svn/trunk
gclient sync --force

echo Resetting/Rebuilding project files...
gclient runhooks --force

make
