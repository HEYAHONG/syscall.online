
.PHONY:all websocket_to_posix_socket


all:websocket_to_posix_socket

websocket_to_posix_socket:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:websocket_to_posix_socket
	cd build && cmake --build . -t install
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d) install;)
