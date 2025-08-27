
.PHONY:all

all:install

SUBDIRS:=$(shell find . -maxdepth 2 -mindepth 2 -name Makefile | xargs dirname | xargs realpath)


install:
	#安装子项目
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) install;)
	@cp -rf s6-service /etc/ 
