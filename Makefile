# config 
#-----------------------

executable_name = "checked"
version = 0.0.1
arch ?= x86_64

LINKER = gcc
CC = gcc
ASM = gcc
NASM = nasm

SOURCEDIR = src
BINDIR = bin
BUILDDIR = build
RESOURCESDIR = resources

executable_fullname = $(executable_name)-$(version)-$(arch)
executable = $(BUILDDIR)/$(executable_fullname)

dirs = $(shell find $(SOURCEDIR)/ -type d -print)
includedirs :=  $(sort $(foreach dir, $(foreach dir1, $(dirs), $(shell dirname $(dir1))), $(wildcard $(dir)/include)))

LFLAGS = -lpthread -lutil
LIBRARIES = -lssh

# c
CFLAGS =

# assembly (.s / gcc)
ASMFLAGS = 

# assembly (.asm / nasm)
NASMFLAGS =

#gdb
GDBARGS = 

# setup
#-----------------------

CFLAGS += $(foreach dir, $(includedirs), -I./$(dir))
ASMFLAGS += $(foreach dir, $(includedirs), -I./$(dir))

# support for .S
assembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.S))
assembly_object_files := $(patsubst $(SOURCEDIR)/%.S, \
	$(BUILDDIR)/%.o, $(assembly_source_files))

# support for .s
assembly_source_files += $(foreach dir,$(dirs),$(wildcard $(dir)/*.s))
assembly_object_files += $(patsubst $(SOURCEDIR)/%.s, \
	$(BUILDDIR)/%.o, $(assembly_source_files))


# support for .asm
nassembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.asm))
nassembly_object_files := $(patsubst $(SOURCEDIR)/%.asm, \
	$(BUILDDIR)/%.o, $(nassembly_source_files))

# support for .c files
c_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.c))
c_object_files := $(patsubst $(SOURCEDIR)/%.c, \
    $(BUILDDIR)/%.o, $(c_source_files))


# default rules
#-----------------------

.PHONY: clean run debug build

build: $(executable)
	@mkdir -p $(RESOURCESDIR)

clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(BINDIR)
	@rm -rf $(RESOURCESDIR)

run: build
	@echo starting
	@cd $(RESOURCESDIR)
	@../$(executable)

debug: $(executable)
	@gdb --args $(executable) $(GDBARGS)

# linking
$(executable): $(assembly_object_files) $(c_object_files) $(nassembly_object_files)
	@echo linking...
	@mkdir -p $(BINDIR)
	@$(LINKER) -L/usr/lib $(LFLAGS) -o $(executable) $(assembly_object_files) $(nassembly_object_files) $(c_object_files) $(LIBRARIES)

# compile assembly files (.S)
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.S
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(ASM) -c $(ASMFLAGS) $< -o $@

# compile assembly files (.S)
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(ASM) -c $(ASMFLAGS) $< -o $@


# compile assembly files
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.asm
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(NASM) $(NASMFLAGS) $< -o $@

# compile c files
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@$(CC) -c $(CFLAGS) $< -o $@

