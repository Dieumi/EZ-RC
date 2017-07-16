SRC_DIR = src

OBJ_DIR := obj
BIN_DIR := bin
BUILD_DIR := build
DIST_DIR := dist
LIB_DIST_DIR := $(DIST_DIR)/lib
INCLUDE_DIST_DIR := $(DIST_DIR)/include

EXECUTABLE := libdll.a

SOURCES_C = $(shell find $(SRC_DIR) -name '*.c')
SOURCES = $(SOURCES_C)
OBJECTS_C = $(SOURCES_C:%.c=%.c.o)
OBJECTS = $(OBJECTS_C)

LINK_OBJECTS_C = $(addprefix $(OBJ_DIR)/,$(OBJECTS_C))
LINK_OBJECTS = $(LINK_OBJECTS_C)

CC := gcc
AR := ar

CC_FLAGS := -std=gnu99 -O2 -Wall -Wextra

AR_FLAGS := rcs

LIBS = -ldl\

BUILD_TYPE := debug

all: debug

Debug: debug

Release: release

init:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR) $(DIST_DIR) $(LIB_DIST_DIR) $(INCLUDE_DIST_DIR)

build: init $(OBJECTS)
	$(AR) $(AR_FLAGS) $(BIN_DIR)/$(EXECUTABLE) $(LINK_OBJECTS)

dist: build
	cp -r $(BIN_DIR)/* $(LIB_DIST_DIR)
	cp -r $(SRC_DIR)/*.h $(INCLUDE_DIST_DIR)

debug: dist

release: dist

%.c.o: %.c
	mkdir -p $(shell dirname "$(OBJ_DIR)/$@")
	$(CC) -c $< -o $(OBJ_DIR)/$@ $(CC_FLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR) $(DIST_DIR) $(LIB_DIST_DIR) $(INCLUDE_DIST_DIR)
