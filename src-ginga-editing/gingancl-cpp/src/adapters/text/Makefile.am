
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

#LinksPlayerAdapter
libgingancllkspadapterdir = /usr/local/lib/ginga/adapters

libgingancllkspadapter_so_LDFLAGS = -Wl,-soname,libgingancllkspadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libgingancllkspadapter_PROGRAMS = libgingancllkspadapter.so

libgingancllkspadapter_so_SOURCES = LinksPlayerAdapter.cpp

libgingancllkspadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl

#PlainTxtPlayerAdapter
libginganclpltpadapterdir = /usr/local/lib/ginga/adapters

libginganclpltpadapter_so_LDFLAGS = -Wl,-soname,libginganclpltpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclpltpadapter_PROGRAMS = libginganclpltpadapter.so

libginganclpltpadapter_so_SOURCES = PlainTxtPlayerAdapter.cpp

libginganclpltpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl


#SubtitlePlayerAdapter
libginganclsubpadapterdir = /usr/local/lib/ginga/adapters

libginganclsubpadapter_so_LDFLAGS = -Wl,-soname,libginganclsubpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclsubpadapter_PROGRAMS = libginganclsubpadapter.so

libginganclsubpadapter_so_SOURCES = SubtitlePlayerAdapter.cpp

libginganclsubpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl
