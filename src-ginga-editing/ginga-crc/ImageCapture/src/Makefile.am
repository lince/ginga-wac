
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingaimagecapturedir = /usr/local/lib/ginga

libgingaimagecapture_so_LDADD = -lpthread -lffmpegginga

libgingaimagecapture_so_LDFLAGS = -Wl,-soname,libgingaimagecapture.so \
    --shared -ldl -I/usr/local/include/ginga/

libgingaimagecapture_PROGRAMS = libgingaimagecapture.so
libgingaimagecapture_so_SOURCES = ImageCapture.cpp
