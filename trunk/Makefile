# Generated automatically from Makefile.in by configure.
######################################################################
# $Id: Makefile,v 1.5 1999-02-11 01:46:27 hartmann Exp $
######################################################################

SRCS = drawing.c files.c main.c game.c play.c bests.c error.c clear.c
HEADS = sweep.h defaults.h
MISC = README Makefile sweeprc configure.in install-sh configure.in Makefile.in config.guess configure config.sub
TARGET = newsweep
CLEANUP = a.out $(TARGET) core made $(OBJS) config.cache config.log config.status
VERSION = 0.81

NOW = $(shell /s/std/bin/date +%Y%m%d%H%M)
SYSTEM_TYPE = i386-unknown-solaris2.6
OBJS =  $(patsubst %.c,%.o,$(SRCS))

CC = gcc
CFLAGS = -Wall -DSYSTEM_TYPE=\"$(SYSTEM_TYPE)\" -DUSER=\"$(USER)\" -DVERSION=\"$(VERSION)\" -DDEBUG_LOG=\"debug.log\" -g -O2  -DHAVE_LIBM=1 -DHAVE_LIBNCURSES=1 -DHAVE_SNPRINTF=1 -DSTDC_HEADERS=1 -DHAVE_STRINGS_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STRDUP=1 
LIBS = -lncurses -lm -L/usr/local/lib 
LDFLAGS = 

.SUFFIXES: .c .o
.PHONY: clean sterile lines distclean
#######################################################################
%.o: %.c $(HEADS) Makefile
	@echo Building $<
	@/bin/rm -f $*.o 
	@[ -d $(SYSTEM_TYPE) ]|| mkdir $(SYSTEM_TYPE)
	@$(CC) -c $(CFLAGS) -o $(SYSTEM_TYPE)/$*.o $<
	@ln -s $(SYSTEM_TYPE)/$*.o .

$(TARGET): $(OBJS) $(HEADS) $(MISC)
	@echo Linking
	@/bin/rm -f $(TARGET)
	@(cd $(SYSTEM_TYPE); $(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $(TARGET))
	@echo "Making symlink to ./$(TARGET)"
	@ln -s $(SYSTEM_TYPE)/$(TARGET) .

configure: configure.in
	@autoconf

clean:
	@-/bin/rm -rf $(CLEANUP) $(SYSTEM_TYPE)

distclean:
	@make sterile

sterile:
	@/bin/rm -f $(CLEANUP)
	@[ ! -d $(SYSTEM_TYPE) ] || /bin/rm -rf $(SYSTEM_TYPE)
	@/bin/rm -f */*.o */core */made */$(TARGET) */a.out

tar: $(SRCS) $(HEADS) $(MISC)
	@(cd ../; tar -czvf $(TARGET)-$(VERSION).$(NOW).tar.gz $(addprefix ./sweep/,$(SRCS) $(HEADS) $(MISC)))

tags: $(SRCS) $(HEADS) $(MISC)
	@ctags $(SRCS) $(HEADS)

lines: $(SRCS) $(HEADS)
	@echo "Total lines of code:"
	@wc -l $(SRCS) $(HEADS)

ci: $(SRCS)
	@ci $(SRCS) $(HEADS)

co:
	@co $(SRCS) $(HEADS)
