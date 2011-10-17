
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingaaudiocapturedir = /usr/local/lib/ginga

libgingaaudiocapture_so_LDADD = -lpthread -lffmpegginga

libgingaaudiocapture_so_LDFLAGS = -Wl,-soname,libgingaaudiocapture.so \
    --shared -ldl -I/usr/local/include/ginga/

libgingaaudiocapture_PROGRAMS = libgingaaudiocapture.so
libgingaaudiocapture_so_SOURCES = AudioCapture.cpp
