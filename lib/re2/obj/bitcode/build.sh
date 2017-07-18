#!/usr/bin/env bash

# Script to run pass over individual bitcode files, if unable to run llvm pass over bitcode archive

LLVM_FUNCRENAME_PASS="/data/UROP/lib/llvm-passes/build/functionrename/libFunctionRenamePass.so"

# Get name of bitcode archive

if [ ! -f "$(find -name "*.bca")" ]; then
  echo "Bitcode archive not found"
  exit 1
fi

ARCHIVE="$(find -name "*.bca")"
BCA_FILENAME=$(basename "$ARCHIVE")
BCA_EXTENSION="${BCA_FILENAME##*.}"
BCA_FILENAME="${BCA_FILENAME%.*}"

BUILD_DIR="renamed"

if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
else
  rm -rf "$BUILD_DIR/*"
fi

RENAMED_ARCHIVE="${BCA_FILENAME}_renamed.${BCA_EXTENSION}"
LINKED_ARCHIVE="${BCA_FILENAME}_linked.${BCA_EXTENSION}"
llvm-ar cr "${BUILD_DIR}/${RENAMED_ARCHIVE}"
llvm-ar cr "${LINKED_ARCHIVE}"

for i in \.*.bc; do
  [ -f "$i" ] || break
  filename=$(basename "$i")
  extension="${filename##*.}"
  filename="${filename%.*}"
  echo "Running pass over file "$i""
  opt -load $LLVM_FUNCRENAME_PASS -functionrename < "$i" > "${BUILD_DIR}/${filename}_renamed.bc"
  llvm-ar r "${BUILD_DIR}/$RENAMED_ARCHIVE" "${BUILD_DIR}/${filename}_renamed.bc"
  
  # Add both bitcode files to linked archive
  echo "Adding files to linked archive"
  llvm-ar r "${LINKED_ARCHIVE}"  "$i" "${BUILD_DIR}/${filename}_renamed.bc"
done

