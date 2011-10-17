
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =
localLibdir = /usr/local/lib/ginga/iocontents/

libgingaccsysctsfs_so_LDFLAGS = -Wl,-soname,libgingaccsysctsfs.so --shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../

localLib_PROGRAMS = libgingaccsysctsfs.so

libgingaccsysctsfs_so_SOURCES = DFBAudioProvider.cpp \
    FusionSoundAudioProvider.cpp

libgingaccsysctsfs_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    -lgingaccsystemiodfb -lgingaccsystemio
