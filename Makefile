# required
export CC=wllvm
export CFLAGS+=-g
GIT=git
FUNCRENAME_PASS=llvm-passes/build/functionrename/libFunctionRenamePass.so
SHELL=/bin/bash

current_dir=$(shell pwd)
# can override
COMMIT_SHA=1aafd41
LLVM_VERSION=3.4
LLVM_LINK=llvm-link-$(LLVM_VERSION)
LLVM_LINK_PATH=/usr/bin/llvm-link-$(LLVM_VERSION)
LLVM_OPT=/usr/bin/opt-$(LLVM_VERSION)

# Variables mandated by GNU
prefix=/usr/local
exec_prefix=$(prefix)
includedir=$(prefix)/include
libdir=$(exec_prefix)
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644

all: checkout-ver obj/libupb.a.bc

checkout-ver:
	@mkdir -p third_party/upb-2
	@cp -R third_party/upb/. third_party/upb-2/
	@$(GIT) -C third_party/upb-2 checkout $(COMMIT_SHA)

ARCHIVE1=third_party/upb/lib/libupb.a
ARCHIVE2=third_party/upb-2/lib/libupb.a
ARCHIVE_BC1=$(addsuffix .bc, $(ARCHIVE1))
ARCHIVE_BC1=$(addsuffix .bc, $(ARCHIVE2))

$(ARCHIVE1):
	$(MAKE) -C third_party/upb

$(ARCHIVE2):
	$(MAKE) -C third_party/upb-2

obj/libupb1.a.bc: $(ARCHIVE1)
	@mkdir -p obj
	@extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/libupb2.a.bc: $(ARCHIVE2)
	@mkdir -p obj
	@extract-bc -b -l $(LLVM_LINK_PATH) -o $@ $<

obj/libupb2opt.a.bc: obj/libupb2.a.bc
	@$(LLVM_OPT) -load $(FUNCRENAME_PASS) -functionrename < $< > $@

obj/libupb.a.bc: obj/libupb1.a.bc obj/libupb2opt.a.bc
	@$(LLVM_LINK) -o $@ $^

clean:
	rm -rf obj
	rm -rf third_party/upb-2
