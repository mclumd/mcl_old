# these next two might not work under all OSes

ARCH = $(shell arch)
SYS  = $(shell uname)

include ../make/Make.$(SYS)
include ../make/Make.$(MACHINE)

%.o:    %.cc
	$(CC) $(CFLAGS) -Iinclude -I../api -c $< -o $@

MCL_LIB_ROOTNAME = MCL2core

all: hostTest

mclLib:	
	(cd .. ; make all)

hostTest: mclLib testHost.o simHost.o
	$(LD) -o hostTest testHost.o simHost.o -L.. -l$(MCL_LIB_ROOTNAME) -lm

clean:
	-$(RM) *~
	-$(RM) *.o
	-$(RM) include/*~
	-$(RM) hostTest
