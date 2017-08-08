#!/usr/bin/env bash

set -e

COMMIT_SHA1=
COMMIT_SHA2=origin/master #default
DIFF_PATH=

die () {
  echo >&2 "$@"
  exit 1
}

if [[ $1 == "--help" ]]; then
  die "Usage: $0 <commit-sha> [<commit-sha>] [path] (if [path] is used, two <commit-sha> arguments are required)"
fi

if [[ $# -lt 1 ]]; then
  die "Usage: $0 <commit-sha> [<commit-sha>] [path] (if [path] is used, two <commit-sha> arguments are required)"
fi

for sha in "${@:1:2}"
do
  if git cat-file -e $sha^{commit}; then
    echo "Commit identifier $sha exists"
  else
    echo "Invalid commit identifier: $sha"
    die "Usage: $0 <commit-sha> [<commit-sha>] [path] (if [path] is used, two <commit-sha> arguments are required)"
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
( [[ args_counter -eq 1 ]] && git diff --ignore-submodules $COMMIT_SHA1 -- '*.c' || \
  [[ args_counter -eq 2 ]] && git diff --ignore-submodules $COMMIT_SHA1 $COMMIT_SHA2 -- '*.c' || \
  [[ args_counter -eq 3 ]] && git diff --ignore-submodules $COMMIT_SHA1 $COMMIT_SHA2 -- $DIFF_PATH ) | \
grep -E '^(@@)' | \
grep '(' | \
sed 's/@@.*@@//' | \
sed 's/(.*//' | \
sed 's/\*//' | \
awk '{print $NF}' | \
uniq

# Explanation:
# 1: Get diff
# 2: Get only lines with hunk headers; if the 'optional section heading' of a hunk header exists, it
#    will be the function definition of a modified function
# 3: Pick only hunk headers containing open parentheses, as they will contain function definitions
# 4: Get rid of '@@ [old-file-range] [new-file-range] @@' sections in the lines
# 5: Get rid of everything after opening parentheses
# 6: Get rid of '*' from pointers
# 7: [See 'awk']: Print the last field (i.e: column) of the records (i.e: lines).
# 8: Get rid of duplicate names.
