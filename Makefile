
.PHONY:all clean

SUBDIRS:=$(shell find . -maxdepth 2 -mindepth 2 -name Makefile | xargs dirname | xargs realpath)

all:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

clean:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clean;)
