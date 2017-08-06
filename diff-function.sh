#!/usr/bin/env bash

set -e

COMMIT_SHA1=undefined
COMMIT_SHA2=origin/master #default

if [[ $# -ne 2 ]]; then
  echo "Please supply exactly 2 arguments."
  exit 1
fi

for sha in "$@"
do
  if git cat-file -e $sha^{commit}; then
    echo "Commit identifier $sha exists"
  else
    echo "Invalid commit identifier: $sha"
    exit 1
  fi
done

i=0
while [[ $# -gt 0 ]]
do
  key="$1"

  if [[ $i == 0 ]]; then
    COMMIT_SHA1="$key"
  else
    COMMIT_SHA2="$key"
  fi
  shift
  i=$((i+1))
done

if [[ $? -ne 0 ]]; then
  echo "Error occurred"
  exit 1
else
  echo "Commit SHA 1: $COMMIT_SHA1"
  echo "Commit SHA 2: $COMMIT_SHA2"
fi

git diff --function-context $COMMIT_SHA1 $COMMIT_SHA2 | \
  grep -E '^(@@)' | \
  grep "(" | \
  sed 's/@@.*@@//' | \
  sed 's/(.*//' | \
  awk -F " " '{print $NF}' | \
  uniq
