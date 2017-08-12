#!/usr/bin/env python3

# Reference: https://stackoverflow.com/questions/943391/how-to-get-the-function-declaration-or-definitions-using-regex/943429#943429

# Exctract function definitions from a C or C++ module
import re
import argparse
import sys
import os
import csv

def get_arguments():
    """Grab user supplied arguments using the argparse library"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input-file", required=True, help="Diff input file", type=str)
    parser.add_argument("-o", "--output-file", required=False, help="Output file", type=str)
    parser.add_argument("-c", "--output-csv", required=False, help="Output to CSV file. You must \
                        specify the OUTPUT FILE, using the --output-file flag, as a csv file.", \
                        action='store_true')
    args = parser.parse_args()

    is_valid_file(parser, args.input_file)

    return args.input_file, args.output_file, args.output_csv

def validate_arguments(arguments):
    output_file = arguments[1]
    output_csv = arguments[2]
    if output_csv and not output_file.endswith('.csv'):
        sys.stderr.write("The file '{}' is not a valid CSV file\n".format(output_file))
        sys.exit(1)

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
    output_file = arguments[1]
    output_csv = arguments[2]

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

        print ('Added functions:\n')
        for i in results2:
            print (i[0] + '(' + i[1] + ')')

        print ('Removed functions:\n')
        for i in results3:
            print (i[0] + '(' + i[1] + ')')
    elif output_csv and output_file is not None:
        print ("Writing to CSV...")
        write_to_csv(output_file, results2)
    else:
        with open(output_file, 'w') as f:
            print ('Modified functions:\n')
            for i in results1:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            print ('Added functions:\n')
            for i in results2:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

            print ('Removed functions:\n')
            for i in results3:
                s = i[0] + '(' + i[1] + ')'
                f.write(s);

def write_to_csv(csv_file, regex_results):
    with open(csv_file, 'w', newline='') as f:
        fieldnames = ['function_name', 'function_params']
        writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter=':')

        try:
            writer.writeheader()
            for i in regex_results:
                writer.writerow({'function_name': i[0], 'function_params': i[1]})
        except csv.Error as e:
            sys.exit('file {}, line {}: {}'.format(csv_file, writer.line_num, e))


if __name__ == "__main__":
    arguments = get_arguments()
    validate_arguments(arguments)
    parse_file(arguments)
