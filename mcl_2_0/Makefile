# these are the variables that need to get filled out

SYS_INCDIRS = 
MACHINE_INCDIRS =
SYS_LIBDIRS =
MACHINE_LIBDIRS =
STATIC_LIBEXT = 
SYS_LIBS = 
MACHINE_LIBS =
SYS_CFLAGS =
ARCH_CFLAGS =
MACHINE_CFLAGS =
BUILD_CFLAGS = -DSAFE

# these next two might not work under all OSes

ARCH = $(shell uname -m)
SYS  = $(shell uname)

# ifeq ($(SYS),Linux)
#   include make/Make.linux
# else
#   $(error No configuration file for your OS in make/Make.$(SYS))
# endif

include make/Make.$(SYS)
include make/Make.$(ARCH)
include make/Make.$(MACHINE)

SMILE_LIB = smile_$(SYS)_$(ARCH)
UMBC_LIB  = UMBC

BUILD_INCDIRS = src/include api/mcl glue/smile
BUILD_LIBDIRS = libs/glue

IDIRS = $(BUILD_INCDIRS) $(SYS_INCDIRS) $(MACHINE_INCDIRS)
LDIRS = $(BUILD_LIBDIRS) $(SYS_LIBDIRS) $(MACHINE_LIBDIRS)
LIBS  = $(SMILE_LIB) $(UMBC_LIB) $(SYS_LIBS) $(MACHINE_LIBS)

IDIR_FLAGS = $(patsubst %,-I%,$(IDIRS))
LDIR_FLAGS = $(patsubst %,-L%,$(LDIRS))
LIBS_FLAGS = $(patsubst %,-l%,$(LIBS))
CFLAGS = $(BUILD_CFLAGS) $(MACHINE_CFLAGS) $(SYS_CFLAGS) $(ARCH_CFLAGS)

# uncomment if you want ungodly amounts of output
# CFLAGS = -DDEBUG_ONTOLOGIES

obj/%.o:    src/%.cc
	$(CC) $(CFLAGS) $(IDIR_FLAGS) -c $< -o $@

test/%.o:    test/%.cc
	$(CC) $(CFLAGS) $(IDIR_FLAGS) -c $< -o $@

test_%: test/test%.o $(MCL_CORE_FILENAME)
	$(LD) -o $@ $^ $(LDIR_FLAGS) $(LIBS_FLAGS) -lMCL2core

MCL_CORE_ROOTNAME = MCL2core
MCL_CORE_FILENAME = lib$(MCL_CORE_ROOTNAME).$(LIBEXT)

include make/Make.files

all: lib server

obj/server.o: server.cc
	$(CC) $(CFLAGS) $(IDIR_FLAGS) -c $< -o $@

lib: $(MCL_CORE_FILENAME)

$(MCL_CORE_FILENAME): symbols $(MCL_OBJ_FILES)
	$(LD) $(LINKFLAGS) -o $@ $(MCL_OBJ_FILES) $(LDIR_FLAGS) $(LIBS_FLAGS) 

prepare: 
	-$(MDIR) $(MACHINE_INSTALLROOT)/include/mcl/
	-$(MDIR) $(MACHINE_INSTALLROOT)/lib/mcl/
	-$(MDIR) $(MACHINE_INSTALLROOT)/lib/mcl/config/

install: $(MCL_CORE_FILENAME)
	$(CP) $(MCL_CORE_FILENAME) $(MACHINE_INSTALLROOT)/lib/
	$(CP) api/mcl/*.h $(MACHINE_INSTALLROOT)/include/mcl/
	$(CP) assets/settings.tet $(MACHINE_INSTALLROOT)/lib/mcl/mcl_settings.tet
	$(CPR) config/netdefs/*.ont $(MACHINE_INSTALLROOT)/lib/mcl/netdefs/
	$(CPR) config/default $(MACHINE_INSTALLROOT)/lib/mcl/config/

objects: $(MCL_OBJ_FILES)

obj/symbols.o: utils/symbols.cc
	$(CC) $(CFLAGS) $(IDIR_FLAGS) -DSAFE -c $< -o $@

symbols: obj/symbols.o
	$(LD) -o $@ $^ $(LDIR_FLAGS) $(LIBS_FLAGS)
	./symbols .

server: obj/server.o $(MCL_CORE_FILENAME)
	$(LD) $(LDIR_FLAGS) -lUMBC -o bin/$@ $^ 

clean:
	-$(RM) obj/*.o
	-$(RM) obj/utils/*.o
	-$(RM) obj/config/*.o
	-$(RM) test/*.o
	-$(RM) test_*
	-$(RM) mclconfig
	-$(RM) src/*~
	-$(RM) src/utils/*~
	-$(RM) src/config/*~
	-$(RM) src/include/*~
	-$(RM) utils/*~
	-$(RM) *.so
	-$(RM) *~
	-$(RM) server
	-$(RM) symbols

obj/makeDot_exe.o: src/utils/makeDot.cc
	$(CC) -DEXECUTABLE $(CFLAGS) $(IDIR_FLAGS) -c $< -o $@

dot: $(MCL_CORE_FILENAME) obj/makeDot_exe.o
	$(LD) -o bin/makeDot obj/makeDot_exe.o -l$(MCL_CORE_ROOTNAME)
	( bin/makeDot )
	( dot -Tps config/default/indication.dot -o documentation/indications.ps )
	( dot -Tps config/default/failures.dot -o documentation/failures.ps )
	( dot -Tps config/default/responses.dot -o documentation/responses.ps )
	( dot -Tps config/default/mclOntologies.dot -o documentation/mclOntologies.ps )

obj/writeconfig.o: src/utils/writeDefaultConfig.cc
	$(CC) -DEXECUTABLE $(CFLAGS) $(IDIR_FLAGS) -c $< -o $@

mclconfig: $(MCL_CORE_FILENAME) obj/writeconfig.o
	$(LD) -o bin/mclconfig obj/writeconfig.o $(LDIR_FLAGS) -lUMBC -l$(MCL_CORE_ROOTNAME)

test: $(MCL_CORE_FILENAME) mclTest.o 
	$(LD) -o ta mclTest.o -l$(MCL_CORE_ROOTNAME)

testTU: $(MCL_CORE_FILENAME) test/testTU.o
	$(LD) -o test/testTU test/testTU.o $(LDIR_FLAGS) -lumbc
	test/testTU

testOBS: $(MCL_CORE_FILENAME) test/testOBS.o
	$(LD) -o test/testOBS test/testOBS.o -l$(MCL_CORE_ROOTNAME) $(LDIR_FLAGS) -lUMBC
	test/testOBS

testSYM: symbols obj/mcl_symbols.o test/testSYM.o
	$(LD) -o test/testSYM test/testSYM.o obj/mcl_symbols.o $(LDIR_FLAGS) -lUMBC
	test/testSYM

config: mclconfig
	./mclconfig cpt default 

testconfig: $(MCL_CORE_FILENAME) config/testconfig.o
	$(LD) -o testconfig config/testconfig.o -l$(MCL_CORE_ROOTNAME)

docs:
	doxygen Doxyfile

alldocs:
	doxygen Doxy-all

jni::
	(cd jni; make)

tsmile: $(MCL_CORE_FILENAME) testSMILE.o
	$(LD) -o tsmile testSMILE.o -l$(MCL_CORE_ROOTNAME) 

tags:
	-$(RM) TAGS
	find . -name '*.cc' | xargs etags -I --append
	find . -name '*.h'  | xargs etags -I --append
	find . -name '*.c'  | xargs etags -I --append

# PNL is out of the will
#
# LINKFLAGS+= $(PNL_LINKFLAGS)
# IDIRS+= $(patsubst %,-I%,$(PNL_INCDIRS))
#tpnl: $(MCL_CORE_FILENAME) testPNL.o
#	$(LD) -o tpnl testPNL.o -l$(MCL_CORE_ROOTNAME) $(PNL_LINKFLAGS)
#tpnl_link.o: testPNL.cc
#	$(CC) $(CFLAGS) -DCOMPILE_FOR_LINKING $(IDIRS) -c testPNL.cc -o tpnl_link.o
