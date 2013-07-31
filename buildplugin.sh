#!/bin/bash

if [ `uname` = "Darwin" ]; then
    echo "Building the GoCastPlayer plugin project ($1 - firebreath 1.6)..."
    cd build
    xcodebuild -project Firebreath.xcodeproj -target GCP -arch i386 -configuration $1
    cd ..
else
    if [ `uname` = "Linux" ]; then
        echo "Building the GoCastPlayer plugin project (firebreath 1.6)..."
        cd build
        make GCP
        cd ..
    fi
fi

