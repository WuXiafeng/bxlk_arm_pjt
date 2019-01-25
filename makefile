#makefile
# Add by Wuxiafeng 20181202 first version 
#

#Do not print entry info
MAKEFLAGS += -rR --no-print-directory

CUR_DIR := $(shell pwd)

ROOT_DIR = $(CUR_DIR)
EXLD_DIR := ./ %/include/ %/modules/
SRC_DIR := $(filter-out $(EXLD_DIR), $(sort $(dir $(wildcard ./*/))))

#define the tools, compatible for cross compile
AS		= $(CROSS)as$(SUFFIX)
LD		= $(CROSS)ld$(SUFFIX)
CC		= $(CROSS)$(GNU)cc$(SUFFIX)
CPP		= $(CC) -E
AR		= $(CROSS)ar$(SUFFIX)
NM		= $(CROSS)nm$(SUFFIX)
STRIP		= $(CROSS)strip$(SUFFIX)
OBJCOPY		= $(CROSS)objcopy$(SUFFIX)
OBJDUMP		= $(CROSS)objdump$(SUFFIX)

export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP

#Include files
BXLK_INCLUDE = $(addprefix -I, $(sort $(dir $(wildcard $(CUR_DIR)/*/))))

export BXLK_INCLUDE

#define
DEFINE = 

ifeq ($(TARGET_OS),linux)
DEFINE += -DOS_LINUX
endif

ifeq ($(ARCH),x86)
DEFINE += -DARCH_X86
endif
export DEFINE

#cc flags 
CC_FLAG = -g -W -Wall -Wno-main -Wundef -Wstrict-prototypes -Wno-trigraphs \
		   -fno-strict-aliasing -fno-common \
		   -Werror-implicit-function-declaration \
		   -Wno-format-security \
		   -fno-delete-null-pointer-checks \
		   -fPIC 
		   
export CC_FLAG

#ld flag
LD_FLAG = 

#target
RELE_TAR = $(CUR_DIR)/bxlk_control

#libs 
LIBS = 
LIBS += -lpthread -ldl -lc
export LIBS

#source
CUR_SRC = $(filter  %.c, $(sort $(wildcard $(CUR_DIR)/*)))
ALL_SRC = $(filter-out %/modules/gpio_isr_module.c, $(filter  %.c, $(sort $(wildcard $(CUR_DIR)/* $(CUR_DIR)/*/*))))

#get target object list
#current obj
CUR_OBJ = $(patsubst %.c,%.o,$(CUR_SRC))
#all obj
ALL_OBJ = $(patsubst %.c,%.o,$(ALL_SRC))

all: build_prog
	@echo Build target program successful!

build_prog: cc
	$(CC) -g -o $(RELE_TAR) $(LD_FLAG) $(LIBS) $(ALL_OBJ)
	 
cc: $(SRC_DIR) $(CUR_OBJ)

$(SRC_DIR):
	@echo $@
	$(MAKE) -B -C $@

$(CUR_OBJ):
	$(CC) $(CC_FLAG) $(BXLK_INCLUDE) $(DEFINE) -o $@ -c $(patsubst %.o, %.c, $@)
	
clean:
	rm -rf $(ALL_OBJ) $(RELE_TAR)
        