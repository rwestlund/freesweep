#                                                           -*- Makefile.am -*-

bin_PROGRAMS = freesweep
dist_sysconf_DATA = sweeprc
dist_localstate_DATA = sweeptimes
man6_MANS = freesweep.6

freesweep_SOURCES = clear.c drawing.c files.c game.c main.c bests.c pause.c \
	play.c logs.c sl.c stats.c themes.c tick.c utils.c image.c xdg.c sweep.h \
	defaults.h
freesweep_CPPFLAGS = -DSYSCONFDIR=$(sysconfdir) \
	-DLOCALSTATEDIR=$(localstatedir) $(CURSES_CFLAGS)
freesweep_LDADD = $(CURSES_LIBS)
