
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =

libgingaccplasrtdir = /usr/local/lib/ginga/players/

libgingaccplasrt_so_LDFLAGS = -Wl,-soname,libgingaccplasrt.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../ -L../

libgingaccplasrt_PROGRAMS = libgingaccplasrt.so

libgingaccplasrt_so_SOURCES = SrtPlayer.cpp

libgingaccplasrt_so_LDADD = -ltelemidiautil -lgingaccsystemthread \
    -lgingaccplayer -lgingaccplatxt -ldl
