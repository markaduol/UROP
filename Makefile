LLVM_VERSION?=3.4
CXX?=clang++
CXXFLAGS?=-g -I /klee/include
LLVM_LINK?=llvm-link-${LLVM_VERSION}
LLVM_LINKFLAGS?=
LLVM_OPT?=opt-${LLVM_VERSION}
# required
RE2_CXXFLAGS?=-std=c++11 -Wall -Wextra -I lib/re2

FUNCRENAME_PASS?=llvm-passes/build/functionrename/libFunctionRenamePass.so

# Variables mandated by GNU.
prefix=/usr/local
exec_prefix=$(prefix)
includedir=$(prefix)/include
libdir=$(exec_prefix)/lib
INSTALL=install
INSTALL_DATA=$(INSTALL) -m 644

all: obj/bitcode/libre2_l.bc

clean:
	rm -rf obj

BCFILE_1=lib/re2/obj/bitcode/libre2.bc
BCFILE_2=lib/re2-2017-06-01/obj/bitcode/libre2.bc
BCFILE_2_RENAMED=obj/bitcode/.libre2_renamed.bc

SRCFILE=src/CEscape_TD.cc

SRC_BCFILE=$(patsubst src/%.cc,obj/bitcode/%.bc,$(SRCFILE))

$(SRC_BCFILE): $(SRCFILE)
	@mkdir -p obj/bitcode
	$(CXX) -c -o $(SRC_BCFILE) -emit-llvm $(RE2_CXXFLAGS) $(CXXFLAGS) $(SRCFILE) 

$(BCFILE_2_RENAMED): $(BCFILE_2)
	@mkdir -p obj/bitcode
	$(LLVM_OPT) -load $(FUNCRENAME_PASS) -functionrename < $(BCFILE_2) > $(BCFILE_2_RENAMED)

obj/bitcode/libre2_l.bc: $(BCFILE_1) $(BCFILE_2_RENAMED) $(SRC_BCFILE)
	@mkdir -p obj/bitcode
	$(LLVM_LINK) -o obj/bitcode/libre2_l.bc $(LLVM_LINKFLAGS) $(BCFILE_1) $(BCFILE_2_RENAMED) $(SRC_BCFILE)
