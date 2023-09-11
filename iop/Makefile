IOP_BIN = dvdmark.irx
IOP_OBJS = dvdmark_iop.o imports.o
IOP_LIBS =

IOP_CFLAGS += -Werror

all: $(IOP_BIN)

clean:
	rm -f $(IOP_BIN) $(IOP_OBJS)

include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal
