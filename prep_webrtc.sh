#!/bin/sh

if [ `uname` = "Linux" ]
then
  sh ./prep_webrtc_linux.sh
fi

if [ `uname` = "Darwin" ]
then
  sh ./prep_webrtc_mac.sh
fi

