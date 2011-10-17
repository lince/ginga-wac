
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingacaptureexceptionsdir = /usr/local/lib/ginga

libgingacaptureexceptions_so_LDADD = -lpthread

libgingacaptureexceptions_so_LDFLAGS = -Wl,-soname,libgingacaptureexceptions.so \
    --shared -ldl -I/usr/local/include/ginga/

libgingacaptureexceptions_PROGRAMS = libgingacaptureexceptions.so
libgingacaptureexceptions_so_SOURCES = Exceptions.cpp
