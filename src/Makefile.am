## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libansiconv.la
bin_PROGRAMS = ansiconv

libansiconv_la_SOURCES = ansiparse.c readbin.c \
                   readidf.c dosfont.c sauce.c output.c \
                   outputlo.c outputgif.c outputpng.c \
                   list.c list.h dosfont.h output.h \
                   outputgif.h outputpng.h sauce.h \
				   convert.c

pkginclude_HEADERS = ansiconv.h

ansiconv_SOURCES = main.c
ansiconv_LDADD = $(top_builddir)/src/libansiconv.la

libansiconv_la_LDFLAGS = -version-info 2:0:1

EXTRA_LTLIBRARIES = 
EXTRA_DIST = 
