AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
bin_PROGRAMS = wordgen
dist_man_MANS = wordgen.1
wordgen_SOURCES = wordgen.c gettext.h
LDADD = $(LIBINTL)

SUBDIRS = po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
