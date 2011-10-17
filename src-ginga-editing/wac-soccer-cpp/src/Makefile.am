# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
        $(all_includes)


LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libgingawacsoccer_so_LDFLAGS = -Wl,-soname,libgingawacsoccer.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L/usr/local/lib/ginga/players
localLib_PROGRAMS = libgingawacsoccer.so

libgingawacsoccer_so_SOURCES = Soccer.cpp EventHandle.cpp WatchingMode.cpp

libgingawacsoccer_so_LDADD = -ltelemidiautil -llinceutil -lncl30 -lncl30generator -lgingaccsystemthread \
    -lgingawacstate -lgingaccinputmode -lgingawacediting -lgingaccplayer -lgingaccplawdg -lgingaimagecapture
