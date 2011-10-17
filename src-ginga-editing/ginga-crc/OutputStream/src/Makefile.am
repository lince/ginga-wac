
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingacaptureoutputstreamdir = /usr/local/lib/ginga

libgingacaptureoutputstream_so_LDADD = -lpthread

libgingacaptureoutputstream_so_LDFLAGS = -Wl,-soname,libgingacaptureoutputstream.so \
    --shared -ldl -I/usr/local/include/ginga/

libgingacaptureoutputstream_PROGRAMS = libgingacaptureoutputstream.so
libgingacaptureoutputstream_so_SOURCES = OutputStream.cpp
