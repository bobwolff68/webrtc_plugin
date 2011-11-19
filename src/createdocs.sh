#!/bin/sh
if [ `uname` = "Darwin" ]
then
  /Applications/Doxygen.app/Contents/Resources/doxygen
else
  doxygen
fi

cd ../docs
mv html srcdocs
tar czf srcdocs.tgz srcdocs/
mv srcdocs html

