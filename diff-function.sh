#!/usr/bin/env bash

set -e

COMMIT_SHA1=undefined
COMMIT_SHA2=origin/master #default

if [[ $# -lt 1 ]]; then
  echo "Please supply 1 or 2 arguments."
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

args_counter=0
while [[ $# -gt 0 ]]
do
  key="$1"

  if [[ $args_counter == 0 ]]; then
    COMMIT_SHA1="$key"
  else
    COMMIT_SHA2="$key"
  fi
  shift
  args_counter=$((args_counter+1))
done

if [[ $? -ne 0 ]]; then
  echo "Error occurred"
  exit 1
else
  echo "Commit SHA 1: $COMMIT_SHA1"
  echo "Commit SHA 2: $COMMIT_SHA2"
fi

# First disjunct is for diff between changes in the working tree and HEAD of local repository
# We disregard changes in submodules
( [[ args_counter -eq 1 ]] && git diff -W --ignore-submodules $COMMIT_SHA1 || git diff -W --ignore-submodules $COMMIT_SHA1 $COMMIT_SHA2 ) | \
awk '!/if/ && !/switch/ && !/return/ && !/for/ && !/\/\// && !/\/\*/ && !/#/ && !/while/' | \
awk '/[a-zA-Z_](\*)?(\s)+(\*)?[a-zA-Z0-9_]*\(.*\)/' | \
awk '!/;$/' | \
awk '/\(/' | \
sed 's/@@.*@@//' | \
sed 's/(.*//' | \
uniq
