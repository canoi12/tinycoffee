#!/bin/bash

./build.sh release
./build.sh release windows
if [ -f "tico.zip" ]; then
  rm tico.zip
fi

strip bin/release/linux/tico
strip bin/release/windows/tico.exe

cp bin/release/* -r .


zip tico.zip linux/ windows/ -r
rm -r linux windows