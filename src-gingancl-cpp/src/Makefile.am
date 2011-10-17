
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libgingancldir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingancl_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L./adaptation -L./adapters -L./gfx \
    -L./emconverter -L./focus -L./model -L./multidevice \
    -Wl,-soname,libgingancl.so -shared

libgingancl_PROGRAMS = libgingancl.so

libgingancl_so_SOURCES = FormatterMediator.cpp FormatterScheduler.cpp \
    privatebase/PrivateBaseManager.cpp privatebase/PrivateBaseContext.cpp \
    animation/AnimationController.cpp

libgingancl_so_LDADD = -ltelemidiautil -lgingaccsystemthread \
    -lncl30 -lgingancladaptation -lgingancladapter \
    -lginganclemconverter -lginganclmd -lginganclfocus -lginganclmodel \
    -lgingaccplayer -lgingaccplaani -lginganclgfx -ldl
