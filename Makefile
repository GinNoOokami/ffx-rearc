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
LD_SCRIPT  := $(BUILD_DIR)/$(BASENAME).ld
ELF        := $(BUILD_DIR)/$(BASENAME).elf
IMAGE      := $(BUILD_DIR)/$(BASENAME).bin

# Written by splat: symbols referenced at addresses outside every segment, defined at fixed values.
UNDEF_SYMS := $(BUILD_DIR)/undefined_syms_auto.txt $(BUILD_DIR)/undefined_funcs_auto.txt

# ---- Tools ----
CROSS   ?= mips-linux-gnu-
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
SEVENZIP ?= 7z
PYTHON  ?= .venv/bin/python3
SPLAT   := $(PYTHON) -m splat split

AS_FLAGS := -EL -march=r5900 -mabi=eabi -no-pad-sections -I include
LD_FLAGS := -EL

# ---- Inputs: whatever splat has written to asm/<version>/ ----
S_FILES := $(shell find $(ASM_DIR) -name '*.s' 2>/dev/null)
O_FILES := $(patsubst %.s,$(BUILD_DIR)/%.s.o,$(S_FILES))

.PHONY: all check extract split clean distclean
all: check

check: $(IMAGE)
	@echo "$(IMAGE_SHA1)  $(IMAGE)" | sha1sum -c -

split: $(ORIG_IMAGE)
	$(SPLAT) $(SPLAT_YAML)

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

$(ELF): $(O_FILES) $(LD_SCRIPT)
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) $(addprefix -T ,$(UNDEF_SYMS)) -Map $(@:.elf=.map) -o $@

$(IMAGE): $(ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build/$(VERSION)

distclean: clean
	rm -rf asm/$(VERSION)
