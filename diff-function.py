#!/usr/bin/env python3

# Reference: https://stackoverflow.com/questions/943391/how-to-get-the-function-declaration-or-definitions-using-regex/943429#943429

# Exctract function definitions from a C or C++ module
import re
import argparse
import sys
import os
import csv
import git
from itertools import combinations, chain, groupby

DEBUG=False

################ DEBUG FUNCTIONS ##################

def PRINT(x):
    if DEBUG:
        print(x)

###################################################

MODIFIED = 'modified'
ADDED = 'added'
REMOVED = 'removed'
VERBOSE_PREFIX = 'verbose_'
cpp_keywords = ['if', 'while', 'do', 'for', 'switch']

def required_length(_min, _max):
    """Custom function used to restrict the number of arguments passed to a particular flag
       when using argparse."""
    class RequiredLength(argparse.Action):
        def __call__(self, parser, args, values, option_string=None):
            if not _min <= len(values) <= _max:
                msg = "argument '{}' requires between {_min} and {_max} arguments inclusive.".format(f=self.dest, _min=_min, _max=_max)
                raise argparse.ArgumentTypeError(msg)
            setattr(args, self.dest, values)
    return RequiredLength

def get_repo(path):
    join = osp.join
    repo = Repo(os.path.abspath(path))
    assert not repo.bare
    return repo

def get_diff(repo, commit_sha1, commit_sha2='HEAD'):
    commit1 = repo.commit(commit_sha1)
    return commit1.diff(commit_sha2)

def get_commits(repo, commit_sha, count):
    return list(repo.iter_commits(commit_sha, max_count=count))

def get_arguments():
    """Grab user supplied arguments using the argparse library"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-r", "--repository", required=True, help="Path to git repository", type=str)

    parser.add_argument("--depth", default = 0, required=False, 
                        help=
                        """
                        Maximum number of commits to consider (relative to supplied commit)
                        If used, only one commit must be supplied.
                        """, type=int)
    parser.add_argument("--show-functions", required=False, choices=['modified', 'removed', 'added'],
                        help=
                        """
                        Dump added, modified or removed functions to standard output.
                        """)

    parser.add_argument("--commits", required=True, nargs='+',
                        help="Valid commit identifiers (min 1)")

    parser.add_argument("-o", "--output-file", required=False, help="Output file", type=str)
    parser.add_argument("-c", "--output-csv", required=False, help="Output to CSV file. You must \
                        specify the OUTPUT FILE, using the --output-file flag, as a csv file.", \
                        action='store_true')
    args = parser.parse_args()

    is_valid_repo(args.repository)

    return args

def validate_arguments(arguments):
    output_file = arguments.output_file
    output_csv = arguments.output_csv
    if output_csv and not output_file.endswith('.csv'):
        sys.stderr.write("The file '{}' is not a valid CSV file\n".format(output_file))
        sys.exit(1)

def is_valid_repo(path):
    """Ensure that the input file exists"""
    if not os.path.exists(os.path.join(path, '.git')):
        sys.stderr.write("The path '{}' does not point to a valid Git Repository".format(path))
        sys.exit(1)

def functions_added(diff_txt):
    """Return names of added functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE

    regex_added = re.compile(r"""
                          ^(?:\+)
                          (\s*[a-zA-Z_][a-zA-Z0-9_*\s]+)
                          \s*
                          (?<!\*)
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """, re_flags)

    return [(i.group(1).strip(), i.group(2).strip()) for i in regex_added.finditer(diff_txt) \
            if i.group(1).strip() not in cpp_keywords]

def functions_removed(diff_txt):
    """Return names of removed functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE

    regex_removed = re.compile(r"""
                          ^(?:\-)
                          (\s*[a-zA-Z_][a-zA-Z0-9_*\s]+)
                          \s*
                          (?<!\*)
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """, re_flags)


    return [(i.group(1).strip(), i.group(2).strip()) for i in regex_removed.finditer(diff_txt) \
            if i.group(1).strip() not in cpp_keywords]

# TODO: Add parameters 'body' and 'signature'
def functions_modified(diff_txt):
    """Return names of modified functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE
    regex_modified = re.compile(r"""
                          ^(?!\+|\-)
                          (\s*[a-zA-Z_][a-zA-Z0-9_*\s]+)
                          \s*
                          (?<!\*)
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """, re_flags)

    return [(i.group(1).strip(), i.group(2).strip()) for i in regex_modified.finditer(diff_txt) \
            if i.group(1).strip() not in cpp_keywords]

def changes(repo_dir, commits, arguments):
    """
    Returns the number of functions and lines added, removed and modified
    in the specified repository, for each pair of commits.
    Returned object is a list of 8-tuples
    """
    results = []
    verbose_results = []

    g = git.Git(repo_dir)
    commit_pairs = []
    commit_pairs = [(commits[i], commits[i+1]) for i in range(len(commits)-1)]

    numstat_pattern = r"(\d+)\s+(\d+)\s+([\w+./\\]+)" # Pattern for records in '--numstat' diff
    
    for pair in commit_pairs:
        diff1 = g.execute(["git", "diff", "-W", "--ignore-submodules", pair[0], pair[1]])
        diff2 = g.execute(["git", "diff", "--numstat", pair[0], pair[1]])

        l_added = 0 # Lines
        l_removed = 0
        l_modified = 0
        f_added = 0 # Functions
        f_removed = 0
        f_modified = 0

        f_added = functions_added(diff1)
        f_removed = functions_removed(diff1)
        f_modified = functions_modified(diff1)

        for line in diff2.splitlines():
            x = re.match(numstat_pattern, line)
            PRINT("Regex Match: {}".format(x))
            if not x:
                break # All records parsed
            l_added += int(x.group(1))
            l_removed += int(x.group(2))
            l_modified += int(x.group(1)) + int(x.group(2))

        results.append((pair[0], pair[1], l_added, l_removed, l_modified, len(f_added), len(f_removed), len(f_modified)))

        verbose_results.append((pair[0], pair[1], l_added, l_removed, l_modified, f_added, f_removed, f_modified))

    return results, verbose_results

def parse_file(arguments):
    """Gets function definitions from the supplied diff file"""

    depth = 0 # Depth of history to examine

    if arguments.depth > 0 and len(arguments.commits) > 1:
        sys.stderr.write("When using '--depth' flag, only one commit is allowed. {} supplied".format(len(arguments.commits)))
        sys.exit(1)
    elif arguments.depth > 0 and len(arguments.commits) == 1:
        depth = arguments.depth

    commits = arguments.commits
    if len(commits) < 2 and not arguments.depth:
        commits.append('HEAD')

    if depth > 0:
        for i in range(depth):
            commits.insert(0, 'HEAD~{}'.format(i+1))

    repo_dir = arguments.repository
    output_file = arguments.output_file
    output_csv = arguments.output_csv

    records         = changes(repo_dir, commits, arguments)[0] # Records for CSV file (as tuples)
    verbose_records = changes(repo_dir, commits, arguments)[1] # Records for CSV file (as tuples)

    for record in records:
        PRINT('Record : {}'.format(record))


    if arguments.show_functions != None:
        for record in verbose_records:
            print("Revision 1: {}, Revision 2: {}".format(record[0], record[1]))
            funcs = []
            if arguments.show_functions == MODIFIED:
                print("Modified Functions...\n")
                funcs = record[7]
            elif arguments.show_functions == ADDED:
                print("Added Functions...\n")
                funcs = record[5]
            elif arguments.show_functions == REMOVED:
                print("Removed Functions...\n")
                funcs = record[6]
            for f in funcs:
                print("{}({})\n".format(f[0], f[1]))


    if output_csv and output_file is not None:
        print("Writing to CSV...")
        dict_records         = [] # Records for CSV file in correct format (list of dictionaries)
        verbose_dict_records = [] # Records for CSV file in correct format (list of dictionaries)

        headers = ['Revision 1', 'Revision 2', 'Lines added', 'Lines removed', 'Lines changed', 
                   'Functions added', 'Functions removed', 'Functions modified']

        for record in records:
            assert len(record) == len(headers)
            dict_record = dict(zip(headers, record))
            dict_records.append(dict_record)
        write_to_csv(output_file, headers, dict_records)

        for record in verbose_records:
            assert len(record) == len(headers)
            dict_record = dict(zip(headers, record))
            verbose_dict_records.append(dict_record)

        # Write full function declarations
        verbose_output_file = VERBOSE_PREFIX + output_file
        write_to_csv(verbose_output_file, headers, verbose_dict_records)
       
def write_to_csv(csv_file, headers, records):
    f = open(csv_file, 'w', newline='')
    writer = csv.DictWriter(f, fieldnames = headers, delimiter=':')

    try:
        writer.writeheader()
        for r in records:
            PRINT ('Row: {}'.format(r))
            writer.writerow(r)
    except csv.Error as e:
        f.close
        sys.exit('file {}, line {}: {}'.format(csv_file, writer.line_num, e))
    f.close


####################### STATISTICS #########################

def output_stats_header():
    print ("\n--------------STATISTICS-----------------\n")

def output_stats_footer():
    print ("\n------------------------------------------\n")

if __name__ == "__main__":
    arguments = get_arguments()
    validate_arguments(arguments)
    parse_file(arguments)
