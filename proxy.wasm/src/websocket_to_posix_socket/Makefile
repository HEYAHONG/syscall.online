
.PHONY:all websocket_to_posix_socket

CGISUBDIRS:=$(shell find cgi -maxdepth 2 -mindepth 2 -name Makefile | xargs dirname | xargs realpath)

all:websocket_to_posix_socket
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d);)

websocket_to_posix_socket:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:websocket_to_posix_socket
	cd build && cmake --build . -t install
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d) install;)
