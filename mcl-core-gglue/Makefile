# these next two might not work under all OSes

ARCH = $(shell arch)
SYS  = $(shell uname)

# ifeq ($(SYS),Linux)
#   include make/Make.linux
# else
#   $(error No configuration file for your OS in make/Make.$(SYS))
# endif

include make/Make.$(SYS)
include make/Make.$(MACHINE)

IDIRS =  -Iinclude -Iapi -Iconfig -Ismile

# uncomment if you want ungodly amounts of output
# CFLAGS = -DDEBUG_ONTOLOGIES

%.o:    %.cc
	$(CC) $(CFLAGS) $(IDIRS) -DSAFE -c $< -o $@

MCL_CORE_ROOTNAME = MCL2core
MCL_CORE_FILENAME = lib$(MCL_CORE_ROOTNAME).$(LIBEXT)

include Make.files

$(MCL_CORE_FILENAME): $(MCL_CORE_FILES)
	$(LD) $(LINKFLAGS) -o $@ $^ libs/libsmile.$(STATIC_LIBEXT)
	$(CP) $(MCL_CORE_FILENAME) $(SYS_LIBDIR)

objects: $(MCL_CORE_FILES)

all: $(MCL_CORE_FILENAME)

clean:
	-$(RM) include/*~
	-$(RM) config/*~
	-$(RM) config/*.o
	-$(RM) util/*.o
	-$(RM) *~
	-$(RM) *.o
	-$(RM) *.so

utils/makeDot_exe.o: utils/makeDot.cc
	$(CC) -DEXECUTABLE $(CFLAGS) $(IDIRS) -c $< -o $@


dot: $(MCL_CORE_FILENAME) utils/makeDot_exe.o
	$(LD) -o utils/makeDot utils/makeDot_exe.o -l$(MCL_CORE_ROOTNAME)
	( utils/makeDot )
	( dot -Tps config/default/indication.dot -o documentation/indications.ps )
	( dot -Tps config/default/failures.dot -o documentation/failures.ps )
	( dot -Tps config/default/responses.dot -o documentation/responses.ps )
	( dot -Tps config/default/mclOntologies.dot -o documentation/mclOntologies.ps )


test: $(MCL_CORE_FILENAME) mclTest.o 
	$(LD) -o ta mclTest.o -l$(MCL_CORE_ROOTNAME)

testTU: $(MCL_CORE_FILENAME) test/testTU.o
	$(LD) -o test/testTU test/testTU.o -l$(MCL_CORE_ROOTNAME)	
	test/testTU

testOBS: $(MCL_CORE_FILENAME) test/testOBS.o
	$(LD) -o test/testOBS test/testOBS.o -l$(MCL_CORE_ROOTNAME)	
	test/testOBS

mclconfig: $(MCL_CORE_FILENAME) writeDefaultConfig.o
	$(LD) -o mclconfig writeDefaultConfig.o -l$(MCL_CORE_ROOTNAME) 

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

# PNL is out of the will
#
# LINKFLAGS+= $(PNL_LINKFLAGS)
# IDIRS+= $(patsubst %,-I%,$(PNL_INCDIRS))
#tpnl: $(MCL_CORE_FILENAME) testPNL.o
#	$(LD) -o tpnl testPNL.o -l$(MCL_CORE_ROOTNAME) $(PNL_LINKFLAGS)
#tpnl_link.o: testPNL.cc
#	$(CC) $(CFLAGS) -DCOMPILE_FOR_LINKING $(IDIRS) -c testPNL.cc -o tpnl_link.o
