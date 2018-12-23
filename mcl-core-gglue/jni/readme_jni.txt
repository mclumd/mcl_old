There are a couple of things you need to do to make this stuff all work.

(1) your classpath has to be set correctly
    the package for the MCL/JNI code is "mcl.jni".

    the system-specific makefile in mcl-core-pnl/make/Make.xxxx sets a
    variable JAVA_BUILD_DIR to where the class files will be written. 
    the default is $(HOME), your home directory.

    JAVA_BUILD_DIR can be overridden in the machine-specific makefile
    in mcl-core-pnl/make/Make.yyyy.

    I. example 1 - using the default (HOME) to write class files directly
       into the build directory.

       add $(HOME) to your CLASSPATH. in bash:
       export CLASSPATH=$CLASSPATH:$HOME

       create a symbolic link from mcl to mcl-core-pnl 
       (java doesn't like dashes)
       ln -s $HOME/mcl <wherever you've checked mcl-core-pnl out to>

    II. example 2 - using a common local java directory (how I do it)
	recommended

	add the directory to your classpath. in bash:
	export CLASSPATH=$CLASSPATH:/home/username/local_java

	override JAVA_BUILD_DIR in Make.yyyy (where yyyy= your hostname)
	JAVA_BUILD_DIR = /home/username/local_java/

	see make/Make.acacia

(2) build the mcl-core by going into mcl-core-pnl and running make

(3) build the jni part by going into mcl-core/jni and running make

(4) test the java class mclJNI:

    java mcl.jni.mclJNI

(5) your java controller should use the mclJNI class to interact with
    MCL.

Potentially Frequently Asked Questions:

(A) What happens in the make file?
    
    Four things:
    First, the marsJNI.java file is compiled.
    Second, a C header file is created from the marsJNI.class file.
    Third, a shared library is built. It's called mars.lib, and it contains 
    all the object files in the mars-sim subdirectories, plus 
    mars_main_jni.o from this jni directory.

(B) How do I modify the JNI to mcl?

    You'd modify the functions in mcl_jni.cc if you need to 
    make improvements to the C side handling of java input. Go ahead and modify
    mclJNI or your extension (you can extend marsJNI if you want to) to
    make improvements to the java side control.

    If you need to add native functions, or change the arguments passed
    between them, you need to modify both sides of the interface. First,
    update the "native" function declarations of mclJNI or your extended 
    class. Then, rebuild the JNI header files with make. Now, go into the 
    include directory to find the updated header files and make sure they
    are all implemented correctly in mcl_jni.cc and make again.
