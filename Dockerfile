# escape=\ (backslash)

FROM ubuntu:14.04

ENV LLVM_VERSION=3.4 \
    UROP_SRC=/home/mark/UROP

# We use layered RUN instructions in order frequently commit the container state during a build.

# Install LLVM toolchain
RUN set -xe && \
  apt-get update && \
  apt-get -y install \
    clang-${LLVM_VERSION} \
    llvm-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-dev \
    llvm-${LLVM_VERSION}-runtime

# Install klee dependencies
RUN set -xe && \
  apt-get update && \
  apt-get install -y \
    build-essential \
    curl \
    libcap-dev \
    git \
    cmake \
    libncurses5-dev \
    python-minimal \
    python-pip \
    unzip \
    zlib1g-dev \
    flex \
    bison

# Install minisat
RUN set -xe && \
  git clone https://github.com/stp/minisat.git && \
  mkdir -p minisat/build && \
  cd minisat/build && \
  cmake -DSTATIC_BINARIES=ON -DCMAKE_INSTALL_PREFIX=/usr/local ../ && \
  make && \
  sudo make install

# Install stp
RUN set -xe && \
  git clone https://github.com/stp/stp.git && \
  cd stp && \
  git checkout tags/2.1.2 && \
  mkdir build && \
  cd build && \
  cmake -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_PYTHON_INTERFACE:BOOL=OFF ../ && \
  make && \
  sudo make install && \
  cd ../ && \
  ulimit -s unlimited && \
  cd ../ 

# Install klee
RUN set -xe && \
  git clone https://github.com/klee/klee.git && \
  cd klee && \
  mkdir build && \
  cd build && \
  cmake -DENABLE_SOLVER_STP=ON \
    -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-${LLVM_VERSION} \
    -DENABLE_UNIT_TESTS=OFF \ 
    -DENABLE_SYSTEM_TESTS=OFF ../

# Add relevant files
RUN set -xe && \
  mkdir -p ${UROP_SRC}

COPY / ${UROP_SRC}
