
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

#AVPlayerAdapter
libginganclaavpadapterdir = /usr/local/lib/ginga/adapters

libginganclaavpadapter_so_LDFLAGS = -Wl,-soname,libginganclaavpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclaavpadapter_PROGRAMS = libginganclaavpadapter.so

libginganclaavpadapter_so_SOURCES = AVPlayerAdapter.cpp

libginganclaavpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl


#ProgramAVPlayerAdapter
libginganclpavpadapterdir = /usr/local/lib/ginga/adapters

libginganclpavpadapter_so_LDFLAGS = -Wl,-soname,libginganclpavpadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclpavpadapter_PROGRAMS = libginganclpavpadapter.so

libginganclpavpadapter_so_SOURCES = tv/ProgramAVPlayerAdapter.cpp

libginganclpavpadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl


#ChannelPlayerAdapter
libginganclchapadapterdir = /usr/local/lib/ginga/adapters

libginganclchapadapter_so_LDFLAGS = -Wl,-soname,libginganclchapadapter.so \
    -shared -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../../model -L../

libginganclchapadapter_PROGRAMS = libginganclchapadapter.so

libginganclchapadapter_so_SOURCES = ChannelPlayerAdapter.cpp

libginganclchapadapter_so_LDADD = -lncl30 -lginganclmodel -lgingancladapter -ldl
