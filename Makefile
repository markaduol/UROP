# required
export CC=wllvm
export CFLAGS+=-g
CLANG=clang
GIT=git
FUNCRENAME_PASS=llvm-passes/build/functionrename/libFunctionRenamePass.so
SHELL=/bin/bash

current_dir=$(shell pwd)
# can override
COMMIT_SHA1?="72c333e"
COMMIT_SHA2?="HEAD"
LLVM_VERSION=
LLVM_OPT=opt
LLC=llc
LLVM_LINK=llvm-link
ifeq ($(origin LLVM_VERSION), command line)
	LLVM_OPT=opt-$(LLVM_VERSION)
	LLVM_LINK=llvm-link-$(LLVM_VERSION)
	LLC=llc-$(LLVM_VERSION)
endif
LLVM_LINK_PATH=/usr/bin/llvm-link-$(LLVM_VERSION)

# Variables mandated by GNU
prefix=/usr/local
exec_prefix=$(prefix)
includedir=$(prefix)/include
libdir=$(exec_prefix)
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644

all: submodule-init checkout-ver obj/libupb.o obj/libupb.a.bc obj/boilerplate.o obj/boilerplate.bc $(TEST_BCFILES)

submodule-init:
	@$(GIT) submodule init
	@$(GIT) submodule update

checkout-ver:
	@mkdir -p third_party/upb-2
	@cp -R third_party/upb/. third_party/upb-2/
	@$(GIT) -C third_party/upb checkout $(COMMIT_SHA1)
	@$(GIT) -C third_party/upb-2 checkout $(COMMIT_SHA2)

ARCHIVE1=third_party/upb/lib/libupb.a
ARCHIVE2=third_party/upb-2/lib/libupb.a
ARCHIVE_BC1=$(addsuffix .bc, $(ARCHIVE1))
ARCHIVE_BC1=$(addsuffix .bc, $(ARCHIVE2))

KLEE_INCLUDE=/klee/include
PROJ_INCLUDE=include/
UPB_INCLUDE=third_party/upb
UPB_2_INCLUDE=third_party/upb-2
INCLUDE_PATHS=-I $(KLEE_INCLUDE) -I $(PROJ_INCLUDE) -I $(UPB_INCLUDE) -I $(UPB_2_INCLUDE)

PROJ_SRCFILES=\
	src/concrete.c\
	src/utils.c\
	src/symbolic.c

TEST_SRCFILES=\
	tests/td1.c\
	tests/td2.c\
	tests/td3.c\
	tests/td4.c

PROJ_HFILES=\
	include/concrete.h\
	include/utils.h\
	include/symbolic.h

PROJ_BCFILES=$(patsubst src/%.c,obj/%.bc,$(PROJ_SRCFILES))
TEST_BCFILES=$(patsubst tests/%.c,obj/%.bc,$(TEST_SRCFILES))

$(ARCHIVE1):
	$(MAKE) -C third_party/upb

$(ARCHIVE2):
	$(MAKE) -C third_party/upb-2

$(FUNCRENAME_PASS):
	cd llvm-passes && ./build_script.sh

obj/libupb1.a.bc: $(ARCHIVE1)
	@mkdir -p obj
	@extract-bc -b -o $@ $< || extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/libupb2.a.bc: $(ARCHIVE2)
	@mkdir -p obj
	@extract-bc -b -o $@ $< || extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/libupb2opt.a.bc: obj/libupb2.a.bc $(FUNCRENAME_PASS)
	$(LLVM_OPT) -load $(FUNCRENAME_PASS) -functionrename < $< > $@

obj/libupb.a.bc: obj/libupb1.a.bc obj/libupb2opt.a.bc
	$(LLVM_LINK) -o $@ $^

obj/libupb.o: obj/libupb.a.bc
	$(LLC) -filetype=obj $< -o $@

obj/%.bc: src/%.c
	@mkdir -p $$(dirname $@)
	$(CLANG) -c -g -emit-llvm -o $@ $(INCLUDE_PATHS) $<

obj/%.bc: tests/%.c
	@mkdir -p $$(dirname $@)
	$(CLANG) -c -g -emit-llvm -o $@ $(INCLUDE_PATHS) $<

obj/boilerplate.bc: $(PROJ_BCFILES) $(TEST_BCFILES)
	@mkdir -p $$(dirname $@)
	$(LLVM_LINK) -o $@ $(PROJ_BCFILES)

obj/boilerplate.o: obj/boilerplate.bc
	$(LLC) -filetype=obj $< -o $@

.PHONY: clean

clean:
	rm -rf obj
	rm -rf third_party/upb-2
	@$(GIT) -C third_party/upb checkout origin/master
	rm -rf llvm-passes/build
