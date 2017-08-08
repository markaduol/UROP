#!/usr/bin/env bash

set -e

COMMIT_SHA1=
COMMIT_SHA2=origin/master #default
DIFF_PATH=

die () {
  echo >&2 "$@"
  exit 1
}

if [[ $# -lt 1 ]]; then
  die "Usage: $0 <commit-sha> [<commit-sha>] [path]\n"
fi

for sha in "${@:1:2}"
do
  if git cat-file -e $sha^{commit}; then
    echo "Commit identifier $sha exists"
  else
    echo "Invalid commit identifier: $sha"
    die "Usage: $0 <commit-sha> [<commit-sha>] [path]\n"
  fi
done

args_counter=0
while [[ $# -gt 0 ]]
do
  key="$1"

  if [[ $args_counter == 0 ]]; then
    COMMIT_SHA1="$key"
  elif [[ $args_counter == 1 ]]; then
    COMMIT_SHA2="$key"
  else
    DIFF_PATH="$key"
  fi
  shift
  args_counter=$((args_counter+1))
done

if [[ $? -ne 0 ]]; then
  die "Error occurred"
else
  echo "Commit SHA 1: $COMMIT_SHA1"
  echo "Commit SHA 2: $COMMIT_SHA2"
  echo "Diff Path: $DIFF_PATH"
fi

# First disjunct is for diff between changes in the working tree and HEAD of local repository
# We disregard changes in submodules
( [[ args_counter -eq 1 ]] && git diff -W --ignore-submodules $COMMIT_SHA1 -- '*.c' || \
  [[ args_counter -eq 2 ]] && git diff -W --ignore-submodules $COMMIT_SHA1 $COMMIT_SHA2 -- '*.c' || \
  [[ args_counter -eq 3 ]] && git diff -W --ignore-submodules $COMMIT_SHA1 $COMMIT_SHA2 -- $DIFF_FILE ) | \
awk '!/if/ && !/switch/ && !/return/ && !/for/ && !/\/\// && !/\/\*/ && !/#/ && !/while/' | \
awk '/[a-zA-Z_](\*\s)?(\s)*(\s\*)?[a-zA-Z0-9_]*\(.*\)/' | \
awk '!/;$/' | \
awk '/\(/' | \
sed 's/@@.*@@//' | \
sed 's/(.*//' | \
uniq
