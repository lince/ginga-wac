
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libginganclmddir = /usr/local/lib/ginga

libginganclmd_so_LDFLAGS = -L/usr/local/lib/ -L/usr/local/lib/ginga \
    -L../model -L../adaptation \
    -Wl,-soname,libginganclmd.so -shared

libginganclmd_PROGRAMS = libginganclmd.so

libginganclmd_so_SOURCES = FormatterMultiDevice.cpp FormatterBaseDevice.cpp \
    FormatterPassiveDevice.cpp FormatterActiveDevice.cpp

libginganclmd_so_LDADD = -ltelemidiautil -lncl30 -lgingaccsystemiocodemap \
    -lginganclmodel -lgingancladaptation -lpthread -ldl
