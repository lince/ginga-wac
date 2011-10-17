
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclgfxdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libginganclgfx_so_LDFLAGS = -L/usr/local/lib/  -L/usr/local/lib/ginga \
    -Wl,-soname,libginganclgfx.so -shared

libginganclgfx_PROGRAMS = libginganclgfx.so

libginganclgfx_so_SOURCES = GingaNclGfx.cpp

libginganclgfx_so_LDADD = -ltelemidiautil -ldl
