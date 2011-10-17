
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =

noinst_PROGRAMS = test

test_SOURCES = main.cpp

test_LDADD = -lgingavideocapture

test_LDFLAGS = -L/usr/local/lib/ginga -L/usr/local/lib/ginga/capture -L../include/ -L../src/ -L/usr/local/lib -g3 -ggdb
