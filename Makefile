PROJECT := numbers_station
OUT_DIR := out
OBJ_DIR := $(OUT_DIR)/obj
BIN_DIR := $(OUT_DIR)/bin
ELF := $(BIN_DIR)/$(PROJECT).elf

# The compiler toolchain and PS2SDK are separate inputs. This prevents a stale
# PS2DEV installation from silently selecting non-ARM64 host executables.
NATIVE_PS2DEV ?= $(abspath ../ps2_arm64_toolchain/build/ps2dev)
PS2SDK ?= /usr/local/ps2dev/ps2sdk
PS2_CRT0 ?= $(abspath $(PS2SDK)/../ee/mips64r5900el-ps2-elf/lib/crt0.o)

EE_PREFIX := $(NATIVE_PS2DEV)/ee/bin/mips64r5900el-ps2-elf-
CC := $(EE_PREFIX)gcc
READELF := $(EE_PREFIX)readelf

SOURCES := src/main.c src/application.c src/video.c
OBJECTS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPENDS := $(OBJECTS:.o=.d)

CPPFLAGS := -D_EE -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include
CFLAGS := -std=c11 -G0 -O2 -Wall -Wextra -Wpedantic -MMD -MP
LDFLAGS := -T$(PS2SDK)/ee/startup/linkfile -L$(PS2SDK)/ee/lib \
	-Wl,-zmax-page-size=128
LDLIBS := -ldebug -lgraph -ldma -lc

.PHONY: all clean info verify run iso

all: verify $(ELF)

verify:
	@test "$(shell uname -m)" = "aarch64" || \
		(echo "ERROR: Native build host must be aarch64."; exit 1)
	@test -x "$(CC)" || \
		(echo "ERROR: Native EE compiler not found: $(CC)"; exit 1)
	@"$(CC)" --version >/dev/null
	@file "$(CC)" | grep -q 'ARM aarch64' || \
		(echo "ERROR: EE compiler is not a native AArch64 executable: $(CC)"; exit 1)
	@test -f "$(PS2SDK)/ee/startup/linkfile" || \
		(echo "ERROR: PS2SDK startup linkfile not found under $(PS2SDK)."; exit 1)
	@test -f "$(PS2SDK)/ee/include/debug.h" || \
		(echo "ERROR: PS2SDK debug header not found under $(PS2SDK)."; exit 1)
	@test -f "$(PS2SDK)/ee/lib/libdebug.a" || \
		(echo "ERROR: PS2SDK debug library not found under $(PS2SDK)."; exit 1)
	@test -f "$(PS2SDK)/ee/lib/libgraph.a" || \
		(echo "ERROR: PS2SDK graph library not found under $(PS2SDK)."; exit 1)
	@test -f "$(PS2_CRT0)" || \
		(echo "ERROR: PS2 startup object not found: $(PS2_CRT0)"; exit 1)
	@echo "Native ARM64 PS2 build environment verified."

$(ELF): $(OBJECTS)
	@mkdir -p "$(@D)"
	$(CC) -nostartfiles $(LDFLAGS) -o "$@" "$(PS2_CRT0)" $^ $(LDLIBS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"

info:
	@echo "Host:        $(shell uname -m)"
	@echo "Toolchain:   $(NATIVE_PS2DEV)"
	@echo "Compiler:    $(CC)"
	@echo "PS2SDK:      $(PS2SDK)"
	@echo "Startup:     $(PS2_CRT0)"
	@echo "Output ELF:  $(ELF)"

clean:
	rm -rf "$(OUT_DIR)"

run: all
	@test -n "$(PCSX2)" || \
		(echo "ERROR: Set PCSX2 to the emulator executable."; exit 1)
	"$(PCSX2)" -elf "$(abspath $(ELF))"

iso:
	@echo "ISO packaging is not implemented; use $(ELF) directly."

-include $(DEPENDS)
