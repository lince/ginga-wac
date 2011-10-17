
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

#LinksPlayerAdapter
libgingancllkspadapterdir = /usr/local/lib/ginga/adapters

libgingancllkspadapter_so_LDFLAGS = -Wl,-soname,libgingancllkspadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../../../model -L../../../ -L../../

libgingancllkspadapter_PROGRAMS = libgingancllkspadapter.so

libgingancllkspadapter_so_SOURCES = LinksPlayerAdapter.cpp

libgingancllkspadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter \
    -lginganclapppadapter -ldl
