ARCHITECTURE		:= x86_64-linux
ARCH_EXT			= $(ARCHITECTURE)

ifeq "$(ARCH_EXT)" "i686-linux"
	LINUX___		= 1
endif

ifeq "$(ARCH_EXT)" "x86_64-linux"
	LINUX___		= 1
endif

ifdef LINUX___
	ARCH			= linux
	CC			= g++ -m64 -std=c++11
	DEFINES			= -D__LINUX__ -D__UNIX__ -Wno-write-strings
	LINK			= -Wl,-g -lstdc++ 
	RELEASE			= -Wall -O3 -Wno-unused-function
	DEBUG			= -g -Wall -O0
endif

OUT                 = bin/${ARCH}/release/
OUT_DEBUG           = bin/${ARCH}/debug/
MAIN                = h2xml
BINDIR				= ../../bin/linux/


INCLUDES += /usr/include/x86_64-linux-gnu
INCLUDES += ../../src
INCLUDES += ../../src/compiler 
INCLUDES += ../../src/libraries
INCLUDE_FLAGS = $(addprefix -I, $(INCLUDES))

OBJECTS  += $(addprefix $(OUT), $(notdir $(patsubst %.cpp, %.o, $(wildcard ../../src/*.cpp))))
OBJECTS  += $(addprefix $(OUT), $(notdir $(patsubst %.cpp, %.o, $(wildcard ../../src/compiler/*.cpp))))
OBJECTS  += $(addprefix $(OUT), $(notdir $(patsubst %.cpp, %.o, $(wildcard ../../src/libraries/*.cpp))))

CCFLAGS   = $(RELEASE) $(INCLUDE_FLAGS) $(DEFINES) 
CCFLAGS_DEBUG   = $(DEBUG) $(INCLUDE_FLAGS) $(DEFINES) 

VPATH = ../../src ../../src/compiler ../../src/libraries
SRC_DIR = ../../src

help:
	@echo ""
	@echo " h2xml targets:"
	@echo ""
	@echo "   release:      release version"
	@echo "   debug:        debug version"
	@echo "   clean:        delete intermediate files"
	@echo ""

release: compilerLexyy.cpp scanAnnotationsLexyy.cpp scanConfigLexyy.cpp compilerYaccyy.cpp
	@echo "Building release version..."
	@-mkdir -p $(OUT)
	$(MAKE) $(MAIN)
	@-mkdir -p $(BINDIR)
	@-chmod 755 $(BINDIR)$(MAIN)
	cp $(OUT)$(MAIN) $(BINDIR)$(MAIN)
	objdump -T $(OUT)$(MAIN) 

debug: compilerLexyy.cpp scanAnnotationsLexyy.cpp scanConfigLexyy.cpp compilerYaccyy.cpp
	@echo "Building debug version..."
	@-mkdir -p $(OUT_DEBUG)
	$(MAKE) OUT=$(OUT_DEBUG)  CCFLAGS="$(CCFLAGS_DEBUG)" $(MAIN)

compilerYaccyy.cpp: compiler.yacc
	bison -v -d -o $(SRC_DIR)/compiler/compilerYaccyy.cpp $(SRC_DIR)/compiler/compiler.yacc

compilerLexyy.cpp: compiler.lex
	flex -d -Pyycompiler -o $(SRC_DIR)/compiler/compilerLexyy.cpp $(SRC_DIR)/compiler/compiler.lex

scanAnnotationsLexyy.cpp: scanAnnotations.lex
	flex -d -Pyyannot -o $(SRC_DIR)/scanAnnotationsLexyy.cpp $(SRC_DIR)/scanAnnotations.lex

scanConfigLexyy.cpp: scanConfig.lex
	flex -d -Pyyconf -o $(SRC_DIR)/scanConfigLexyy.cpp $(SRC_DIR)/scanConfig.lex

$(MAIN): $(OBJECTS)
	@echo "(LD)" $@
	${CC} ${CCFLAGS} $(OBJECTS) -o ${OUT}$@ $(LINK)

$(OUT)%.o: %.cpp
	@echo "(CC)" $?
	${CC} -o $@ ${CCFLAGS} -c $?

clean:
	${RM} $(OUT)*
	${RM} $(OUT_DEBUG)*
	${RM} $(SRC_DIR)/*Lexyy*
	${RM} $(SRC_DIR)/compiler/*Lexyy*
	${RM} $(SRC_DIR)/compiler/*Yaccyy*

 # ldd -v ./h2xml   show dynamic dependencies
 # objdump -T ./h2xml show where dependencies come from
 # objdump -dSr ./h2xml | less list assembly with symbols  


