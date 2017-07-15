SRC_DIR = src

INCLUDE_DIR := include
LIB_DIR := lib
OBJ_DIR := obj
BIN_DIR := bin
BUILD_DIR := build
DIST_DIR := dist
DLL_DIST_DIR := $(DIST_DIR)/dll
LIB_DIST_DIR := $(DIST_DIR)/lib
INCLUDE_DIST_DIR := $(DIST_DIR)/include

EXECUTABLE := irc.so

SOURCES_C = $(shell find $(SRC_DIR) -name '*.c')
SOURCES = $(SOURCES_C)
OBJECTS_C = $(SOURCES_C:%.c=%.c.o)
OBJECTS = $(OBJECTS_C)

LINK_OBJECTS_C = $(addprefix $(OBJ_DIR)/,$(OBJECTS_C))
LINK_OBJECTS = $(LINK_OBJECTS_C)

CC := gcc
LINKER := gcc

CC_FLAGS := -std=gnu99 -O2 -Wall -Wextra

AR_FLAGS := rcs

LIB_PATHS = ../libnet\

LIBS = 	lib/libnet.a\

BUILD_TYPE := debug

all: debug

Debug: debug

Release: release

init:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(BUILD_DIR) $(DIST_DIR) $(DLL_DIST_DIR) $(LIB_DIST_DIR) $(INCLUDE_DIST_DIR)
	$(foreach LIB_PATH,$(LIB_PATHS),$(MAKE) -C $(LIB_PATH) dist; cp -r $(LIB_PATH)/$(DIST_DIR)/* .;)

build: init $(OBJECTS)
	$(LINKER) -shared -o $(BIN_DIR)/$(EXECUTABLE) $(LINKER_FLAGS) $(LINK_OBJECTS) -L$(LIB_DIR) $(LIBS)

dist: build
	cp -r $(BIN_DIR)/* $(DLL_DIST_DIR)
	cp -r $(SRC_DIR)/*.h $(INCLUDE_DIST_DIR)

debug: dist

release: dist

%.c.o: %.c
	mkdir -p $(shell dirname "$(OBJ_DIR)/$@")
	$(CC) -c -fPIC $< -o $(OBJ_DIR)/$@ $(CC_FLAGS) -I$(INCLUDE_DIR)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) $(BUILD_DIR) $(DIST_DIR) $(LIB_DIST_DIR) $(INCLUDE_DIST_DIR) $(INCLUDE_DIR)
