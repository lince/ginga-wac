
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclnclpadapterdir = /usr/local/lib/ginga/adapters

libginganclnclpadapter_so_LDFLAGS = -Wl,-soname,libginganclnclpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../../../model -L../../../ -L../../

libginganclnclpadapter_PROGRAMS = libginganclnclpadapter.so

libginganclnclpadapter_so_SOURCES = NCLPlayerAdapter.cpp

libginganclnclpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter \
    -lginganclapppadapter -ldl
