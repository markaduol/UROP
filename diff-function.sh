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

( [[ args_counter -eq 1 ]] && git diff --function-context $COMMIT_SHA1 || \  # Diff between changes working tree and HEAD of local repository
  git diff --function-context $COMMIT_SHA1 $COMMIT_SHA2 ) | \
awk '/(@@|\+|\-)/' | \
awk '/[a-z0-9]\(.*\)/' | \
awk '!/;$/' | \
awk '/\(/' | \
sed 's/(.*//' | \
uniq
