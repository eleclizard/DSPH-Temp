#ToVtk v5.0.026 (05-04-2020)

#=============== Compilation Options ===============
USE_GCC5=NO
USE_DEBUG=NO
USE_FAST_MATH=YES
USE_NATIVE_CPU_OPTIMIZATIONS=NO

LIBS_DIRECTORIES=-L./
LIBS_DIRECTORIES:=$(LIBS_DIRECTORIES) -L../lib/linux_gcc

EXECNAME=ToVTK_linux64
EXECS_DIRECTORY=../bin

ifeq ($(USE_DEBUG), YES)
  CCFLAGS=-c -O0 -g -Wall
else
  CCFLAGS=-c -O3
  ifeq ($(USE_FAST_MATH), YES)
    CCFLAGS+= -ffast-math
  endif
  ifeq ($(USE_NATIVE_CPU_OPTIMIZATIONS), YES)
    CCFLAGS+= -march=native
  endif
endif
CC=g++
CCLINKFLAGS=

#Required for GCC versions >=5.0
ifeq ($(USE_GCC5), YES)
  CCFLAGS+=-D_GLIBCXX_USE_CXX11_ABI=0
  CCLINKFLAGS+=-D_GLIBCXX_USE_CXX11_ABI=0
endif

#=============== Files to compile ===============
OBJXML=JXml.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
OBCOMMON=Functions.o FunctionsGeo3d.o JBinaryData.o JDataArrays.o JException.o JObject.o JOutputCsv.o JPartDataBi4.o JPartDataHead.o JRangeFilter.o JSpaceCtes.o JSpaceEParms.o JSpaceParts.o 
OBCODE=JCfgRun.o main.o

OBJECTS=$(OBJXML) $(OBCOMMON) $(OBCODE)

#=============== DualSPHysics libs to be included ===============
JLIBS=${LIBS_DIRECTORIES} -ljvtklib_64

#=============== CPU Code Compilation ===============
all:$(EXECS_DIRECTORY)/$(EXECNAME)
	rm -rf *.o
ifeq ($(USE_DEBUG), NO)
	@echo "  --- Compiled Release CPU version ---"	
else
	@echo "  --- Compiled Debug CPU version ---"
	mv $(EXECNAME) $(EXECNAME)_debug
endif

$(EXECS_DIRECTORY)/$(EXECNAME):  $(OBJECTS)
	$(CC) $(OBJECTS) $(CCLINKFLAGS) -o $@ $(JLIBS)

.cpp.o: 
	$(CC) $(CCFLAGS) $< 

clean:
	rm -rf *.o $(EXECNAME) $(EXECNAME)_debug



