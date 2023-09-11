EE_BIN = dvdmark.elf
EE_OBJS = dvdmark.o iop/dvdmark.o
EE_LIBS = -lkernel -lpatches

EE_CXXFLAGS = -Werror -pedantic-errors

all: build_irx $(EE_BIN)

build_irx:
	make -C iop

clean:
	rm -f $(EE_OBJS) $(EE_BIN)
	make -C iop clean

run: build_irx $(EE_BIN)
	ps2client execee host:$(EE_BIN)

wsl: build_irx $(EE_BIN)
	$(PCSX2) -elf "$(shell wslpath -w $(shell pwd))/$(EE_BIN)"

emu: build_irx $(EE_BIN)
	$(PCSX2) -elf "$(shell pwd)/$(EE_BIN)"

reset:
	ps2client reset
	ps2client netdump

%.s: %.irx
	@echo "Converting $< to $(basename $(notdir $@)) using bin2s..."
	bin2s $< $@ $(basename $(notdir $@))

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp
