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

In `upb` (https://github.com/google/upb)

  ```
  export LLVM_COMPILER=clang
  CC=wllvm CFLAGS+=-g make
  extract-bc -b lib/libupb.a  // flag -b gets bitcode (.bc file). You may need '--linker' flag
  cp lib/libupb.a.bc ../../libupb1.a.bc
  make clean
  git checkout 1aafd41 // Checkout commit '1aafd41' in [DETACHED_HEAD](https://git-scm.com/docs/git-checkout) state
  CC=wllvm CFLAGS+=-g make
  extract-bc -b lib/libupb.a
  cp lib/libupb.a.bc ../../libupb2.a.bc
  make clean
  cd ../..
  ```

You may need to use the `--linker` flag of `extract-bc`, passing in the path of your `llvm-link` binary. In the Docker and Vagrant environments, the path is `/usr/bin/llvm-link-3.4`.

  ```
  extract-bc -b --linker /usr/bin/llvm-link-3.4 [archive]
  ```
  
If you would rather not use the `--linker` flag, create a symbolic link `ln -s /usr/bin/llvm-link-3.4 /usr/bin/llvm-link`.

In the Docker and Vagrant environments, the names of the LLVM toolchain binaries are suffixed with the version number `3.4`. So instead of `opt` and `llvm-link`, write `opt-3.4`, `llvm-link-3.4`.

  ```
  export PASS=llvm-passes/build/functionrename/libFunctionRenamePass.so
  opt -load $PASS -functionrename < libupb2.a.bc > libupb2opt.a.bc
  llvm-link -o libupb.a.bc libupb1.a.bc libupb2opt.a.bc
  ```

To compile the test driver `td1.c`, run
  ```
  klee-clang -g -I third_party/upb -I src/td1.c
  klee-clang -g -I third_party/upb -I src/boilerplate.c
  ```
(you can replace `td1.c` with any of the test drivers in `src/`)

If `klee-clang` is not installed, use your `clang` compiler instead
  ```
  clang -g -c -emit-llvm -I third_party/upb -I /path/to/klee/include src/td1.c src/boilerplate.c
  ```
  
You can add additional flags such as `--only-output-states-covering-new` to only output test cases covering new code
  ```
  klee -libc=uclibc -link-llvm-lib=libupb.a.bc -link-llvm-lib=boilerplate.bc td1.bc
  ```
