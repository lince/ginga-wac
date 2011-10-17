
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD =  -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
    -lgingaccsystemiocodemap -lgingaccsystemiodfb \
    -lgingaccplayer -lgingaccplaavp -lgingaccplaimg -lgingaccplalua \
    -lgingaccplatxt -lgingaccplasrt -lgingaccplaplt \
    -lgingaccplacha -lgingaccplapav -lgingaccplawdg \
    -ltelemidialinks -ldirectfb -lgingaccsysctsdfbav -lgingaccsysctsdfbf \
    -lgingaccsysctsfs -lgingaccsysctsimgdfb -llua

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga -L../src/ -L../src/image/ -L../src/lang/lua \
    -L../src/text -L../src/text/srt -L../src/text/links -L../src/text/plain \
    -L../src/tv -L../src/video -L../src/widget -g3 -ggdb
