
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclemconverterdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libginganclemconverter_so_LDFLAGS = -Wl,-soname,libginganclemconverter.so \
    -shared -L/usr/local/lib/ginga -L../model -L../adaptation

libginganclemconverter_PROGRAMS = libginganclemconverter.so

libginganclemconverter_so_SOURCES = FormatterConverter.cpp \
    FormatterLinkConverter.cpp

libginganclemconverter_so_LDADD = -ltelemidiautil -lncl30 -lginganclmodel \
    -lgingancladaptation
