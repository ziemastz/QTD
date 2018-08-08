########################################################################
#                                                                      #
#              --- CAEN SpA - Computing Division ---                   #
#                                                                      #
#   CAENHVWRAPPER Software Project                                     #
#                                                                      #
#   Makefile: it installs shared library and HVWrapperdemo             #
#                                                                      #
#   Created: January 2010                                              #
#   Last mod: September  2014					       #
#                                                                      #
#   Auth: A. Lucchesi                                                  #
#	  M. Pardi						       #
#                                                                      #
########################################################################

GLOBALDIR=      HVWrapperDemo/

PROGRAM=	$(GLOBALDIR)HVWrappdemo

CC=		gcc

FLAGS=		-DUNIX -DLINUX

LFLAGS=

DIRLIB =        /usr/lib
LIBS=		-L $(DIRLIB) -lcaenhvwrapper -lncurses -lpthread -ldl -lm


INCLUDEDIR=	-I./$(GLOBALDIR) -I./include/

INSTALLDIR=	./

SOURCES=	$(GLOBALDIR)MainWrapp.c $(GLOBALDIR)CmdWrapp.c $(GLOBALDIR)console.c

OBJECTS=	$(GLOBALDIR)MainWrapp.o $(GLOBALDIR)CmdWrapp.o $(GLOBALDIR)console.o

INCLUDES=	MainWrapp.h CAENHVWrapper.h console.h

LIBNAME=	libcaenhvwrapper.so.5.61

LIB=		lib

INSTALLDIRLIB=	/usr/lib

INSTALLDIRINCLUDE = /usr/include

########################################################################

ARFLAGS=		r

CFLAGS=			$(FLAGS)

clean:
			rm -f $(OBJECTS) $(PROGRAM)

install:
			install $(LIB)/$(LIBNAME) $(INSTALLDIRLIB)
			ln -sf $(INSTALLDIRLIB)/$(LIBNAME) $(INSTALLDIRLIB)/libcaenhvwrapper.so
			cp ./include/CAENHVWrapper.h $(INSTALLDIRINCLUDE)/CAENHVWrapper.h
			ldconfig
			make all
			rm -f $(OBJECTS)

all:			$(PROGRAM)

$(PROGRAM):		$(OBJECTS)
			$(CC) $(CFLAGS) $(LFLAGS) -o $(PROGRAM) $(OBJECTS)\
			$(LIBS)

$(OBJECTS):		$(SOURCES)

$(GLOBALDIR)%.o:	$(GLOBALDIR)%.c
			$(CC) $(CFLAGS) $(INCLUDEDIR) -o $@ -c $<

