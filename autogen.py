#!/usr/bin/env python3

import argparse
import git
import os
import io
import shutil
import re
import csv
import sys

import diff_function as df

from glob import glob

DEBUG = False

##################### DEBUG FUNCTIONS ######################

def PRINT(x):
    if DEBUG:
        print(x)

############################################################

DEFAULT_ARR_SIZE=8
C_HEADERS=['<stdio.h>', '<string.h>', '<stdlib.h>', '<stdint.h>']
BOILERPLATE_HEADERS=['utils.h', 'concrete.h', 'symbolic.h']
KLEE_HEADERS=['klee/klee.h']
HEADER_COMMENT="""/* This file was automatically generated by autogen.py */\n\n"""
TD_PREFIX="autotd"
RENAME_SUFFIX="_renamed"

def create_outputstream():
    f = io.StringIO()
    f.write(HEADER_COMMENT)
    return f

def is_valid_outputstream(outputstream):
    if type(outputstream) is not io.StringIO:
        sys.stderr.write("Expected type: io.StringIO\nActual type: {}\n".format(type(outputstream)))
        sys.exit(1)

def read_csv(csv_file):
    """Reads rows of CSV file. Returns a list of tuples."""
    results = []
    with open(csv_file, newline='') as f:
        reader = csv.reader(f, delimiter=':')
        for row in reader:
            #PRINT('CSV Row: {}'.format(row))
            results.append(row)
    return results

def write_with_tabs(outputstream, output_str, tabs=0):
    is_valid_outputstream(outputstream)
    for i in range(tabs):
        output_tab(outputstream)
    outputstream.write(output_str)

def output_newline(outputstream):
    outputstream.write("\n")

def output_tab(outputstream):
    outputstream.write("\t")

def output_boilerplate_headers(outputstream):
    for header in BOILERPLATE_HEADERS:
        output_str = "#include \"{}\"\n".format(header)
        outputstream.write(output_str)

def output_stdc_headers(outputstream):
    for header in C_HEADERS:
        output_str = "#include {}\n".format(header)
        outputstream.write(output_str)

def output_lib_headers(repo_dir, outputstream):
    # Find all 'include' dirs and add the absolute paths of all header files found within them
    #   find repo_dir -type d -name '*include' -print > include_dirs
    #   for each dir in include_dirs: for each root, subdir, files in os.walk(dir): if 
    include_files = []
    # For the 'upb' repository, the public C/C++ API is defined by all header (.h) files under
    # "upb/", except those ending with '.int.h', which are internal-only (see 'upb' repository README)
    path = os.path.join(repo_dir, 'upb')

    for f in glob(os.path.join(path, '*.h')):
        if f.endswith('.int.h') or f.endswith('-inl.h'): # 'upb' issue: '-inl.h' files should also be internal (waiting for confirmation)
            continue
        include_files.append(f)
        output_str = "#include \"{}\"\n".format(f)
        outputstream.write(output_str)

class TDContext:
    _id = 0

    def __init__(self, funcname, functype, params):
        TDContext._id += 1

        self.id       = TDContext._id # Used to number output files (i.e: test drivers)
        self.funcname = funcname
        self.functype = functype
        self.params   = params

    def get_funcname(self):
        return self.funcname

    def get_functype(self):
        return self.functype

    def get_params(self):
        return self.params

    def to_string(self):
        return """
          TDContext:
            ID: {}
            Name: {}
            Type: {}
            Params: {}
          """.format(self.id, self.funcname, self.functype, self.params)

    def get_vars_from_params(self):
        """Creates list of 'Variable' objects from 'self''s list of 'params'"""
        variables = []
        #pattern = r"([a-zA-Z_][a-zA-Z0-9_]*)(?:\s*\**\s*)([a-zA-Z_][a-zA-Z0-9_]*)" # TODO: Potential false positive for x*y or similar
        pattern = r"((?:const |volatile )?[a-zA-Z_][a-zA-Z0-9_]*\s*\**)(\s*[a-zA-Z_][a-zA-Z0-9_]*)"
        for param in self.params:
            x = re.match(pattern, param)
            if not x:
                sys.stderr.write("Could not extract variable name from function parameter: {}\n".format(param))
                return None
            var_name = x.group(2)
            var_type = x.group(1)
            var_isPtr = var_type.endswith('*')
            var_arrSize = 0 #TODO
            var = Variable(var_name, var_type, var_isPtr, var_arrSize)
            variables.append(var)
        return variables

    def output_funccalls(self, outputstream):
        """Emits function call code to 'outputstream' and also returns the return variables of said function calls"""
        isPtr = self.functype.endswith('*')
        var1  = Variable("res1", self.functype, isPtr, 0) ## TODO: arrSize
        var2  = Variable("res2", self.functype, isPtr, 0) ## TODO: arrSize

        param_vars = self.get_vars_from_params()
        arguments  = []
        for var in param_vars:
            if var.is_ptr():
                star_count = var.get_type().count('*')
                ptr_str    = '&' * star_count
                # TODO: Be sure this works
                arg_str = "{}{}".format(ptr_str, var.get_name()) 
            else:
                arg_str = var.get_name()
            arguments.append(arg_str)
        arguments = ', '.join(arguments)

        output_str1 = "{} {} = {}({});\n".format(var1.get_type(), var1.get_name(), self.funcname, arguments)
        output_str2 = "{} {} = {}{}({});\n".format(var2.get_type(), var2.get_name(), self.funcname, RENAME_SUFFIX, arguments)

        outputstream.write(output_str1)
        outputstream.write(output_str2)
        return var1, var2

    def dump_to_cfile(self, outputstream):
        filename = TD_PREFIX + '{}.c'.format(self.id)
        with open(filename, 'w') as f:
            outputstream.seek(0)
            shutil.copyfileobj(outputstream, f)

class Variable:
    vars_to_free = []

    def __init__(self, name, _type, isPtr=False, arrSize=0):
        self.name    = name
        self.type    = _type
        self.isPtr   = isPtr
        self.arrSize = arrSize

    def to_string(self):
        return """
          Variable:
            Name: {}
            Type: {}
            IsPtr: {}
            Array Size: {}
              """.format(self.name, self.type, self.isPtr, self.arrSize)

    def get_name(self):
        return self.name

    def get_id(self):
        return self.id

    def get_type(self):
        return self.type

    def is_ptr(self):
        return self.isPtr

    def get_arr_size(self):
        return self.arrSize

    def output_mk_var(self, outputstream):
        # Check outputstream is valid
        # Procedure for initialising variable:
        #   [self._type] [self.name] = malloc(sizeof([self._type]))
        #   Push [self.name] onto vars_to_free
        #   Output check testing whether malloc was successful (remember to free vars)
        #   Output klee_make_symbolic (should call Klee.output_symbolic(self, outputstream))
        is_valid_outputstream(outputstream)

        if self.isPtr:
            # self.type should already contain '*'s if var is pointer
            output_str = "{} {} = malloc(sizeof(*{}));\n".format(self.type, self.name, self.name)
            outputstream.write(output_str)
            Variable.vars_to_free.append(self)
            output_str = "if (!{})\n\tmalloc_fail(-1);\n".format(self.name)
            outputstream.write(output_str)
        else:
            output_str = "{} {};\n".format(self.type, self.name)
            outputstream.write(output_str)
        Klee.output_symbolic(self, outputstream)
        return

    def output_free_vars(outputstream):
        # Output free calls for variables in 'vars_to_free', from last element to first
        is_valid_outputstream(outputstream)
        return

class Klee:

    @staticmethod
    def output_headers(outputstream):
        is_valid_outputstream(outputstream)
        for header in KLEE_HEADERS:
            output_str = "#include \"{}\"\n".format(header)
            outputstream.write(output_str)

    @staticmethod
    def output_init(outputstream):
        is_valid_outputstream(outputstream)
        output_str = "int main()\n{\n"
        outputstream.write(output_str)

    @staticmethod
    def output_footer(outputstream):
        is_valid_outputstream(outputstream)
        output_str = "return 0;\n}"
        outputstream.write(output_str)
        return

    @staticmethod
    def output_symbolic(var, outputstream):
        is_valid_outputstream(outputstream)
        if var.is_ptr():
            star_count = var.get_type().count('*')
            ptr_str    = '*' * star_count
            output_str = "klee_make_symbolic({}, sizeof({}{}), \"{}\");\n".format(var.get_name(), ptr_str, var.get_name(), var.get_name())
            outputstream.write(output_str)
        else:
            output_str = "klee_make_symbolic(&{}, sizeof({}), \"{}\");\n".format(var.get_name(), var.get_name(), var.get_name())
            outputstream.write(output_str)

    @staticmethod
    def output_assume(var1, var2, outputstream):
        is_valid_outputstream(outputstream)
        if var.is_ptr():
            _str1 = "int i;\n"
            _str2 = "for (i=0; i < {}; i++)\n"
            _str3 = "{\n"
            _str4 = "klee_assume({}[i] == {}[i]);\n".format(var1.get_name(), var2.get_name())
            _str5 = "}\n\n"
            write_with_tabs(outputstream, _str1, 1)
            write_with_tabs(outputstream, _str2, 1)
            write_with_tabs(outputstream, _str3, 1)
            write_with_tabs(outputstream, _str4, 2)
            write_with_tabs(outputstream, _str5, 1)
        else:
            _str1 = "klee_assume({} == {});\n\n".format(var1.get_name(), var2.get_name())
            write_with_tabs(outputstream, _str1, 1)
                
    @staticmethod
    def output_assert(var1, var2, outputstream):
        is_valid_outputstream(outputstream)
        output_str = "klee_assert({} == {});\n".format(var1.get_name(), var2.get_name())
        outputstream.write(output_str)

def get_arguments():
    """Grab user supplied arguments"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", required=False, help="Verbose output", action="store_true")
    parser.add_argument("-r", "--repository", required=True, help="Path to git repository", type=str)
    parser.add_argument("--commits", required=True, nargs=2, help="Commits (exactly 2) which are used to generate test drivers", type=str)
    parser.add_argument("--sortby", required=False, choices=['lines-changed', 'lines-added', 
                        'lines-removed', 'functions-changed', 'functions-added', 'functions-removed'],
                        default='functions-changed',
                        help=
                        """
                        Optional comparator to sort data from CSV file. Use with the
                        '--depth N' so that test drivers are generated for the top N
                        inputs as specified by the comparator.
                        """)
    parser.add_argument("-d", "--depth", required=False, default=10, type=int, help="Number of test drivers to generate (default 10)")
    args = parser.parse_args()

    if args.verbose:
        global DEBUG
        DEBUG = True

    # Validate arguments
    is_valid_repo(args.repository)
    return args

def is_valid_csv(input_file):
    if not input_file.endswith('.csv'):
        sys.stderr.write("The file {} is not a valid CSV file\n".format(input_file))
        sys.exit(1)

def is_valid_repo(path):
    """Ensure that the input file exists"""
    if not os.path.exists(os.path.join(path, '.git')):
        sys.stderr.write("The path '{}' does not point to a valid Git Repository\n".format(path))
        sys.exit(1)

def transpose(xs):
    """Transposes a list of lists"""
    results = []
    for i in range(len(xs[0])):
        results.append([x[i] for x in xs])
    return results

def candidate_funcs(funcs):
    """
    Filter the input list of functions, retaining only functions for which test
    drivers can be generated. Note: 'funcs' is a list of tuples, the first element
    is the function name and the second are the parameters.
    """
    for f in funcs:
        PRINT("Function: {}".format(f))
    pattern = r"(\s*static\s*|\s*void\s*).*"
    return [(name, params) for name, params in funcs if not re.match(pattern, name)]

def parse_funcname(fdecl):
    """'fdecl' refers to the function declaration without parameters"""
    pattern = r"((?:static )?[a-zA-Z_][a-zA-Z0-9_]*\s*\*?)(\s*[a-zA-Z_][a-zA-Z0-9_]*)"
    m = re.match(pattern, fdecl)
    if not m:
        sys.stderr.write("Could not extract name from function {}\n".format(fdecl))
    else:
        return m.group(2).strip()

def parse_functype(fdecl):
    """'fdecl' refers to the function declaration without parameters"""
    pattern = r"((?:static )?[a-zA-Z_][a-zA-Z0-9_]*\s*\*?)(\s*[a-zA-Z_][a-zA-Z0-9_]*)"
    m = re.match(pattern, fdecl)
    if not m:
        sys.stderr.write("Could not extract type from function {}\n".format(fdecl))
    else:
        return m.group(1).strip()

if  __name__ == "__main__":
    arguments = get_arguments()
    #TODO: Get list of records of form: (funcname, functype, list of params (incl. types))
    # The records retrieved should be indexed by pairs of revisions: (rev1, rev2)
    # So we need to extract this info from the verbose CSV file, not the one given as input.

    # Get Functions for first revision

    commit1 = arguments.commits[0]
    commit2 = arguments.commits[1]
    res, verbose_res = df.changes(arguments.repository, arguments.commits)

    assert len(res) == 1
    assert len(verbose_res) == 1

    for i in range(1):
        record     = verbose_res[i]
        print ("Generating Test Drivers for revision pair: ({}, {})".format(record[0], record[1]))
        f_modified = record[7]
        funcs      = candidate_funcs(f_modified)

        contexts = []
        
        for f in funcs:

            outputstream = create_outputstream()

            output_stdc_headers(outputstream)
            output_lib_headers(arguments.repository, outputstream)
            output_boilerplate_headers(outputstream)
            Klee.output_headers(outputstream)

            fname  = parse_funcname(f[0])
            ftype  = parse_functype(f[0])
            params = re.split(r"\s*,\s*", f[1])

            if not params:
                sys.stderr.write("Could not parse parameter string: {}\n".format(f[1]))

            ctx = TDContext(fname, ftype, params)
            PRINT(ctx.to_string())

            variables = ctx.get_vars_from_params()

            if not variables:
                continue

            Klee.output_init(outputstream)

            for var in variables:
                PRINT(var.to_string())
                var.output_mk_var(outputstream)
                output_newline(outputstream)

            res1, res2 = ctx.output_funccalls(outputstream)
            Klee.output_assert(res1, res2, outputstream)

            Klee.output_footer(outputstream)

            ctx.dump_to_cfile(outputstream)
            print ("Test Driver for function '{}' generated successfully".format(f))

