
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclluapadapterdir = /usr/local/lib/ginga/adapters

libginganclluapadapter_so_LDFLAGS = -Wl,-soname,libginganclluapadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../../../model -L../../../ -L../../

libginganclluapadapter_PROGRAMS = libginganclluapadapter.so

libginganclluapadapter_so_SOURCES = LuaPlayerAdapter.cpp

libginganclluapadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter \
    -lginganclapppadapter -ldl
