##################################################################
##
## Microsoft Windows Visual Studio 6 makefile for mcl-core
## 
## From the original Linux Makefile by Dean Wright February 2007
##
## usage: nmake -f nmake.mak 
##
##################################################################

#===================================================================
#   PNL config
#===================================================================

PNL = ..\pnl

#===================================================================
#   Object file lists
#===================================================================

MCL_CORE_FILES = bayesNodes.obj \
         buildCore.obj \
		 dynamicILinks.obj \
		 expectations.obj \
		 expectationFactory.obj \
		 keyGen.obj \
         linkFactory.obj \
		 links.obj \
		 mcl.obj \
		 mclCPT.obj \
		 mclEntity.obj \
		 mclExceptions.obj \
		 mclFrame.obj \
		 mclHostAPI.obj \
		 mclMonitorResponse.obj \
         mclNode.obj \
         mclOntology.obj \
		 mclProperties.obj \
		 mclSensorDef.obj \
		 mclTimer.obj \
		 oNodes.obj \
		 output.obj \
		 textUtils.obj \
         config\configCPT.obj \
         config\configManager.obj \
         config\configRC.obj

#===================================================================
#   MCL config
#===================================================================

LIBEXT   = LIB
MCL_CORE_ROOTNAME = MCL2core
MCL_CORE_FILENAME = $(MCL_CORE_ROOTNAME).$(LIBEXT)

#===================================================================
#   Compiler/Linker and Flags and Stuff
#===================================================================

CC     = CL
DEFINE = -DWIN32 -D_CRT_SECURE_NO_DEPRECATE
IDIRS  = -Iinclude -Iconfig -Iapi -Ipnl\include
CFLAGS = /nologo /TP /GR /EHsc -c 
DFLAGS = /nologo /TP /GR /EHsc -c /Zi /w

LD     = LINK
LFLAGS = /nologo /SUBSYSTEM:CONSOLE
DLLFLAG = /DLL

#LIBS   = WSOCK32.LIB
LIBS   = $(PNL)\lib\pnl.lib $(PNL)\high\bin\PNLHigh.lib

LS     = DIR /B
CAT    = TYPE
RM     = ERASE
CP     = COPY
LIBCMD = LIB

#===================================================================
#   Default inference rules
#===================================================================
.cc.obj:
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

.c.obj:
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.c

#===================================================================
#   Main build rules
#===================================================================

all: $(MCL_CORE_FILENAME)

$(MCL_CORE_FILENAME): $(MCL_CORE_FILES)
	$(LIBCMD) $(LFLAGS) /out:$(MCL_CORE_FILENAME) $(MCL_CORE_FILES)

debug:
    nmake /NOLOGO -f nmake.mak -a "CFLAGS= $(DFLAGS)" "LFLAGS= $(LFLAGS) /debug"

#===================================================================
#   MCL Tester
#===================================================================

test: $(MCL_CORE_FILENAME) mclTest.obj  testPNL.obj $(MCL_CORE_FILENAME)
	$(LD) $(LFLAGS) /out:mclTest.exe mclTest.obj $(MCL_CORE_FILENAME) $(LIBS) 
	$(LD) $(LFLAGS) /out:pnlTest.exe testPNL.obj $(MCL_CORE_FILENAME) $(LIBS) 

dtest:
    nmake /NOLOGO -f nmake.mak test -a "CFLAGS= $(DFLAGS)" "LFLAGS= $(LFLAGS) /debug"

#===================================================================
#   Generate configuration file
#===================================================================
mclconfig: $(MCL_CORE_FILENAME) writeDefaultConfig.obj
	$(LD) $(LFLAGS) /out:mclconfig.exe writeDefaultConfig.obj $(MCL_CORE_FILENAME) $(LIBS) 

#===================================================================
#   Accessary make targets
#===================================================================
clean:
	-if exist *.obj                $(RM) *.obj
	-if exist config\*.obj         $(RM) config\*.obj  
	-if exist host_fake\*.obj      $(RM) host_fake\*.obj  
	-if exist utils\*.obj          $(RM) utils\*.obj  
	-if exist MCL2Core.LIB         $(RM) MCL2Core.LIB  
	-if exist *.exe                $(RM) *.exe  

tidy:
	-if exist *~                   $(RM) *~  
	-if exist config\*~            $(RM) config\*~  
	-if exist host_fake\*~         $(RM) host_fake\*~  
	-if exist utils\*~             $(RM) utils\*~  
	-if exist api\*~               $(RM) api\*~  

#===================================================================
#   Source file lists
#===================================================================
bayesNodes.obj: bayesNodes.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

buildCore.obj: buildCore.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS)  $*.cc /Fo$*.obj

dynamicILinks.obj: dynamicILinks.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

expectations.obj: expectations.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

expectationFactory.obj: expectationFactory.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

keyGen.obj: keyGen.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

linkFactory.obj: linkFactory.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

links.obj: links.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mcl.obj: mcl.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclCPT.obj: mclCPT.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclEntity.obj: mclEntity.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclExceptions.obj: mclExceptions.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclFrame.obj: mclFrame.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclHostAPI.obj: mclHostAPI.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclMonitorResponse.obj: mclMonitorResponse.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclNode.obj: mclNode.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclOntology.obj: mclOntology.cc	 
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclProperties.obj: mclProperties.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclSensorDef.obj: mclSensorDef.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclTimer.obj: mclTimer.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

mclTest.obj: mclTest.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

testPNL.obj: testPNL.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

oNodes.obj: oNodes.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

output.obj: output.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

textUtils.obj: textUtils.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

config\configCPT.obj: config\configCPT.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

config\configManager.obj: config\configManager.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

config\configRC.obj: config\configRC.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj

writeDefaultConfig.obj: writeDefaultConfig.cc
    $(CC) $(CFLAGS) $(DEFINE) $(IDIRS) $*.cc /Fo$*.obj
 
#===================================================================
# end                    n m a k e . m a k                       end 
#===================================================================

