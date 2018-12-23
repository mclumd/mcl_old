# these next two might not work under all OSes

ARCH = $(shell arch)
SYS  = $(shell uname)

include ../../make/Make.$(SYS)
include ../../make/Make.$(MACHINE)

MCL_CORE_ROOTNAME = MCL2core
MCL_CORE_FILENAME = lib$(MCL_CORE_ROOTNAME).$(LIBEXT)

%.o:    %.cc
	$(CC) $(CFLAGS) -Iinclude -I../include -c $< -o $@

keymake: keymake.o
	$(LD) -o keymake keymake.o -L../ -l$(MCL_CORE_ROOTNAME)
