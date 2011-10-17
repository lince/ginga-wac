
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclimgpadapterdir = /usr/local/lib/ginga/adapters

libginganclimgpadapter_so_LDFLAGS = -Wl,-soname,libginganclimgpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclimgpadapter_PROGRAMS = libginganclimgpadapter.so

libginganclimgpadapter_so_SOURCES = ImagePlayerAdapter.cpp

libginganclimgpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl
