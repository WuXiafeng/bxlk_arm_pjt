#sub makefile
CURDIR := $(shell pwd)

CURSRC := $(filter  %.c, $(sort $(wildcard ./*)))
INCLUDE := $(BXLK_INCLUDE) $(addprefix -I, $(sort $(dir $(wildcard $(CURDIR)/*/))))
CURSRC := $(filter  %.c, $(sort $(wildcard ./*)))
CUROBJ := $(patsubst %.c,%.o,$(CURSRC))
CCFLAG := $(CC_FLAG) $(INCLUDE) $(DEFINE) 

all: $(CUROBJ)
	echo $(CURDIR)
	
$(CUROBJ):
	$(CC) $(CCFLAG) -o $@ -c $(patsubst %.o, %.c, $@)
