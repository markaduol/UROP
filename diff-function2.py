#!/usr/bin/env python3

# Reference: https://stackoverflow.com/questions/943391/how-to-get-the-function-declaration-or-definitions-using-regex/943429#943429

# Exctract function definitions from a C or C++ module
import re
import argparse
import sys
import os
import csv
import git


MODIFIED = 'modified'
ADDED = 'added'
REMOVED = 'removed'

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

    parser.add_argument("--depth", default=50, required=False, 
                        help="Maximum number of commits to consider (relative to HEAD)", type=int)

    parser.add_argument("--commits", required=True, nargs='+', action=required_length(1,2),
                        help="Valid commit identifiers (min 1, max 2)")

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
        parser.error("The path '{}' does not point to a valid Git Repository!".format(path))
        sys.exit(1)

def functions_added(diff_txt):
    """Return names of added functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE
    regex_added = re.compile(r"""
                          ^(?=\+).*
                          (?<=[\s:~])
                          ([a-zA-Z0-9_*]+)
                          \s*
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """, re_flags)

    return [(i.group(1), i.group(2)) for i in regex_added.finditer(diff_txt) \
            if i.group(1) not in cpp_keywords]

def functions_removed(diff_txt):
    """Return names of removed functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE
    regex_removed = re.compile(r"""
                          ^(?=\-).*
                          (?<=[\s:~])
                          ([a-zA-Z0-9_*]+)
                          \s*
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """,
                          re_flags)

    return [(i.group(1), i.group(2)) for i in regex_removed.finditer(diff_txt) \
            if i.group(1) not in cpp_keywords]

# TODO: Add parameters 'body' and 'signature'
def functions_modified(diff_txt):
    """Return names of modified functions as tuple of function name and parameters"""
    re_flags = re.VERBOSE | re.MULTILINE
    regex_modified = re.compile(r"""
                          ^(?!\+|\-).*
                          (?<=[\s:~])
                          ([a-zA-Z0-9_*]+)
                          \s*
                          \(([\w\s,<>\[\].=&':/*]*?)\)
                          \s*
                          (const)?
                          \s*
                          (?={)
                          """, re_flags)

    return [(i.group(1), i.group(2)) for i in regex_modified.finditer(diff_txt) \
            if i.group(1) not in cpp_keywords]


def lines_changed(diff_txt):
    """Returns number of lines added, lines removed, lines changed"""
    numstats = None
    pattern = r"(\d+)\s+(\d+)\s+([\w+./\\]+)" # Pattern for records
    added_column = []
    removed_column = []
    modified_column = []

    for line in diff_txt:
        x = re.match(pattern, line)
        if not x:
            break # All records parsed
        added_column.append(x.group(1))
        removed_column.append(x.group(2))
        modified_column.append(int(x.group(1)) + int(x.group(2)))

    return sum(added_column), sum(removed_column), sum(modified_column)


def get_pairwise_diffs(working_dir, commits):
    """
    Generate all diffs between pairs of commits in the list 'commits'.
    The returned list will should be a list of diff files (ending in .diff)
    """
    diffs = []
    g = git.Git(working_dir)
    commit_pairs = [(i, j) for i in commits for j in commits if i != j]

    for pair in commit_pairs:
        diff = g.execute(["git", "diff", "--numstat", pair[0], pair[1]])
        diffs.append(diff)
    return diffs

def parse_file(arguments):
    """Gets function definitions from the supplied diff file"""
    # TODO: Need to extend to support multiple git diff files
    # repo = get_repo(arguments.repository)
    commits = arguments.commits
    if len(commits) < 2:
        commits.append('HEAD')
    diffs = get_pairwise_diffs(arguments.repository, commits)
    

    print (diffs[0])
    for diff in diffs:
        process(diff, arguments)

    
def process(text, arguments):
    """Process text (should be from git diff) and write to output stream"""
    output_file = arguments.output_file
    output_csv = arguments.output_csv

    diff_txt = ''.join(text)

    records = [] # Records for CSV file
    results1 = functions_modified(diff_txt)
    results2 = functions_added(diff_txt)
    results3 = functions_removed(diff_txt)
    numstats = lines_changed(text) # Length 3 array

    if output_file is None:
        print ('Modified functions:\n')
        for i in results1:
            print (i[0] + '(' + i[1] + ')')

        print ('Added functions:\n')
        for i in results2:
            print (i[0] + '(' + i[1] + ')')

        print ('Removed functions:\n')
        for i in results3:
            print (i[0] + '(' + i[1] + ')')
    elif output_csv and output_file is not None:
        print ("Writing to CSV...")
        output_stats_header()

        output_statistics(MODIFIED, results1)
        
        output_statistics(ADDED, results2)
        
        output_statistics(REMOVED, results3)

        output_stats_footer()
    else:
        with open(output_file, 'w') as f:
            for i in results1:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            for i in results2:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            for i in results3:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

def write_to_csv(csv_file, headers, records):
    f = open(csv_file, 'w', newline='')
    writer = csv.DictWriter(f, fieldnames = headers, delimiter=':')

    try:
        for r in records:
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

def output_statistics(category, results):
    if category == "":
        sys.stderr.write("Error: Empty 'category' variable")
        sys.exit(1)

    num_records = len(results)
    print ("Number of '{}' functions: '{}'".format(category, num_records))

if __name__ == "__main__":
    arguments = get_arguments()
    validate_arguments(arguments)
    parse_file(arguments)
