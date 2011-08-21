## Process this file with automake to produce Makefile.in

cgiexecdir = $(prefix)/httpd/cgi-bin
cgiexec_PROGRAMS = ansiview.cgi binview.cgi idfview.cgi \
                   ansithumb.cgi binthumb.cgi idfthumb.cgi 
                   
ansiview_cgi_SOURCES = ansi2gif.c ansiparse.c dosfont.c \
                       outputgif.c list.c list.h dosfont.h

binview_cgi_SOURCES = bin2gif.c readbin.c dosfont.c \
                      sauce.c outputgif.c list.c list.h \
                      sauce.h dosfont.h

idfview_cgi_SOURCES = idf2gif.c readidf.c dosfont.c \
                      sauce.c outputgif.c list.c list.h \
                      dosfont.h

ansithumb_cgi_SOURCES = ansi2gif.c ansiparse.c dosfont.c \
                       outputlo.c list.c list.h dosfont.h

binthumb_cgi_SOURCES = bin2gif.c readbin.c dosfont.c \
                      sauce.c outputlo.c list.c list.h \
                      sauce.h dosfont.h

idfthumb_cgi_SOURCES = idf2gif.c readidf.c dosfont.c \
                      sauce.c outputlo.c list.c list.h \
                      dosfont.h

