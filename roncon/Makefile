# these next two might not work under all OSes

SYS     = $(shell uname)
MACHINE = $(shell uname -n)
PROC    = $(shell uname -m)

# the following must be resolved in Make.* :
# .... in the $(SYS) file, usually:
#  PLAYERFLAGS PLAYERLIBS
#  LIBEXT STATIC_LIBEXT
#  CFLAGS LINKFLAGS
#  CC LD CP RM LS CAT

include make/Make.$(SYS)
include make/Make.$(MACHINE)
include make/Make.files

BUILDDIR = obj/$(SYS)-$(PROC)
PROJECT_OBJ_FILES = $(patsubst %.o,$(BUILDDIR)/%.o,$(RONCON_OBJ_FILES))

IDIRS    = -Isrc/include -I/usr/local/include/

# REQ_LIBS = MCL2core UMBC
REQ_LIBS = UMBC
LINKLIBS = $(patsubst %,-l%,$(REQ_LIBS)) $(PLAYERLIBS)

LIBDIRS = $(SYS_LIBDIR) $(LOCAL_LIBDIR)
LIBFLAGS = $(patsubst %,-L%,$(LIBDIRS))

$(BUILDDIR)/%.o:    src/%.cc
	$(CC) $(CFLAGS) $(IDIRS) -c $< -o $@

$(BUILDDIR)/Roncon%.o: main.cc
	$(CC) $(CFLAGS) $(IDIRS) -c $< -o $@

test/%.o: test/%.cc
	$(CC) $(CFLAGS) $(IDIRS) -c $< -o $@

all: Roncon-$(SYS) #fixperms fixgrp

Roncon-$(SYS): $(PROJECT_OBJ_FILES) $(BUILDDIR)/Roncon-$(SYS).o 
	$(LD) $(LINKFLAGS) -o $@ $^ $(LIBFLAGS) $(LINKLIBS)

fixperms:
	chmod -R a=u ../roncon

fixgrp:
	chgrp -R active ../roncon

objects: $(PROJECT_OBJ_FILES)

show:
	echo $(BUILDDIR)
	echo $(RONCON_OBJ_FILES)

clean:
	-$(RM) *~
	-$(RM) obj/*/*.o
	-$(RM) rexe-*
	-$(RM) src/*~
	-$(RM) src/include/*~

tst_args: $(PROJECT_OBJ_FILES) test/test_args.o
	$(LD) $(LINKFLAGS) -o test/$@ $^ $(LIBFLAGS) $(LINKLIBS)

tst_pbreak: test/pipebreaker.o
	$(LD) -o test/$@ $^ 

#mclconfig: $(MCL_CORE_FILENAME) obj/writeDefaultConfig.o
#	$(LD) -o mclconfig obj/writeDefaultConfig.o -l$(MCL_CORE_ROOTNAME) 

docs:
	doxygen Doxyfile

alldocs:
	doxygen Doxy-all
