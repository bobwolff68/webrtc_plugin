#!/bin/sh

# Grabbing all the firebreath items
git submodule init
git submodule update
cd third_party/FireBreath
# Within firebreath, boost libraries must be init/updated
git submodule init
git submodule update
cd ../..

if [ `uname` = "Linux" ]
then
  sh ./prep_webrtc_linux.sh
fi

if [ `uname` = "Darwin" ]
then
  sh ./prep_webrtc_mac.sh
fi

