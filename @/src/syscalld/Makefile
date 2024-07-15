
.PHONY:all syscalld

all:syscalld

syscalld:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:syscalld
	cd build && cmake --build . -t install
	systemctl enable syscalld
