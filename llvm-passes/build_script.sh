#!/usr/bin/env bash

if [ ! -d "build" ]; then
  mkdir build
else
  rm -rf build/*
  mkdir build
fi

cd build
cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-3.4 ..
cmake --build .
cd ..
