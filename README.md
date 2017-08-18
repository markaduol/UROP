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

Dependencies: LLVM Version 3.4, [KLEE](klee.github.io) (>= 1.3.0) Symbolic Execution Engine, [wllvm](https://github.com/travitch/whole-program-llvm), python3, GitPython package for python3.

See Dockerfile or Vagrantfile for general installation procedure on Ubuntu/Linux.

`src` -> files needed by test drivers

`tests` -> drivers for the experiments

`llvm-passes` -> passes

`third_party` -> Third party git repositories which we will test

In `llvm-passes` run `./build_script.sh` to build the LLVM pass.

In project root

  ```
  export LLVM_COMPILER=clang
  make
  ```
In the Docker or Vagrant environments, instead of `make`, run

  ```
  make LLVM_VERSION=3.4
  ```
because the names of the LLVM tools are suffixed with the version number of the LLVM distribution.

You may also need to change some fixed paths in the Makefile such as `KLEE_INCLUDE=/klee/include` if you
are not using the Vagrant or Docker environments. Note: Planning on migrating to a build system (CMake) in order to avoid this.

The `libupb.a.bc` in the `obj` directory is the bitcode file we're interested in. It contains LLVM bitcode for the 2 revisions of the `upb` library, `third_party/upb` and `third_party/upb-2` respectively.

All the test drivers are in the `test` directory. After running `make`, their LLVM bitcode files will be in the `obj` directory. Run

  ```
  klee -libc=uclibc -link-llvm-lib=obj/libupb.a.bc -link-llvm-lib=obj/boilerplate.bc obj/td1.bc
  ```
to run the test driver `td1.bc` for example.
  
You can add additional flags such as `--only-output-states-covering-new` to only output test cases covering new code.
