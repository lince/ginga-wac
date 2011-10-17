
# set the include path found by configure
INCLUDES = $(all_includes) $(COMPONENTS_INCS)
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD =  -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
    -lgingaccsystemiocodemap -lgingaccsystemiodfb \
    -lgingaccplayer -lgingaccsysctsimgdfb -lgingaccplaimg -lgingaccplaani \
    -lncl30 -lgingancladaptation -lgingancladapter \
    -lginganclapppadapter -lginganclimgpadapter -lginganclemconverter \
    -lginganclfocus \
    -lginganclmodel -lgingancl -lginganclgfx -lginganclmd $(COMPONENTS_LIBS)

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../src/adapters -L../src/adapters/image \
    -L../src/adapters/application \
    -L../src/adaptation -L../src/emconverter -L../src/multidevice \
    -L../src/focus -L../src/model -L../src/ -L../src/gfx
