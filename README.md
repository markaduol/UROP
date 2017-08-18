# UROP project on symbolic execution

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

The script `diff-function.py` can be used to examine function changes across revisions of the repository `third_party/upb`. Note that the script is only designed to detect changes of C/C++ functions, and so will likely produce erroneous results if used over a repository that contains lots of non C or C++ code. Run

  ```
  ./diff-function.py -h
  ```
to see usage information.

A sample invocation is as follows

  ```
  ./diff-function.py -r third_party/upb -c -o tmp.csv --commits HEAD~1 HEAD~5 HEAD~8
  ```
* `-r` specifying the target repository
* `-c` to output to a CSV file
* `-o` specifying the output file
* `--commits` specifying the commits to compare. In this case, `HEAD~1` will be compared with `HEAD~5` and `HEAD~5` with `HEAD~8`

A more useful invocation may be

  ```
  ./diff-function.py -r third_party/upb -c -o tmp.csv --commits HEAD --depth 20 --step 2 --show-graph
  ```
* `--depth` specifying the number of consecutive commits to examine, relative to the supplied commit (in this case `HEAD`). Note that when this flag is used, only one commit must be supplied.
* `--step` can only be used if `--depth` is used as well.
* `--show-graph` shows a bar chart detailing the function changes across revisions.

  
## Building with Docker

After pulling the docker image with

`docker pull markaduol/urop:latest`

create a container instance from the image as follows

`docker run -i -t markaduol/urop /bin/bash`

The terminal should display

`root@[CONTAINER ID]:/#`


## Building with Vagrant

`vagrant up`
You can add additional flags such as `--only-output-states-covering-new` to only output test cases covering new code.
