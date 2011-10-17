
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libgingancladaptationdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingancladaptation_so_LDFLAGS = -Wl,-soname,libgingancladaptation.so \
    -shared -L../model

libgingancladaptation_PROGRAMS = libgingancladaptation.so

libgingancladaptation_so_SOURCES = context/ContextBase.cpp \
    context/PresentationContext.cpp context/RuleAdapter.cpp

libgingancladaptation_so_LDADD = -ltelemidiautil -lncl30 -lginganclmodel -lgingacccm
