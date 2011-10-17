
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingavideocapturedir = /usr/local/lib/ginga

libgingavideocapture_so_LDADD = -lpthread -lffmpegginga

libgingavideocapture_so_LDFLAGS = -Wl,-soname,libgingavideocapture.so \
    --shared -ldl -I/usr/local/include/ginga/

libgingavideocapture_PROGRAMS = libgingavideocapture.so
libgingavideocapture_so_SOURCES = VideoCapture.cpp
