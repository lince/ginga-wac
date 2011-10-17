
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    $(all_includes)

LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD =  -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
              -lgingaccsystemiodfb -lgingaccsystemiocodemap

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L../src/thread -L../src/io -g3 -ggdb
