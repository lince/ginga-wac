
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclfocusdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libginganclfocus_so_LDFLAGS = -Wl,-soname,libginganclfocus.so -shared \
    -L/usr/local/lib/ginga -L../model -L../adaptation -L../emconverter \
    -L../adapters

libginganclfocus_PROGRAMS = libginganclfocus.so

libginganclfocus_so_SOURCES = FormatterFocusManager.cpp

libginganclfocus_so_LDADD = -ltelemidiautil -lpthread \
    -lncl30 -lgingaccsystemiocodemap \
    -lginganclmodel -lgingancladaptation -lginganclemconverter \
    -lgingancladapter -ldl
