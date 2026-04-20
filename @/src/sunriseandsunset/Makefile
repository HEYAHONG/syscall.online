
.PHONY:all index.cgi

all:index.cgi

index.cgi:
	if [ -d build ];then rm -rf build; else true;fi
	mkdir -p  build && cd build && cmake -G "CodeBlocks - Unix Makefiles" ../ && cmake --build .

install:index.cgi
	cd build && cmake --build . -t install
