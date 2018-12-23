
# defines, system commands

CC = g++
LD = g++
RM = rm
LIBEXT = so

# default file/directory definitions and shit

include Make.files

# overrides for architecture and/or machine and/or user

SYS     = $(shell uname)
ARCH    = $(shell uname -m)
MACHINE = $(shell hostname)
USNM    = $(shell whoami)

-include make/Make.$(SYS)
-include make/Make.$(ARCH)
-include make/Make.$(MACHINE)
-include make/Make.$(USNM)

# defines, switches and flags

CFLAGS = -g -Wall -m32 $(SYS_CFLAGS) $(ARCH_CFLAGS) $(MACH_CFLAGS) $(USR_CFLAGS)
LINKFLAGS = -Wl,-G
MARSLIBS = -lpthread

# uncomment if you want the TTY controller
# CFLAGS+= -DTTY

# rules

util/%.o:    util/%.cc
	$(CC) $(CFLAGS) $(UTIL_INCL) -I$(MCL_INC_DIR) -c $< -o $@

substrate/%.o: substrate/%.cc
	$(CC) $(CFLAGS) $(SUBSTRATE_INCL) $(UTIL_INCL) -I$(MCL_INC_DIR) -c $< -o $@

%.o:    %.cc
	$(CC) $(CFLAGS) $(SUBSTRATE_INCL) -I$(MCL_INC_DIR) -c $< -o $@

# libs

all: utilLibrary substrateLibrary
#all: substrateLibrary

substrateLibrary: $(SUBSTRATE_OBJS)
	$(LD) -Wl,-G -shared -o libs/moncon_substrate.so $(SUBSTRATE_OBJS) -lpthread $(UTIL_LIBS)

utilLibrary: $(UTIL_OBJS)
	$(LD) -Wl,-G -shared -o libs/moncon_utils.so $(UTIL_OBJS)

# test 

linktest: test/linktest

test/linktest: test/linktest.o 
	$(LD) -o test/linktest libs/moncon_substrate.so $^ -lpthread $(UTIL_LIBS) -L /usr/local/lib/ -lMCL2core

clean:
	-$(RM) *~
	-$(RM) libs/*.so
	-$(RM) bin/*
	-$(RM) substrate/*/*.o
	-$(RM) substrate/*/*~
	-$(RM) test/*.o
	-$(RM) test/*~
	-$(RM) substrate/include/substrate/*~

#	-$(RM) util/*.o
#	-$(RM) util/*~
#	-$(RM) util/include/mc_utils/*~
