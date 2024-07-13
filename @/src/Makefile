
.PHONY:all

all:install

SUBDIRS:=$(shell find . -maxdepth 2 -mindepth 2 -name Makefile | xargs dirname | xargs realpath)


install:
	#安装index.cgi
	install -d /var/www/html
	install index.cgi /var/www/html/
	#安装子项目
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) install;)
