
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD =  -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
    -lgingaccsystemiocodemap -lgingaccsystemiodfb \
    -lgingaccplayer -lgingaccsysctsimgdfb -lgingaccplaimg -lgingaccplaani \
    -lncl30 -lncl30converter -lgingancladaptation -lgingancladapter \
    -lginganclemconverter -lginganclfocus -lginganclmodel -lgingancl \
    -lncl30privatebase -lgingawac -lginganclgfx

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga/players -L../src/adaptation -L../src/adapters \
    -L../src/emconverter -L../src/focus -L../src/model -L../src/ -L../src/gfx \
    -g3 -ggdb
