
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclprefetchdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libginganclprefetch_so_LDFLAGS = -Wl,-soname,libginganclprefetch.so -shared \
    -L/usr/local/lib/ginga -L../gfx

libginganclprefetch_PROGRAMS = libginganclprefetch.so

libginganclprefetch_so_SOURCES = PrefetchManager.cpp

libginganclprefetch_so_LDADD = -ldl -ltelemidiautil -lginganclgfx
