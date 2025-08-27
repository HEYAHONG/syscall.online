
.PHONY:all modbus_tcp_tiny_server


all:modbus_tcp_tiny_server

modbus_tcp_tiny_server:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:modbus_tcp_tiny_server
	cd build && cmake --build . -t install
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d) install;)
