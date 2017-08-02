# UROP project on symbolic execution

## Building with Docker

After pulling the docker image with

`docker pull markaduol/urop:latest`

create a container instance from the image as follows

`docker run -i -t markaduol/urop /bin/bash`

The terminal should display

`root@[CONTAINER ID]:/#`

## Building with Vagrant

`vagrant up`

## How to get started

`src` -> drivers for the experiments

`llvm-passes` -> passes

`third_party` -> Third party git repositories which we will test

In project root

  ```
  git submodule init
  git submodule update
  ```

In `llvm-passes` run `./build_script.sh` to build the LLVM pass.

`upb-2` will contain the files for a different revision of the `upb` repository. Copy the `upb` repository to a new directory as follows: (If you see any directories `lib` or `obj` in `third_party/upb-2`, run `make clean` in the directory.)

```
cp -R third_party/upb third_party/upb-2
```

In `upb` (https://github.com/google/upb)
  ```
  export LLVM_COMPILER=clang
  CC=wllvm CFLAGS+=-g make
  cd ../upb-2
  git checkout 1aafd41 // Checkout revision with SHA `1aafd41`
  CC=wllvm CFLAGS+=-g make
  cd ../..
  extract-bc -b third_party/upb/lib/libupb.a  // flag -b gets bitcode (.bc file)
  extract-bc -b third_party/upb-2/lib/libupb.a
  cp third_party/upb/lib/libupb.a.bc libupb1.a.bc
  cp third_party/upb-2/lib/libupb.a.bc libupb2.a.bc
  ```

In the Docker and Vagrant environment, the names of the LLVM toolchain binaries are suffixed with the version number `3.4`. So instead of `opt` and `llvm-link`, write `opt-3.4`, `llvm-link-3.4`.

  ```
  export PASS=llvm-passes/build/functionrename/libFunctionRenamePass.so
  opt -load $PASS -functionrename < libupb2.a.bc > libupb2opt.a.bc
  llvm-link -o libupb.a.bc libupb1.a.bc libupb2opt.a.bc
  ```

To compile the test driver `td1.c`, run
  ```
  klee-clang -g -I third_party/upb -I third_party/upb-2 src/td1.c
  ```
(you can replace `td1.c` with any of the test drivers in `src/`)

If `klee-clang` is not installed, use your `clang` compiler instead
  ```
  clang -g -c -emit-llvm -I third_party/upb -I third_party/upb-2 -I /path/to/klee/include src/td1.c
  ```
  
You can add additional flags such as `--only-output-states-covering-new` to only output test cases covering new code
  ```
  klee -libc=uclibc -link-llvm-lib=libupb.a.bc td1.bc
  ```
