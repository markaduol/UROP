# required
export CC=wllvm
export CFLAGS+=-g
export CXXFLAGS+=-g
CLANG=clang
GIT=git
FUNCRENAME_PASS=llvm-passes/build/functionrename/libFunctionRenamePass.so
SHELL=/bin/bash
GCC=gcc

current_dir=$(shell pwd)
# can override

SHA1?="master~4"
SHA2?="master~3"
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

all: submodule-init checkout-ver obj/libupb_all.o obj/libupb_all.a.bc obj/boilerplate.o obj/boilerplate.bc

submodule-init:
	@$(GIT) submodule init
	@$(GIT) submodule update

checkout-ver:
	@mkdir -p third_party/upb-2
	$(GIT) clone -l --no-hardlinks third_party/upb third_party/upb-2
	$(GIT) -C third_party/upb checkout $(SHA1)
	$(GIT) -C third_party/upb-2 checkout master # Need to first checkout 'master' branch on clone
	$(GIT) -C third_party/upb-2 checkout $(SHA2)
	@echo 'upb/*/** -diff' >> third_party/upb/.gitattributes
	@echo 'upb/*/** -diff' >> third_party/upb-2/.gitattributes

ARCHIVE1=third_party/upb/lib/libupb.a
ARCHIVE2=third_party/upb-2/lib/libupb.a

klee_dir=$(shell find / -name "klee" -type d -print -quit 2>/dev/null | head -n1)
KLEE_INCLUDE=$(klee_dir)/include
KLEE_BUILD_LIBS=$(klee_dir)/build/lib/
PROJ_INCLUDE=include/
UPB_INCLUDE=third_party/upb
UPB_2_INCLUDE=third_party/upb-2
INCLUDE_PATHS=-I $(KLEE_INCLUDE) -I $(PROJ_INCLUDE) -I $(UPB_INCLUDE) -I $(UPB_2_INCLUDE)

PROJ_SRCFILES=\
	src/utils.c\

TEST_SRCFILES=\
	tests/td1.c\
	tests/td2.c\
	tests/td3.c\
	tests/td4.c\
	tests/td5.c\
	tests/td6.c

PROJ_HFILES=\
	include/utils.h\

PROJ_BCFILES=$(patsubst src/%.c, obj/%.bc, $(PROJ_SRCFILES))
TEST_BCFILES=$(patsubst tests/%.c, obj/%.bc, $(TEST_SRCFILES))
TEST_OUTFILES=$(patsubst tests/%.c, obj/%.out, $(TEST_SRCFILES))

# See 'upb' Makefile (TODO: IMPORTANT - If the interface of the 'upb' Makefile breaks,
# everthing from this point onwards will also break.)
UPB_MODULES=upb upb.pb upb.json upb.descriptor
UPB_LIBS=$(patsubst %, third_party/upb/lib/lib%.a, $(UPB_MODULES))
UPB2_LIBS=$(patsubst %, third_party/upb-2/lib/lib%.a, $(UPB_MODULES))

UPB_LIBS_BC=$(patsubst %, obj/lib%1.a.bc, $(UPB_MODULES))
UPB2_LIBS_RENAMED_BC=$(patsubst %, obj/lib%2opt.a.bc, $(UPB_MODULES))

$(UPB_LIBS):
	$(MAKE) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) Q= -C third_party/upb

$(UPB2_LIBS):
	$(MAKE) CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS) Q= -C third_party/upb-2

$(FUNCRENAME_PASS):
	cd llvm-passes && ./build_script.sh


##### UPB Libs ####

obj/lib%1.a.bc: third_party/upb/lib/lib%.a $(UPB_LIBS)
	@mkdir -p obj
	@extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/lib%2.a.bc: third_party/upb-2/lib/lib%.a $(UPB2_LIBS)
	@mkdir -p obj
	@extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/lib%2opt.a.bc: obj/lib%2.a.bc $(FUNCRENAME_PASS)
	$(LLVM_OPT) -load $(FUNCRENAME_PASS) -functionrename < $< > $@

obj/libupb_all.a.bc: $(UPB_LIBS_BC) $(UPB2_LIBS_RENAMED_BC)
	$(LLVM_LINK) -o $@ $^

obj/libupb_all.o: obj/libupb_all.a.bc
	$(LLC) -filetype=obj $< -o $@



#### Test drivers and Helper C files ####

obj/%.bc: src/%.c 
	@mkdir -p $$(dirname $@)
	@echo "KLEE Directory: $(klee_dir)"
	$(CLANG) -c -g -emit-llvm -o $@ $(INCLUDE_PATHS) $<

obj/%.bc: tests/%.c 
	@mkdir -p $$(dirname $@)
	@echo "KLEE Directory: $(klee_dir)"
	$(CLANG) -c -g -emit-llvm -o $@ $(INCLUDE_PATHS) $<

obj/boilerplate.bc: $(PROJ_BCFILES)
	@mkdir -p $$(dirname $@)
	$(LLVM_LINK) -o $@ $(PROJ_BCFILES)

obj/boilerplate.o: obj/boilerplate.bc
	$(LLC) -filetype=obj $< -o $@

obj/%.out: tests/%.c obj/boilerplate.o obj/libupb_all.o
	@mkdir -p $$(dirname $@)
	@echo "KLEE Directory: $(klee_dir)"
	$(GCC) -L $(KLEE_BUILD_LIBS) -fprofile-arcs -ftest-coverage $(INCLUDE_PATHS) obj/boilerplate.o obj/libupb.o $< -o $@ -lkleeRuntest

obj/autotd%.bc: autotd%.c
	@mkdir -p $$(dirname $@)
	@echo "KLEE Directory: $(klee_dir)"
	$(CLANG) -c -g -emit-llvm -o $@ $(INCLUDE_PATHS) $<

.PHONY: clean submodule-clean td-clean

submodule-clean:
	@$(MAKE) -C third_party/upb clean
	@$(GIT) -C third_party/upb checkout master
	rm -f third_party/upb/.gitattributes

td-clean:
	rm -f autotd*
	rm -f obj/autotd*

clean:
	rm -rf obj
	rm -rf third_party/upb-2
	@$(MAKE) submodule-clean
	rm -rf llvm-passes/build
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f *.csv
	@$(MAKE) td-clean
