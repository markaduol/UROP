#!/usr/bin/env python3

import argparse

DEFAULT_ARR_SIZE=8

def read_csv(csv_file):
    with open(csv_file, newline='') as f:
        reader = csv.reader(csv_file, delimiter=':')
        for row in reader:
    return

def output_stdc_headers(outputstream):
    return

def output_lib_headers(repo_dir, outputstream):
    return

class Variable:
    vars_to_free = []

    def __init__(self, name, _type, bool isArr=False, bool arrSize=0):
        self.name    = name
        self._type   = _type
        self.isArr   = isArr
        self.arrSize = arrSize

    def output_mk_var(self, outputstream):
        # Check outputstream is valid
        # Procedure for initialising variable:
        #   [self._type] [self.name] = malloc(sizeof([self._type]))
        #   Push [self.name] onto vars_to_free
        #   Output check testing whether malloc was successful (remember to free vars)
        #   Output klee_make_symbolic (should call Klee.output_symbolic(self, outputstream))
        return

    def output_free_vars(outputstream):
        # Output free calls for variables in 'vars_to_free', from last element to first

class Klee:
    """Encapsulates state for function under test by klee"""
    def __init__(self, funcname, functype, params):
        self.funcname = funcname
        self.functype = functype
        self.params   = params

    @staticmethod
    def output_headers():
        return

    @staticmethod
    def output_init():
        return

    @staticmethod
    def output_footer():
        return

    @staticmethod
    def output_symbolic(var, outputstream):
        return

    @staticmethod
    def output_assume(var1, var2, outputstream):
        return

    @staticmethod
    def output_assert(var1, var2):
        return

def get_arguments(arguments):
    """Grab user supplied arguments"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input-file", required=True, help="Input CSV file", type=str)
    parser.add_argument("--sortby", required=False, choices=['lines-changed', 'lines-added', 
                        'lines-removed', 'functions-changed', 'functions-added', 'functions-removed'],
                        default='functions-changed',
                        help=
                        """
                        Optional comparator to sort data from CSV file. Use with the
                        '--depth N' so that test drivers are generated for the top N
                        inputs as specified by the comparator.
                        """)
    parser.add_argument("-d", "--depth", required=False, default=10, type=int)
    args = parser.parse_args()

    # Validate arguments
    is_valid_csv(args.input_file)
    return args

def is_valid_csv(input_file):
    if not input_file.endswith('.csv'):
        sys.stderr.write("The file {} is not a valid CSV file".format(input_file))
        sys.exit(1)

if __name__ == "__main__":
    arguments = get_arguments()
