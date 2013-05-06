#!/bin/bash

if [ `uname` = "Darwin" ]; then
    echo "Building (Webrtc+Libjingle) monolithic library..."
    cd deps/libjingle/trunk

    echo "build/gyp_chromium --depth=. third_party/webrtc/build/merge_libs.gyp"
    build/gyp_chromium --depth=. third_party/webrtc/build/merge_libs.gyp
    cd third_party/webrtc/build

    echo "xcodebuild -project merge_libs.xcodeproj -target merged_lib -configuration $1 -arch i386 -sdk macosx10.6 GCC_ENABLE_CPP_RTTI=YES"
    xcodebuild -project merge_libs.xcodeproj -target merged_lib -configuration $1 -arch i386 -sdk macosx10.6 GCC_ENABLE_CPP_RTTI=YES
    cd ../../../../../..
else
    if [ `uname` = "Linux" ]; then
        echo "Building (Webrtc+Libjingle) monolithic library..."
        cd deps/webrtc/trunk

        echo "build/gyp_chromium --depth=. src/build/merge_libs.gyp"
        build/gyp_chromium --depth=. src/build/merge_libs.gyp

        echo "make merged_lib BUILDTYPE=Debug"
	make merged_lib BUILDTYPE=Debug
        cd ../../..
    fi 
fi

