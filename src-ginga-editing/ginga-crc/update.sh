#!/bin/bash

cd Exceptions
make
make install
cd ../OutputStream
make
make install
cd ../FFMpeg
make uninstall
make
make install
cd ginga_ffmpeg
make clean
make uninstall
make
make install
cd ../../AudioCapture
make
make install
cd ../ImageCapture
make
make install
cd ../VideoCapture
make
make install
cd /usr/src/gingaNcl/gingalssm-cpp
make
make install
cd ~

