
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclapppadapterdir = /usr/local/lib/ginga/adapters

libginganclapppadapter_so_LDFLAGS = -Wl,-soname,libginganclapppadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclapppadapter_PROGRAMS = libginganclapppadapter.so

libginganclapppadapter_so_SOURCES = ApplicationPlayerAdapter.cpp

libginganclapppadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl
