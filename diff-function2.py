#!/usr/bin/env python3

# Reference: https://stackoverflow.com/questions/943391/how-to-get-the-function-declaration-or-definitions-using-regex/943429#943429

# Exctract function definitions from a C or C++ module
import re
import argparse
import sys
import os

def get_arguments():
    """Grab user supplied arguments using the argparse library"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input-file", required=True, help="Diff input file", type=str)
    parser.add_argument("-o", "--output-file", required=False, help="Output file", type=str)
    args = parser.parse_args()

    is_valid_file(parser, args.input_file)

    return args.input_file, args.output_file

def is_valid_file(parser, file_name):
    """Ensure that the input file exists"""
    if not os.path.exists(file_name):
        parser.error("The file '{}' does not exist!".format(filename))
        sys.exit(1)

def gather_statistics(rev1, rev2):
    return


def parse_file(arguments):
    """Gets function definitions from the supplied diff file"""
    input_file = arguments[0]
    output_file = None
    if len(arguments) > 1:
        output_file = arguments[1]

    f = open(input_file)
    diff_txt = ''.join(f.readlines())
    f.close

    re_flags = re.VERBOSE | re.MULTILINE
    pattern_modified = re.compile(r"""
                          ^(?!\+|\-).*
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

    pattern_added = re.compile(r"""
                          ^(?=\+).*
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

    pattern_removed = re.compile(r"""
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

    cpp_keywords = ['if', 'while', 'do', 'for', 'switch']
    results1 = [(i.group(1), i.group(2)) for i in pattern_modified.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]

    results2 = [(i.group(1), i.group(2)) for i in pattern_added.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]

    results3 = [(i.group(1), i.group(2)) for i in pattern_removed.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]

    if output_file is None:
        print ('Modified functions:\n')
        for i in results1:
            print (i[0] + '(' + i[1] + ')')

        print ('\nAdded functions:\n')
        for i in results2:
            print (i[0] + '(' + i[1] + ')')

        print ('\nRemoved functions:\n')
        for i in results3:
            print (i[0] + '(' + i[1] + ')')
    else:
        with open(output_file, 'w') as f:
            print ('Modified functions:\n')
            for i in results1:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            print ('\nAdded functions:\n')
            for i in results2:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            print ('\nRemoved functions:\n')
            for i in results3:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

if __name__ == "__main__":
    arguments = get_arguments()
    parse_file(arguments)
