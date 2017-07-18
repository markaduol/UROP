#!/usr/bin/env python

# References
# https://github.com/averagesecurityguy/Python-Examples/blob/master/read_file.py
# https://github.com/eliben/pycparser/blob/master/examples/func_defs.py

# Script to open a C file, extract C code of each function and
# write each function to file with the function's name as the file name.

from __future__ import print_function
import sys
import os.path

sys.path.extend(['.', '..'])

from pycparser import c_parser, c_ast, c_generator, parse_file

PREFIX_FILE = 'prefix_file.c'
SAVE_PATH = '/homes/ma7614/UROP/symbolic-execution/tmp/'

class PrefixVisitor(object):

    def visit(self, node):
        if (type(node) is c_ast.FuncDef):
            return
        with open(os.path.join(SAVE_PATH, PREFIX_FILE), 'a') as f:
            data = translate_to_c(node)
            f.write(data + ';\n\n')

# A simple visitor for FuncDef nodes that prints the names and locations of
# function definitions and writes the C code of each function to a new file
class FuncDefVisitor(c_ast.NodeVisitor):

    def visit_FuncDef(self, node):
        print('Generating file %s.c' % (node.decl.name))
        with open(os.path.join(SAVE_PATH, PREFIX_FILE)) as infile:
            with open(os.path.join(SAVE_PATH, node.decl.name + '.c'), 'w+') as outfile:
                # Prepend with prefix
                for line in infile:
                    outfile.write(line)
                # Write entire function to C file
                body = translate_to_c(node)
                outfile.write('\n' + body)
        self.insert_main_func(node)

    #TODO Insert 'main' function. Need to extract type of parameters; make them
    # symoblic and then call the function with the symbolic variables.
    def insert_main_func(self, node):
        str_list = []

        function_decl = node.decl

        if function_decl.type.args is None:
            return

        param_decls = function_decl.type.args.params

        with open(os.path.join(SAVE_PATH, node.decl.name + '.c'), 'a') as outfile:

            str_list.append("\nint main() {\n")
            args_list = []

            for param_decl in param_decls:

                if param_decl is None:
                    pass

                # Make parameters symbolic
                symbolic_addr = '&' + param_decl.name

                if (param_decl.type is c_ast.PtrDecl):
                    symbolic_addr = param_decl.name

                param_type = get_decl_type(param_decl)
                str_list.append("\t%s %s;\n" % (param_type, param_decl.name))

                str_list.append("\tklee_make_symbolic(%s, sizeof %s, \"%s\")\n" %
                                (symbolic_addr, param_decl.name, param_decl.name))


            symbolic_params = ','.join([param_decl.name for param_decl in param_decls])
            # Call function with symbolic params
            str_list.append("\t%s(%s)\n" % (node.decl.name, symbolic_params))
            str_list.append("}")

            main_func = ''.join(str_list)
            outfile.write(main_func)

def get_decl_type(decl):
    """ Recursively get the type of a decl node
    """
    typ = type(decl)

    if typ == c_ast.TypeDecl:
        quals = ' '.join(decl.quals) + ' ' if decl.quals else ''
        decl_type = get_decl_type(decl.type)

        if decl_type is None:
            return quals + 'void'
        else:
            return quals + decl_type

    elif typ == c_ast.Typename or typ == c_ast.Decl:
        return get_decl_type(decl.type)

    elif typ == c_ast.IdentifierType:
        return ' '.join(decl.names)

    elif typ == c_ast.PtrDecl:
        quals = ' '.join(decl.quals) + ' ' if decl.quals else ''
        decl_type = get_decl_type(decl.type)

        if decl_type is None:
            return quals + '*'
        else:
            return quals + decl_type + '*'


    elif typ == c_ast.ArrayDecl:
        arr = '[%s]' % decl.dim.value if decl.dim else ''
        return arr + get_decl_type(decl.type)

    elif typ == c_ast.Struct:
        return ('struct%s' % (' ' + decl.name if decl.name else ''))

def translate_to_c(node):
    generator = c_generator.CGenerator()
    return generator.visit(node)

def show_func_defs(filename):
    ast = parse_file(filename, use_cpp=True,
                     cpp_args=r'-Iutils/fake_libc_include')

    p_visitor = PrefixVisitor()
    for external_decl in ast.ext:
        p_visitor.visit(external_decl)

    f_visitor = FuncDefVisitor()
    f_visitor.visit(ast)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename = sys.argv[1]

        with open(os.path.join(SAVE_PATH, PREFIX_FILE), 'w+') as f:
            f.write('#include<klee/klee.h>\n\n')

        show_func_defs(filename)
    else:
        # Command line error
        sys.exit(2)
