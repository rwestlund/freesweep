######################################################################
# $Id: Makefile,v 1.1.1.1 1998-11-23 03:57:08 hartmann Exp $
######################################################################

SRCS = drawing.c files.c main.c game.c play.c bests.c error.c hash.c
HEADS = sweep.h defaults.h
MISC = README Makefile sweeprc configure.in
TARGET = newsweep
CLEANUP = a.out $(TARGET) core made $(OBJS)
VERSION = 0.8

UNAME = uname
UNAME_M = uname -m
DATE = $(shell date +%Y%m%d%H%M)
SYSTEM_TYPE = $(shell $(UNAME))_$(shell $(UNAME_M))
OBJS =  $(patsubst %.c,%.o,$(SRCS))

CC = gcc
CFLAGS = -g -O -Wall -DSYSTEM_TYPE=\"$(SYSTEM_TYPE)\" -DUSER=\"$(USER)\" -DVERSION=\"$(VERSION)\" -DDEBUG_LOG=\"debug.log\"
LIBS = -L/usr/local/lib -lncurses -lm
INCL = -I/usr/local/include

.SUFFIXES: .c .o
.PHONY: clean sterile lines distclean
#######################################################################
#$(SYSTEM_TYPE)/%.o: %.c $(HEADS) Makefile
%.o: %.c $(HEADS) Makefile
	@echo Building $<
	@/bin/rm -f $*.o 
	@[ -d $(SYSTEM_TYPE) ]|| mkdir $(SYSTEM_TYPE)
	@$(CC) -c $(CFLAGS) -o $(SYSTEM_TYPE)/$*.o $<
	@/bin/ln -s $(SYSTEM_TYPE)/$*.o .

$(TARGET): $(OBJS) $(HEADS) $(MISC)
	@echo Linking
	@/bin/rm -f $(TARGET)
	@(cd $(SYSTEM_TYPE); $(CC) $(CFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -o $(TARGET))
	@echo "Making symlink to ./$(TARGET)"
	@/bin/ln -s $(SYSTEM_TYPE)/$(TARGET) .

clean:
	@-/bin/rm -f $(CLEANUP)
#	@[ ! -d $(SYSTEM_TYPE) ] || (cd $(SYSTEM_TYPE); /bin/rm -f a.out core *.o made newsweep)
#	@[ ! -d $(SYSTEM_TYPE) ] || (cd $(SYSTEM_TYPE); /bin/rm -f $(CLEANUP) )

distclean:
	@make sterile

sterile:
	@/bin/rm -f $(CLEANUP)
	@[ ! -d $(SYSTEM_TYPE) ] || /bin/rm -rf $(SYSTEM_TYPE)
	@/bin/rm -f */*.o */core */made */$(TARGET) */a.out

tar: $(SRCS) $(HEADS) $(MISC)
	@[ -d backup ]|| mkdir backup
	@(cd ../; tar -czvf sweep/backup/$(TARGET)-$(VERSION).$(DATE).tar.gz $(addprefix ./sweep/,$(SRCS) $(HEADS) $(MISC)))

tags: $(SRCS) $(HEADS) $(MISC)
	@ctags $(SRCS) $(HEADS)

lines: $(SRCS) $(HEADS)
	@echo "Total lines of code:"
	@wc -l $(SRCS) $(HEADS)

ci: $(SRCS)
	@ci -u $(SRCS) $(HEADS)

co:
	@co $(SRCS) $(HEADS)
