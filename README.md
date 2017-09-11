# UROP project on symbolic execution

## Dependencies

* Linux Ubuntu 14.04 (trusty)

The following ubuntu packages are required

### LLVM Toolchain

```
apt-get update
```
`apt-get install -y` for all the following

* clang-3.4
* llvm-3.4
* llvm-3.4-dev
* llvm-3.4-runtime

### Klee Dependencies

`apt-get install -y` for all the following

* build-essential
* curl
* libcap-dev
* git
* cmake
* libncurses5-dev
* python-minimal
* python-pip
* unzip
* zlib1g-dev
* flex
* bison

You should verify that the version of `cmake --version` is 2.8. This version of CMake is not present in the package repositories for
Ubuntu 16.04 (xenial), which is why it is important to use Ubuntu 14.04. 

Similarly, The LLVM version 3.4 toolchain is also not present in the package repostories Ubuntu 16.04.

So if you decide to not use Ubuntu 14.04, you should build CMake version 2.8 and LLVM 3.4 from source, on your host machine.

The following programs must also be built and installed if not already present on your system. For simplicity, you may wish to build
and install all the following programs within a single directory. For example, the `/data` directory.

```
cd /data
```

* [minisat](https://github.com/stp/minisat)

```
git clone https://github.com/stp/minisat.git
mkdir -p minisat/build
cd minisat/build
cmake -DSTATIC_BINARIES=ON -DCMAKE_INSTALL_PREFIX=/usr/local ../
make
sudo make install
```

* [stp](https://github.com/stp/stp.git)

```
git clone https://github.com/stp/stp.git
cd stp
git checkout tags/2.1.2
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_PYTHON_INTERFACE:BOOL=OFF ../
make
sudo make install
cd ..
ulimit -s unlimited
cd ..
```

**Create Symbolic Links to LLVM binary**

We only create a symbolic link to the binary `llvm-config-3.4` in order to avoid problems when building KLEE with CMake, although if
desired, you can also create symbolic links for all the other LLVM toolchain binaries. Make sure that the path for the symbolic
link, `usr/bin/llvm-config`, is not already present on your system. If it is, instead use `usr/local/bin/llvm-config`. And of course,
perform this check for the other LLVM binaries (`llvm-ar-3.4`, `llvm-nm-3.4`, `llvm-link-3.4`, ...) if you choose to create symbolic
links to them as well.

```
ln -s /usr/bin/llvm-config-3.4 /usr/bin/llvm-config

```

**Install klee and klee-uclibc**

```
git clone https://github.com/klee/klee-uclibc.git
cd klee-uclibc
./configure --make-llvm-lib
make -j2
cd ..
git clone https://github.com/klee/klee.git
cd klee
mkdir build
cd build
cmake -DENABLE_SOLVER_STP=ON \
  -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-3.4 \
  -DENABLE_UNIT_TESTS=OFF \
  -DENABLE_POSIX_RUNTIME=ON \
  -DENABLE_KLEE_UCLIBC=ON \
  -DKLEE_UCLIBC_PATH=/path/to/klee-uclibc \
  -DENABLE_SYSTEM_TESTS=OFF ../
make
```

**Add klee binary to PATH**

Ensure that the `HOME` environment variable is set on your system. If not,

```
export HOME=/where/you/want/your/home/directory/to/be
```

If the file $HOME/.bashrc does not exist

```
touch $HOME/.bashrc
```

Add klee binary to PATH

```
export KLEE_BUILD_DIR=/where/you/built/klee
(echo 'export PATH=$PATH:'${KLEE_BUILD_DIR}'/bin' >> ${HOME}/.bashrc)
(for executable in ${KLEE_BUILD_DIR}/bin/* ; do sudo ln -s ${executable} /usr/bin/`basename ${executable}`; done)
```

Note: If working in the `/data` directory and the above instructions have been followed correctly, 
then `/where/you/built/klee` should be `/data/klee/build`

### [wllvm](https://github.com/travitch/whole-program-llvm)

```
pip install wllvm
```

### Ubuntu Python 3 dependencies

```
apt-get install -y python3-tk python3-pip pkg-config libfreetype6-dev
```

### Python 3 packages

Note that the python scripts in this repository use Python 3 and not Python 2, so even if these packages installed for Python 2, you will still need to install them for Python 3.

```
pip3 install GitPython
pip3 install numpy
pip3 install matplotlib
```

If you have not installed the Ubuntu packages `pkg-config` and `libfreetype6-dev`, `matplotlib` will likely fail to install, stating
that it required the packages `freetype` and `png`

## How to get started


`src` -> files needed by test drivers

`tests` -> drivers for the experiments

`llvm-passes` -> passes

`third_party` -> Third party git repositories which we will test

In `llvm-passes` run `./build_script.sh` to build the LLVM pass.

In project root

  ```
  export LLVM_COMPILER=clang
  make LLVM_VERSION=3.4
  ```

If you created symbolic links `llvm-[toolname] -> llvm-[toolname]-3.4` in the directories `/bin` `/usr/bin` or `/usr/local/bin`, you
should also be able to build the project by running 

  ```
  make
  ```
instead of
  ```
  make LLVM_VERSION=3.4
  ```

(Note: It is not good practice to create your own files in `/bin` - see documentation on Linux directory structure.)

You may also need to change some fixed paths in the Makefile:

1. `KLEE_INCLUDE=/path/to/klee/source/dir/include`
2. `KLEE_BUILD_LIBS=/path/to/klee/build/dir/lib`

Note: Planning on migrating to a build system (CMake) in order to avoid this.

You can also specify the revisions of the third party repository to checkout `third_party/upb`, by specifying the SHA-1 hashes as
environment variables (see the Makefile for the defaults)

  ```
  COMMIT_SHA1=HEAD~1 COMMIT_SHA2=HEAD make
  ```

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

`root@[CONTAINER ID]:/home/urop`

You should immediately set the `HOME` environment variable

`export HOME=/home/urop`

`cd UROP` to go to project root.

## Building with Vagrant

`vagrant up`

If your host machine is running Windows, then in Powershell

`$env:DISPLAY="localhost:0.0"`

Enter the Vagrant virtual machine instance

```
vagrant ssh
cd UROP
```

If your host machine is Windows,

`find . -type f -print | xargs -i dos2unix '{}'`

## Tutorial

This tutorial works in the `vagrant` environment. 
To start off, we use the `diff_function.py` tool to identify interesting revision pairs.

```
./diff_function.py -r third_party/upb --commits master --depth 20 --show-graph
```

In the generated graph, you should see that over all consecutive revision pairs, revision pair `(master~15, master~14)` has the greatest number of modified functions between the two revisions. So this revision pair seems like a good candidate for testing function implementation inconsistencies across the two revisions.

We can print the list of modified functions to standard output as follows

```
./diff_function.py -r third_party/upb --commits master~15 master~14 --show-functions modified
```

We have our revision pair, so now we can compile two different versions of the third party repository, using the two revisions.

If you get an error while running `make`, try omitting the `LLVM_VERSION=3.4` suffix. Also, the `Makefile` is equipped to find the path of your KLEE installation, but if it fails to do so, you may need to change the `KLEE_INCLUDE` and `KLEE_BUILD_LIBS` variables in the `Makefile`.

```
export LLVM_COMPILER=clang
SHA1=master~15 SHA2=master~14 make LLVM_VERSION=3.4
```
Confirm third party repositories checked out correctly

```
git -C third_party/upb diff master~15 HEAD
git -C third_party/upb diff master~14 HEAD
```
Generate test drivers for revision pair `(master~15, master~14)`. Note that in the following command, instead of `-r third_party/upb`, we can also use `-r third_party/upb-2` since the commits `master~15` and `master~14` exist in the history of both repositories and point to the same objects in both repositories.

```
./autogen.py -r third_party/upb --commits master~15 master~14
```
Compile automatically generated tests to LLVM bitcode

```
make autotests
```
Now we can test the modified functions with KLEE using the generated test drivers. To run test `obj/autotd1.bc`

```
cd obj
klee -libc=uclibc -link-llvm-lib=boilerplate.bc -link-llvm-lib=libupb_all.a.bc autotd1.bc
```
View statistics with `klee-stats`

```
klee-stats .
```

## Suggestions for improvement
Compile third party libraries with `-ftest-coverage -fprofile-arcs` flags to generate GCOV files and use GCOV to generate coverage information when running the test drivers with KLEE. It is possible to compile the third party libraries with these flags, but this leads to a number of undefined references in the bitcode archive `libupb_all.a.bc`, such as 

* `llvm_gcda_emit_arcs`
* `llvm_gcda_emit_function` 
* `llvm_gcda_end_file`
* `llvm_gcda_start_file`
* `llvm_gcda_summary_info`
* `llvm_gcov_init`

that cause KLEE to crash.

When compiling the third party libraries with the `-ftest-coverage -fprofile-arcs` flags, multiple `.gcno` files are generated in the directories `third_party/upb/obj/upb` and `third_party/upb-2/obj/upb`, although I am unsure on how to use these files to instrument KLEE and obtain coverage information while running the test drivers.
