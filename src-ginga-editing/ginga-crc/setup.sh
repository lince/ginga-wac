#!/bin/bash

cd Exceptions
./autogen.sh
make
make install
cd ../OutputStream
./autogen.sh
make
make install
cd ../FFMpeg
./configure --enable-gpl --enable-fbgrab --enable-static --enable-postproc --enable-pthreads --enable-shared
# ./configure --enable-gpl --enable-fbgrab --enable-static --enable-postproc --enable-pthreads --enable-shared # Caso a versão acima não funciona, comentá-la e utilizar essa.
make
make install
cd ginga_ffmpeg
make
make install
cd ../../AudioCapture
./autogen.sh
make
make install
cd ../ImageCapture
./autogen.sh
make
make install
cd ../VideoCapture
./autogen.sh
make
make install
cd /usr/src/gingaNcl/gingalssm-cpp
./autogen.sh --enable-tuner --enable-tsparser --enable-dataprocessing --enable-imagecapture --enable-audiocapture --enable-videocapture
make
make install
cd ~

