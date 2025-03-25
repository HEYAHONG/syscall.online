\
.PHONY:all

all:install

SUBDIRS:=$(shell find . -maxdepth 2 -mindepth 2 -name CMakeLists.txt | xargs dirname | xargs realpath)
PWD:=$(shell pwd)

install:
	#生成emscripten配置文件（主要允许使用CACHE）
	PATH=. generate_emscripten_config
	#安装子项目
	$(foreach d, $(SUBDIRS), export EM_CONFIG=${PWD}/emscripten_config; rm -rf  "${d}/build"; mkdir -p "${d}/build" && cd "${d}/build" &&  emcmake cmake -DCMAKE_BUILD_TYPE=Debug ${d} && cmake --build . --target install; )
