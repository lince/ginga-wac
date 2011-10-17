METASOURCES = AUTO

INCLUDES = \
    -I/usr/local/include \
    -I$(top_srcdir)/include \
    $(all_includes) \
    -I../include/util

noinst_PROGRAMS = \
	UTest

UTest_SOURCES = \
	../test/UTest.cpp

UTest_LDADD = -llinceutil -ldl

UTest_LDFLAGS = -L../bin/ -L../bin/.libs/ -L/lib/ -L/usr/lib/ -L/usr/local/lib/
