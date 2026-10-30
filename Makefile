# Final Fantasy X decompilation.  Usage: make [VERSION=jp_intl] [target]

VERSION ?= jp_intl

# ---- Version block: the only place a version's identity lives ----
ifeq ($(VERSION),jp_intl)
  BASENAME   := SLPS_250.88
  ISO_SHA1   := 6c97681f621716024f9a16f84c0835c06dd4bfd7
  ELF_SHA1   := 7a3943dde429f616afd6ec67c433bdce1c37d41a
  IMAGE_SHA1 := 143a49aad843f367036e7dbfcce3094fbd5b4ca1
else
  $(error Unknown VERSION '$(VERSION)')
endif

# ---- Paths (everything generated sits under the version) ----
ORIG_DIR   := orig/$(VERSION)
CONFIG_DIR := config/$(VERSION)
ASM_DIR    := asm/$(VERSION)
BUILD_DIR  := build/$(VERSION)

# Supply either the disc (Redump-verified ISO, under this fixed name: spaces in file names break
# Make) or the ELF itself.
ISO        := $(ORIG_DIR)/ffx-$(VERSION).iso
ORIG_ELF   := $(ORIG_DIR)/$(BASENAME)

# The load image is the ELF's loadable sections laid out by address (objcopy -O binary):
# the bytes the PS2 copies to RAM at 0x100000. That is what the build must reproduce.
ORIG_IMAGE := $(BUILD_DIR)/orig/$(BASENAME).bin
SPLAT_YAML := $(CONFIG_DIR)/$(BASENAME).yaml
SYMBOL_ADDRS := $(CONFIG_DIR)/symbol_addrs.txt
SPLIT_STAMP := $(ASM_DIR)/split.stamp
LD_SCRIPT  := $(ASM_DIR)/$(BASENAME).ld
ELF        := $(BUILD_DIR)/$(BASENAME).elf
IMAGE      := $(BUILD_DIR)/$(BASENAME).bin

# Written by splat: symbols referenced at addresses outside every segment, defined at fixed values.
UNDEF_SYMS := $(ASM_DIR)/undefined_syms_auto.txt $(ASM_DIR)/undefined_funcs_auto.txt

# ---- Tools ----
CROSS   ?= mips-linux-gnu-
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
SEVENZIP ?= 7z
PYTHON  ?= .venv/bin/python3
VENV_BIN := $(dir $(PYTHON))
SPLAT   := $(PYTHON) -m splat split

# say <color> <mark> <text>: a status line, colored (32 green, 31 red) only on a terminal, so logs
# and objdiff's build output stay plain.
SAY     := say() { if [ -t 1 ]; then printf '\033[%sm%s %s\033[0m\n' "$$1" "$$2" "$$3"; \
	else printf '%s %s\n' "$$2" "$$3"; fi; }; say

AS_FLAGS := -EL -march=r5900 -mabi=eabi -no-pad-sections -I include
LD_FLAGS := -EL

# The original compiler, unpacked into tools/cc/ by tools/fetch_toolchain. Its ee-gcc finds its own
# cc1 and as (bin/../lib/gcc-lib, bin/../ee/bin), so C goes through the original assembler too.
EE_CC_NAME := ee-gcc2.9-991111-01
EE_CC_DIR  := tools/cc/$(EE_CC_NAME)
EE_CC      := $(EE_CC_DIR)/bin/ee-gcc

# The system headers the package lacks (newlib's libc, gcc's own), unpacked by tools/fetch_headers.
SYS_INC    := tools/cc/include
C_FLAGS    := -O2 -G0 -I include -isystem $(SYS_INC)/gcc -isystem $(SYS_INC)/newlib

# ---- Inputs: whatever splat has written to asm/<version>/ ----
# nonmatchings/ holds the functions a C file pulls in with INCLUDE_ASM; they aren't built alone.
S_FILES := $(shell find $(ASM_DIR) -name '*.s' -not -path '*/nonmatchings/*' 2>/dev/null)
C_FILES := $(shell find src -name '*.c' 2>/dev/null)
HASM_FILES := $(shell find src -name '*.s' 2>/dev/null)
O_FILES := $(patsubst %.s,$(BUILD_DIR)/%.s.o,$(S_FILES) $(HASM_FILES)) $(patsubst %.c,$(BUILD_DIR)/%.c.o,$(C_FILES))

.PHONY: all build split-if-stale build-after-split check expected extract split format clean distclean
all: build

# Everything in one command: re-split if splat's inputs changed, then build, check, refresh
# expected/ and objdiff.json. The second pass is a fresh make, so its file lists (found when make
# starts) include what the split just wrote. While a function doesn't match yet, the build stops at
# the link or the sha1 check: the objects are rebuilt, and expected/ keeps the last matching state.
build:
	@$(MAKE) --no-print-directory split-if-stale
	@$(MAKE) --no-print-directory build-after-split

# The two passes as phony goals with an empty recipe (@:), so up-to-date steps print nothing: make
# reports "is up to date" for a file named on its command line.
split-if-stale: $(SPLIT_STAMP)
	@:
build-after-split: objdiff.json expected
	@:

check: $(IMAGE)
	@if echo "$(IMAGE_SHA1)  $(IMAGE)" | sha1sum --status -c -; then \
		$(SAY) 32 "$(IMAGE): OK ✓"; \
	else \
		$(SAY) 31 "$(IMAGE): FAILED ✗ (sha1 differs)"; exit 1; \
	fi

# Target objects for diff tools: a copy of build/<version>/src, taken only after the image matches,
# so every object holds the original code (still asm, or C that matches).
expected: check
	@rm -rf expected/$(BUILD_DIR)
	@mkdir -p expected/$(BUILD_DIR)
	@cp -a $(BUILD_DIR)/src expected/$(BUILD_DIR)/

# Splitting starts from an empty asm/<version>/: every .s found there is built, so a file left from
# an old layout would be linked twice.
split: $(ORIG_IMAGE)
	rm -rf $(ASM_DIR)
	$(SPLAT) $(SPLAT_YAML)
	touch $(SPLIT_STAMP)

# The last split's inputs. Everything splat writes (asm, linker script, undefined-symbol lists, the
# stamp) is under asm/<version>/, so make clean only recompiles; make distclean splits again. The
# image splat reads is order-only: make clean deletes it, and rebuilding it isn't a change.
$(SPLIT_STAMP): $(SPLAT_YAML) $(SYMBOL_ADDRS) $(ORIG_ELF) $(wildcard tools/splat_ext/*.py) | $(ORIG_IMAGE)
	rm -rf $(ASM_DIR)
	$(SPLAT) $(SPLAT_YAML)
	touch $@

objdiff.json: $(SPLAT_YAML) tools/objdiff_config.py
	$(PYTHON) tools/objdiff_config.py $(SPLAT_YAML) $(BUILD_DIR) $@

extract: $(ORIG_ELF)

# The ISO is a prerequisite only if it exists ($(wildcard) is empty otherwise), so a supplied ELF
# without an ISO counts as up to date. 7z exits 0 even when the file isn't in the ISO, so the
# extracted file is checked before it replaces anything.
$(ORIG_ELF): $(wildcard $(ISO))
	@test -f "$(ISO)" || { echo "Supply $(ISO) or $@ (see README)"; exit 1; }
	@echo "$(ISO_SHA1)  $(ISO)" | sha1sum -c -
	$(SEVENZIP) e -so $(ISO) $(BASENAME) > $@.tmp
	@echo "$(ELF_SHA1)  $@.tmp" | sha1sum -c -
	mv $@.tmp $@

$(ORIG_IMAGE): $(ORIG_ELF)
	@echo "$(ELF_SHA1)  $<" | sha1sum -c -
	@mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.s.o: %.s
	@mkdir -p $(@D)
	$(AS) $(AS_FLAGS) -o $@ $<

# The toolchain is an order-only prerequisite (after |): fetched when missing, but its timestamp
# never forces a recompile. The old assembler writes a symbol table modern ld rejects (sh_info
# doesn't count all local symbols); a plain objcopy rewrites it correctly.
$(BUILD_DIR)/%.c.o: %.c | $(EE_CC_DIR) $(SYS_INC)
	@mkdir -p $(@D)
	$(EE_CC) -c $(C_FLAGS) -o $@ $<
	$(OBJCOPY) $@

$(EE_CC_DIR):
	tools/fetch_toolchain $(EE_CC_NAME)

$(SYS_INC):
	tools/fetch_headers

# splat writes, for each C file, the asm files its INCLUDE_ASMs pull in (create_asm_dependencies),
# so a re-split recompiles it. The - skips files not written yet (before the first split).
-include $(patsubst %.c,$(BUILD_DIR)/%.asmproc.d,$(C_FILES))
$(ELF): $(O_FILES) $(LD_SCRIPT)
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) $(addprefix -T ,$(UNDEF_SYMS)) -Map $(@:.elf=.map) -o $@

$(IMAGE): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Formatting: clang-format (.clang-format) for our C, black for our Python; both pinned in
# requirements.txt. Black skips git-ignored paths such as tools/cc/.
format:
	@$(VENV_BIN)/clang-format -i $(shell find src include -name '*.[ch]')
	@$(VENV_BIN)/black -q tools

clean:
	rm -rf build/$(VERSION)

distclean: clean
	rm -rf asm/$(VERSION)
