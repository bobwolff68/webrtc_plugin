#!/bin/bash

#############
# libjingle #
#############

LIBJINGLE_REV=284
mkdir -p deps
cd deps

if [[ $1 = "libjingle" || $* = "" ]]; then 
    echo "Checking for directory [libjingle]..."
    mkdir -p libjingle
    cd libjingle

    if [ ! -f .gclient ]; then
        echo "Running gclient config with target url [http://libjingle.googlecode.com/svn/trunk/]"
        gclient config http://libjingle.googlecode.com/svn/trunk/
    fi

    echo "Running [gclient sync -r $LIBJINGLE_REV --force] to obtain webrtc source..."
    gclient sync -r "$LIBJINGLE_REV" --force
    cd ..

    echo "Patching webrtc source..."
    patch -p0 -i ../dep_mods/common/webrtc.diff

    echo "Patching libjingle source..."
    patch -p0 -i ../dep_mods/common/libjingle.diff

    echo "Patching libjingle build..."
    patch -p0 -i ../dep_mods/common/libjingle_build.diff
fi

##############
# firebreath #
##############

if [[ $1 = "firebreath" || $* = "" ]]; then
    if [ ! -d firebreath ]; then
        echo "Cloning firebreath branch 1.6 from [git://github.com/firebreath/FireBreath.git]"
        git clone git://github.com/firebreath/FireBreath.git -b firebreath-1.6 firebreath
    fi
fi

cd ..

