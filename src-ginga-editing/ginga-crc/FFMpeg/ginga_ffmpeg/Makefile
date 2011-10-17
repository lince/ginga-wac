all: exceptions ffmpeg_ginga

exceptions: Exceptions.cpp
	g++ -c Exceptions.cpp -o exceptions.o

ffmpeg_ginga: ffmpeg_ginga.cpp
	g++ -fPIC -g -c -Wall -I../ -L"/home/lince/Desktop/CUR/FFMpeg"/libavcodec -L"/home/lince/Desktop/CUR/FFMpeg"/libavdevice -L"/home/lince/Desktop/CUR/FFMpeg"/libavfilter -L"/home/lince/Desktop/CUR/FFMpeg"/libavformat -L"/home/lince/Desktop/CUR/FFMpeg"/libavutil -L"/home/lince/Desktop/CUR/FFMpeg"/libpostproc -L"/home/lince/Desktop/CUR/FFMpeg"/libswscale -Wl,--warn-common -Wl,--as-needed -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libpostproc -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libswscale -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavfilter -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavdevice -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavformat -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavcodec -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavutil -Wl,-Bsymbolic -c ffmpeg_ginga.cpp ../cmdutils.o -lpostproc -lavdevice -lavformat -lavcodec -lavutil -lswscale -lz -pthread -lm  -lasound -ldl -lasound -lz -lasound exceptions.o
	g++ -shared -Wl,-soname,libffmpegginga.so -o libffmpegginga.so ffmpeg_ginga.o ../libavdevice/libavdevice.a ../libavcodec/libavcodec.a ../libavformat/libavformat.a ../libavutil/libavutil.a ../libswscale/libswscale.a ../cmdutils.o exceptions.o ../libpostproc/libpostproc.a -lpostproc -lavdevice -lavformat -lavcodec -lavutil -lswscale -lz -pthread -lm  -lasound -ldl -lasound -lz -lasound
	#OK-> g++ -I../ -L"/home/lince/Desktop/CUR/FFMpeg"/libavcodec -L"/home/lince/Desktop/CUR/FFMpeg"/libavdevice -L"/home/lince/Desktop/CUR/FFMpeg"/libavfilter -L"/home/lince/Desktop/CUR/FFMpeg"/libavformat -L"/home/lince/Desktop/CUR/FFMpeg"/libavutil -L"/home/lince/Desktop/CUR/FFMpeg"/libpostproc -L"/home/lince/Desktop/CUR/FFMpeg"/libswscale -Wl,--warn-common -Wl,--as-needed -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libpostproc -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libswscale -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavfilter -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavdevice -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavformat -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavcodec -Wl,-rpath-link,"/home/lince/Desktop/CUR/FFMpeg"/libavutil -Wl,-Bsymbolic -o ffmpeg_ginga ffmpeg_ginga.cpp ../cmdutils.o -lpostproc -lavdevice -lavformat -lavcodec -lavutil -lswscale -lz -lbz2 -pthread -lm  -lasound -ldl -lasound -lz -lasound

install:
	ldconfig -n .
	export LD_LIBRARY_PATH=$pwd:$LD_LIBRARY_PATH
	cp libffmpegginga.so /usr/local/lib
	cp ffmpeg_ginga.h Exceptions.h /usr/local/include
	cp ../libavcodec/audioconvert.h ../libavcodec/colorspace.h /usr/local/include/libavcodec	
	#cp ../libavcodec/audioconvert.h ../config.h ../libavcodec/colorspace.h ../libavutil/fifo.h ../libavformat/os_support.h ../cmdutils.h /usr/local/include
	cp ../libavutil/avstring.h ../libavutil/fifo.h /usr/local/include/libavutil
	cp ../libavformat/os_support.h /usr/local/include/libavformat
	cp ../config.h ../cmdutils.h /usr/local/include
	ln -s /usr/local/lib/libffmpegginga.so /usr/lib/libffmpegginga.so
	

clean::
	rm -f ffmpeg_ginga *.png *.o libffmpegginga.so *.avi
	

uninstall::
	rm -f /usr/local/include/ffmpeg_ginga.h
	rm -f /usr/local/lib/libffmpegginga.so
	rm -f /usr/lib/libffmpegginga.so
	rm -f /usr/local/include/libavcodec/audioconvert.h /usr/local/include/libavcodec/colorspace.h /usr/local/include/libavutil/fifo.h /usr/local/include/libavformat/os_support.h /usr/local/include/cmdutils.h /usr/local/include/config.h
	
	

