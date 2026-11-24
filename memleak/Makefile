BIN := main
ifeq ($(OS), Windows_NT)
BIN := main.exe
endif

LIBNAME:=  $(notdir $(shell echo $(PWD) | tr " " "@"))

SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
BUILD_DIR := build

SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS := $(patsubst %.c,%.obj,$(SRCS))
ifeq ($(OS), Windows_NT)
OBJS += $(notdir $(wildcard $(LIB_DIR)/*.lib))
STATICNAME:=$(addsuffix .lib,$(LIBNAME))
SHAREDNAME:=$(addprefix lib, $(addsuffix .dll,$(LIBNAME)))
else
OBJS += $(notdir $(wildcard $(LIB_DIR)/*.a))
STATICNAME:=$(addsuffix .a,$(LIBNAME))
SHAREDNAME:=$(addprefix lib, $(addsuffix .so,$(LIBNAME)))
endif

CC := gcc
CFLAGS := -Wall
ifeq ($(DEBUG),1)
CFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -s -O2
endif

.PHONY: all build run lib clean

all: build run

build: $(BUILD_DIR)/$(BIN)

run: $(BUILD_DIR)/$(BIN)
	./$(BUILD_DIR)/$(BIN)

lib: $(addprefix $(BUILD_DIR)/,$(filter-out main%, $(OBJS)))
	ar rcs $(STATICNAME) $^
	$(CC) $(CFLAGS) -shared $^ -o $(SHAREDNAME)

clean:
	rm -f -R $(BUILD_DIR)
	rm -f $(STATICNAME)
	rm -f $(SHAREDNAME)

$(BUILD_DIR)/$(BIN): $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(CC) $(CFLAGS) $^ -o $@

ifeq ($(OS), Windows_NT)
$(BUILD_DIR)/%.obj: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@
else
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@
endif
