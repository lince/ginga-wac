# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
        $(all_includes)


LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libgingawacediting_so_LDFLAGS = -Wl,-soname,libgingawacediting.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga
localLib_PROGRAMS = libgingawacediting.so

libgingawacediting_so_SOURCES = ClientEditingManager.cpp ModeManager.cpp EditingCommand.cpp

libgingawacediting_so_LDADD = -ltelemidiautil -lncl30 -lncl30generator -lgingaccsystemthread \
    -lgingaccsystemio -lgingaccsystemiocodemap -lgingawacstate -lpthread -lfbscreenshot \
	-lgingaccinputmode
