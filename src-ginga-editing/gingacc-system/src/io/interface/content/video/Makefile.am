
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =

localLibdir = /usr/local/lib/ginga/iocontents/

libgingaccsysctsdfbav_so_LDFLAGS = -Wl,-soname,libgingaccsysctsdfbav.so \
    --shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../

localLib_PROGRAMS = libgingaccsysctsdfbav.so

libgingaccsysctsdfbav_so_SOURCES = DFBVideoProvider.cpp

libgingaccsysctsdfbav_so_LDADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccsystemiodfb -lgingaccsystemio
