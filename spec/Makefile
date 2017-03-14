CC := gcc
CFLAGS := -std=c99 -pedantic -Wall -Wpointer-arith
# LD - Linker for bundling object files into executable.
LD := gcc
# LDLIBS - Libraries to link with.
LDLIBS := -lm

MODULES := 
SRC_DIR := ../src #$(addprefix src/,$(MODULES))
BUILD_DIR := build #$(addprefix build/,$(MODULES))

# $(foreach var, list, text)
# Generate a list of new values by operating on each value in input list.
SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
SRC := $(filter-out ../src/main.c, $(SRC))
SRC += spec.c

# $(patsubst pattern, replacement, text)
# Find whitespace-separated words in text which match pattern and replace them.
# NOTE: % acts as a wildcard.
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

# $(addprefex, prefix, names...)
# Prepend prefix to each name is list of names.
INCLUDES := $(addprefix -I,$(SRC_DIR))
INCLUDES += minunit.h

# VPATH and vpath (latter is more specific) provide lists of directories
# in which to search for missing source files.
vpath %.c $(SRC_DIR)

# Multline variable syntax.
# Define targets / dependencies dynamically.
define make-goal
$1/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $$< -o $$@
endef

# By default, Makefile assumes targets are files. By labeling a target as
# PHONY, we're indicating that the target (e.g. commands like all or clean)
# does not represent a physical file in the file system. PHONY targets are
# treated like files that are always out of date - i.e. they will always
# execute.
.PHONY: all checkdirs clean

all: checkdirs build/spec.exe

build/spec.exe: $(OBJ)
	$(LD) $^ $(LDLIBS) -o $@

checkdirs: $(BUILD_DIR)

# NOTE: -p flag will create nested directories if they do not already exist.
$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

# Loop through build directories and check corresponding rules.
$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
