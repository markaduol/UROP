#!/usr/bin/env python3

# Reference: https://stackoverflow.com/questions/943391/how-to-get-the-function-declaration-or-definitions-using-regex/943429#943429

# Exctract function definitions from a C or C++ module
import re
import argparse
import sys
import os
import csv

MODIFIED = 'modified'
ADDED = 'added'
REMOVED = 'removed'

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

def parse_file(arguments):
    """Gets function definitions from the supplied diff file"""
    input_file = arguments[0]
    output_file = arguments[1]
    output_csv = arguments[2]

    f = open(input_file)
    diff_txt = ''.join(f.readlines())
    f.close

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
                          """,
                          re_flags)
    s = re.sub(r"^(?!\+|\-).*", r"^(?=\+).*", regex_modified.pattern)
    regex_added = re.compile(s, re_flags)
    s = re.sub(r"^(?!\+|\-).*", r"^(?=\-).*", regex_modified.pattern)
    regex_removed = re.compile(s, re_flags)

    cpp_keywords = ['if', 'while', 'do', 'for', 'switch']
    results1 = [(i.group(1), i.group(2)) for i in regex_modified.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]

    results2 = [(i.group(1), i.group(2)) for i in regex_added.finditer(diff_txt) \
               if i.group(1) not in cpp_keywords]

    results3 = [(i.group(1), i.group(2)) for i in regex_removed.finditer(diff_txt) \
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
        output_stats_header()

        output_statistics(MODIFIED, results1)
        write_to_csv(output_file, results1, False, MODIFIED)
        
        output_statistics(ADDED, results2)
        write_to_csv(output_file, results2, True, ADDED)
        
        output_statistics(REMOVED, results3)
        write_to_csv(output_file, results3, True, REMOVED)

        output_stats_footer()
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

def write_to_csv(csv_file, regex_results, append, category):
    """
    csv_file: CSV file to which to write
    regex_results: data which will be written
    append: True=append to CSV file, False=otherwise
    category: Should be 'modified', 'added' or 'removed'
    """
    if category == "":
        sys.stderr.write("Error: Empty 'category' variable")
        sys.exit(1)
    if append:
        f = open(csv_file, 'a', newline='')
    else:
        f = open(csv_file, 'w', newline='')
    fieldnames = ['category', 'function_name', 'function_params']
    writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter=':')

    try:
        if not append:
            writer.writeheader()
        for i in regex_results:
            writer.writerow({'category': category, 'function_name': i[0], 'function_params': i[1]})
    except csv.Error as e:
        f.close
        sys.exit('file {}, line {}: {}'.format(csv_file, writer.line_num, e))
    f.close

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
