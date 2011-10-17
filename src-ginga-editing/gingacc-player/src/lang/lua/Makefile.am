
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	-I/usr/local/include/sfslite-1.2/ $(all_includes)

LIBS = /usr/local/lib/sfslite-1.2/libasync.la

localLibdir = /usr/local/lib/ginga/players/

libgingaccplalua_so_LDFLAGS = -Wl,-soname,libgingaccplalua.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../ -L../../image/

localLib_PROGRAMS = libgingaccplalua.so

libgingaccplalua_so_SOURCES = LuaCanvas.cpp LuaEvent.cpp LuaPlayer.cpp

libgingaccplalua_so_LDADD = -llua -ldirectfb -ltelemidiautil \
     -lgingaccsystemthread -lgingaccsystemiocodemap -lgingaccplayer \
     -lgingaccplaimg -ldl
