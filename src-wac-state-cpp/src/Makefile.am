# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
        $(all_includes)


LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libgingawacstate_so_LDFLAGS = -Wl,-soname,libgingawacstate.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga
localLib_PROGRAMS = libgingawacstate.so

libgingawacstate_so_SOURCES = PresentationState.cpp StateManager.cpp ElementaryState.cpp

libgingawacstate_so_LDADD = -ltelemidiautil -lgingaccplayer

