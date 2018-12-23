
# defines, system commands

CC = g++
LD = g++
RM = rm
LIBEXT = so

# defines, switches and flags

include ../../Make.files

CFLAGS     = -g -Wall
LINKFLAGS  = -Wl,-G
MONCONINCS = -I../../substrate/include -I/fs/metacog/group/mcl/include
MONCONLIBS = ../../libs/moncon_substrate.so

# files

DOMDIRS = agents common controllers env scorers mcl
DOMINCS = $(patsubst %,-I%/include,$(DOMDIRS))
DOMSRCS = $(foreach d, $(DOMDIRS), $(wildcard $(d)/*.cc))
DOMOBJS = $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(DOMSRCS)))

## MAKE DOMAIN-SPECIFIC FLAG ADDITIONS HERE

LIBPATHS = -L/fs/metacog/group/mcl/lib/
MARSLIBS = -lpthread -lUMBC -lMCL2core

#rules 

all: mars_main

%.o:    %.cc
	$(CC) $(CFLAGS) $(DOMINCS) $(MONCONINCS) -I$(MCL_INC_DIR) -c $< -o $@

mars_main: $(DOMOBJS) main.o 
	$(LD) -o mars_main $^ $(LIBPATHS) $(MONCONLIBS) $(MARSLIBS)

mars_test: $(DOMOBJS) test.o 
	$(LD) -o mars_test $^ $(LIBPATHS) $(MONCONLIBS) $(MARSLIBS)

obj/timeline_generator.o: utils/timeline_generator.cc
	$(CC) $(CFLAGS) $(DOMINCS) $(MONCONINCS) -I$(MCL_INC_DIR) -c $< -o $@

tlg: $(DOMOBJS) obj/timeline_generator.o
	$(LD) -o utils/tlg $^ $(LIBPATHS) $(MONCONLIBS) $(MARSLIBS)

objects: $(DOMOBJS)

libs: 
	(cd ../..; make all)

clean:
	-$(RM) *~
	-$(RM) main.o mars_main
	-$(RM) agents/*.o
	-$(RM) agents/*~
	-$(RM) agents/include/*~
	-$(RM) common/*.o
	-$(RM) common/*~
	-$(RM) common/include/*~
	-$(RM) controllers/*.o
	-$(RM) controllers/*~
	-$(RM) controllers/include/*~
	-$(RM) env/*.o
	-$(RM) env/*~
	-$(RM) env/include/*~
	-$(RM) mcl/*.o
	-$(RM) mcl/*~
	-$(RM) mcl/include/*~
	-$(RM) scorers/*.o
	-$(RM) scorers/*~
	-$(RM) scorers/include/*~
