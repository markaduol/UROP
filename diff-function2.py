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

def parse_file(arguments):
    """Gets function definitions from the supplied diff file"""
    input_file = arguments[0]
    output_file = None
    if len(arguments) > 1:
        output_file = arguments[1]

    f = open(input_file)
    diff_txt = ''.join(f.readlines())
    f.close

                           # Problematic line: Want to ensure we do not match lines with +/-
    re_flags = re.VERBOSE | re.MULTILINE
    pattern = re.compile(r"""
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

    cpp_keywords = ['if', 'while', 'do', 'for', 'switch']
    results = [(i.group(1), i.group(2)) for i in pattern.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]
    if output_file is None:
        for i in results:
            print (i[0] + '(' + i[1] + ')')
    else:
        with open(output_file, 'w') as f:
            for i in results:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

if __name__ == "__main__":
    arguments = get_arguments()
    parse_file(arguments)
