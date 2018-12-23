# java section

ARCH = $(shell arch)
SYS  = $(shell uname)
include ../make/Make.$(SYS)
include ../make/Make.$(MACHINE)

JAVAC = javac
JAVAH = javah

ALL =  $(patsubst %.java,%.class,$(wildcard *.java))
ALL += include/mclJNI.h

%.class:    %.java
	$(JAVAC) -d $(JAVA_BUILD_DIR) -Xlint:unchecked $<

include/%.h: %.class %.java
	$(JAVAH) -d include -jni mcl.jni.$* 

CC = g++
LD = g++
RM = rm
LIBEXT = so

CFLAGS = -g -Wall
LINKFLAGS = -Wl,-G -shared $(PNL_LINKFLAGS)

MCLINCS = -I../api -I../include
MCLINCS += -I/usr/lib/jvm/java-6-sun/include/ -I/usr/lib/jvm/java-6-sun/include/linux

include ../Make.files

MCLOBJS = $(patsubst %.o,../%.o,$(MCL_CORE_FILES))

# this is a band-aid
MCLOBJS += $(patsubst %.cc,%.o,$(wildcard ../config/config*.cc))

%.o:    %.cc
	$(CC) $(CFLAGS) $(MCLINCS) -c $< -o $@

all: $(ALL) mcl.lib

mcl.lib: mcl_jni.o
	(cd .. ; make objects)
	$(LD) -o mcl.lib $(LINKFLAGS) $(MCLOBJS) mcl_jni.o

clean:
	-$(RM) *~
	-$(RM) *.class
	-$(RM) *.o
	-$(RM) include/*.h
	-$(RM) *.lib
	-$(RM) *err*.log
