\
.PHONY:all

all:install

SUBDIRS:=$(shell find . -maxdepth 2 -mindepth 2 -name CMakeLists.txt | xargs dirname | xargs realpath)
PWD:=$(shell pwd)

install:
	#安装子项目
	$(foreach d, $(SUBDIRS), mkdir -p "${d}/build" && cd "${d}/build" &&  emcmake cmake ${d} && cmake --build . --target install; )
