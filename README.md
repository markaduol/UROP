UROP project on symbolic execution.

After pulling the docker image with

`docker pull markaduol/urop:latest`

create a container instance from the image as follows

`docker run -i -t markaduol/urop /bin/bash`

The terminal should display

`root@[CONTAINER ID]:/#`

Navigate to `/home/mark/UROP/lib/re2` and from there run

`make clean`

`CXX=clang++ make`

Do the same in the directory `/home/mark/UROP/lib/re2-2017-06-01` (Note that for this directory, the date tag, currently `2017-06-01`, may change in future versions of this project).

Navigate to `home/mark/UROP/lib/llvm-passes` and from there run `./build_script.sh` to compile and link the LLVM pass.

Navigate to `/home/mark/UROP` and from there run

`make`

Build **musl** by running `CC=wllvm make` in lib/musl. An archive will be generated, the full path is lib/musl/lib/libc.a. Extract the bitcode from this archive by running

`extract-bc -b -l /usr/bin/llvm-link-3.4 libc.a`

The extracted bitcode will be in the same directory and it should be named `libc.a.bc`. Certain files in musl are in assembly code, since of implementation of certain functions is depednent on the target machine architecture. `wllvm` cannot lift assembly code to LLVM bitcode, and so `libc.a.bc` is missing the definitions of certain architecture-dependent functions in the musl library. Need to find a way to link these definitions at runtime, when the bitcode file is being passed to klee, perhaps by using weak linking with another C library.

The LLVM pass can be run

`export PASS=/home/mark/UROP/lib/llvm-passes/build/functionrename/libFunctionRenamePass.so`

`opt-3.4 -load $PASS -functionrename < [input bitcode file] > [output bitcode file]`

If [input bitcode file] is `foo.bc` and was generated from a C file, and the output file is `foo_renamed.bc`, you should be able to link `foo.bc` and `foo_r.bc` without any errors as follows:

`llvm-link-3.4 -o foo_linked.bc foo.bc foo_renamed.bc`
