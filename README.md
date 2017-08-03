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

`src` -> files needed by test drivers

`tests` -> drivers for the experiments

`llvm-passes` -> passes

`third_party` -> Third party git repositories which we will test

In `llvm-passes` run `./build_script.sh` to build the LLVM pass.

In project root

  ```
  git submodule init
  git submodule update
  export LLVM_COMPILER=clang
  make
  ```
In the Docker or Vagrant environments, you would instead run

  ```
  make LLVM_VERSION=3.4
  ```
because the names of the LLVM tools are suffixed with the version number of the LLVM distribution.

The `libupb.a.bc` in the `obj` directory is the bitcode file we're interested in. It contains LLVM bitcode for the 2 revisions of the `upb` library, `third_party/upb` and `third_party/upb-2` respectively.

All the test drivers are in the `test` directory. After running `make`, their LLVM bitcode files will be in the `obj` directory. Run

  ```
  klee -libc=uclibc -link-llvm-lib=obj/libupb.a.bc -link-llvm-lib=obj/boilerplate.bc td1.bc
  ```
to run the test driver `td1.bc` for example.
  
You can add additional flags such as `--only-output-states-covering-new` to only output test cases covering new code.
