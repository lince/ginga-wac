METASOURCES = AUTO

INCLUDES = \
    -I/usr/local/include \
    -I$(top_srcdir)/include \
    $(all_includes) \
    -I../include/util

########################################
# Util
########################################

util_header = \
	../include/util/LoggerUtil.h \
	../include/util/Utils.h \
	../include/util/InitializationException.h \
	../include/util/BadArgumentException.h

util_source = \
	../src/util/LoggerUtil.cpp \
	../src/util/Utils.cpp \
	../src/util/InitializationException.cpp \
	../src/util/BadArgumentException.cpp

########################################
# Install
########################################

localIncludedir = /usr/local/include/ginga/linceutil

localInclude_HEADERS = \
	$(util_header)

localLibdir = /usr/local/lib/ginga

localLib_LTLIBRARIES = liblinceutil.la

liblinceutil_la_SOURCES = \
	$(util_source)

liblinceutil_la_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ -version-info 0:0:0 -shared -Wl,-soname -Wl,liblinceutil.so $(UTIL_LIBS) -ldl
