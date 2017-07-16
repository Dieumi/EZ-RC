SRC_DIR = src

OBJ_DIR := obj
BIN_DIR := bin
BUILD_DIR := build
DIST_DIR := dist
LIB_DIR := lib
DLL_DIR := dll
INCLUDE_DIR := include

EXECUTABLE := server

SOURCES_C = $(shell find $(SRC_DIR) -name '*.c')
SOURCES = $(SOURCES_C)
OBJECTS_C = $(SOURCES_C:%.c=%.c.o)
OBJECTS = $(OBJECTS_C)

LINK_OBJECTS_C = $(addprefix $(OBJ_DIR)/,$(OBJECTS_C))
LINK_OBJECTS = $(LINK_OBJECTS_C)

CC := gcc
LINKER := gcc

CC_FLAGS := -std=gnu99 -O2 -Wall -Wextra

LINKER_FLAGS := -O2

LIB_PATHS = ../libnet\
			../libdll\
			../irc\

LIBS = 	lib/libnet.a\
		lib/libdll.a\
		dll/irc.so\
		-ldl\
		-pthread\

BUILD_TYPE := debug

all: debug

Debug: debug

Release: release

init:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR) $(DIST_DIR) $(LIB_DIR) $(INCLUDE_DIR) $(DLL_DIR)
	$(foreach LIB_PATH,$(LIB_PATHS),$(MAKE) -C $(LIB_PATH) dist; cp -r $(LIB_PATH)/$(DIST_DIR)/* .;)

build: init $(OBJECTS)
	$(LINKER) -o $(BIN_DIR)/$(EXECUTABLE) $(LINKER_FLAGS) $(LINK_OBJECTS) -L$(LIB_DIR) $(LIBS)

dist: build
	cp -r $(DLL_DIR) $(DIST_DIR)
	cp -r $(BIN_DIR)/* $(DIST_DIR)

run: dist
	cd $(DIST_DIR) && ./$(EXECUTABLE)

debug: dist

release: dist

%.c.o: %.c
	mkdir -p $(shell dirname "$(OBJ_DIR)/$@")
	$(CC) -c $< -o $(OBJ_DIR)/$@ $(CC_FLAGS) -I$(INCLUDE_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR) $(DIST_DIR) $(LIB_DIR) $(INCLUDE_DIR) $(DLL_DIR)
	$(foreach LIB_PATH,$(LIB_PATHS),$(MAKE) -C $(LIB_PATH) clean;)
