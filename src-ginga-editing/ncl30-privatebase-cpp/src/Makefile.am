# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
        $(all_includes)


LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libncl30privatebase_so_LDFLAGS = -Wl,-soname,libncl30privatebase.so -shared \
-L/usr/local/lib -L/usr/local/lib/ginga
localLib_PROGRAMS = libncl30privatebase.so

libncl30privatebase_so_SOURCES = PrivateBaseManager.cpp DocumentManagerAdapter.cpp

libncl30privatebase_so_LDADD = -lncl30 -lncl30converter -llinceutil

