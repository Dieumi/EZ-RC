DIST_DIR := dist

PROJECTS = libdll libnet irc IRC_server

BUILD_TYPE := debug

all: dist

init:
	mkdir -p $(DIST_DIR)

build: init compile

compile: $(PROJECTS)
	cd $< && make dist

dist: build $(PROJECTS)
	$(foreach PROJECT,$(PROJECTS),$(MAKE) -C $(PROJECT) dist; mkdir -p $(DIST_DIR)/$(PROJECT); cp -r $(PROJECT)/$(DIST_DIR)/* $(DIST_DIR)/$(PROJECT);)

debug: dist
	BUILD_TYPE = debug

release: dist
	BUILD_TYPE = release

run: dist
	cd IRC_server && make run

clean:
	$(foreach PROJECT,$(PROJECTS),$(MAKE) -C $(PROJECT) clean;)
	rm -rf $(DIST_DIR)
