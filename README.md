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

Currently, the build process initiated by `home/mark/UROP/Makefile` (i.e: the top-level `Makefile` in this repo) will fail at the linking stage, due to symbol clashes between the two (whole-library) bitcode files generated from the two different revisions of the re2 library. In particular, these linking errors are related to symbols introduced by the compiler for VTable management.
