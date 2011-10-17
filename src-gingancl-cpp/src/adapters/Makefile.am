
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libgingancladapterdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingancladapter_so_LDFLAGS = -L/lib -L/usr/lib -L/usr/local/lib \
    -L/usr/local/lib/ginga -L../model \
    -Wl,-soname,libgingancladapter.so -shared -ltelemidiautil \
    -lpthread -lgingaccsystemthread -lncl30 -lginganclmodel -lgingacccm -ldl

libgingancladapter_PROGRAMS = libgingancladapter.so

libgingancladapter_so_SOURCES = FormatterPlayerAdapter.cpp \
    NominalEventMonitor.cpp PlayerAdapterManager.cpp
