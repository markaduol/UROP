#!/usr/bin/env bash

if [ ! -d "build" ]; then
  mkdir build
else
  echo "Removing 'build' directory..."
  rm -rf build
  mkdir build
fi

cd build
CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0"
cmake \
  -DCMAKE_BUILD_TYPE=Release\
  -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-3.4 ..
cmake --build .
cd ..
