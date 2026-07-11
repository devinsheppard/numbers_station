PROJECT := numbers_station
ELF := $(PROJECT).elf

.PHONY: all verify clean run iso

all: verify
	@test -f src/main.cpp || (echo "ERROR: src/main.cpp is intentionally absent until PS2 toolchain verification succeeds."; exit 1)
	@echo "ERROR: Runtime build rules are blocked until Project 001 toolchain verification passes."
	@exit 1

verify:
	@test -n "$$PS2DEV" || (echo "ERROR: PS2DEV is not set."; exit 1)
	@test -n "$$PS2SDK" || (echo "ERROR: PS2SDK is not set."; exit 1)
	@command -v ee-g++ >/dev/null 2>&1 || (echo "ERROR: ee-g++ not found."; exit 1)
	@command -v ee-gcc >/dev/null 2>&1 || (echo "ERROR: ee-gcc not found."; exit 1)
	@command -v ee-ld >/dev/null 2>&1 || (echo "ERROR: ee-ld not found."; exit 1)
	@test -d "$$PS2SDK" || (echo "ERROR: PS2SDK path does not exist: $$PS2SDK"; exit 1)
	@test -d "$$GSKIT" || (echo "ERROR: GSKIT is not set or path does not exist."; exit 1)
	@echo "Toolchain verification passed."

clean:
	@echo "Cleaning local build outputs."
	@rm -rf build/*
	@rm -f $(ELF)

run: all
	@echo "Run support will be added after $(ELF) builds successfully."

iso:
	@echo "ISO creation is intentionally blocked for Project 001."
	@exit 1

