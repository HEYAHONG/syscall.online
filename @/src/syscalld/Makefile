
.PHONY:all syscalld

CGISUBDIRS:=$(shell find cgi -maxdepth 2 -mindepth 2 -name Makefile | xargs dirname | xargs realpath)

all:syscalld
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d);)

syscalld:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:syscalld
	cd build && cmake --build . -t install
	systemctl enable syscalld
	$(foreach d, $(CGISUBDIRS), $(MAKE) -C $(d) install;)
