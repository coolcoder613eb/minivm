
OPT ?= -Os

ARCH := x86

SRCS_x86 := vm/main.c vm/jump.c vm/arch/int.c vm/arch/x86.tmp.c
SRCS_int := vm/main.c vm/jump.c vm/arch/int.c

CFLAGS_x86 := -DVM_USE_ARCH_X86
CFLAGS_int := 

SRCS := $(SRCS_$(ARCH))
CFLAGS := $(CFLAGS_$(ARCH)) $(CFLAGS)

default: all

all: bin/minivm bin/minivm-dis bin/minivm-color

bin/luajit-minilua: luajit/src/host/minilua.c | bin
	$(CC) -o $(@) $(^) -lm

vm/arch/x86.tmp.c: vm/arch/x86.dasc | bin/luajit-minilua
	bin/luajit-minilua luajit/dynasm/dynasm.lua -o vm/arch/x86.tmp.c vm/arch/x86.dasc

bin/minivm: $(SRCS) | bin
	$(CC) $(OPT) $(SRCS) -o bin/minivm $(CFLAGS)

bin/minivm-color:
	$(MAKE) -f util/makefile bin/minivm-color

bin/minivm-dis:
	$(MAKE) -f util/makefile bin/minivm-dis

bin: .dummy
	mkdir -p $(@)

.dummy:

clean: .dummy
	rm -f $(OBJS) vm/arch/x86.tmp.c minivm minivm.profdata minivm.profraw vm/*.tmp.c vm/*/*.tmp.c vm/*.o vm/*/*.o
