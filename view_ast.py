from __future__ import print_function
import sys

from pycparser import c_parser, c_ast

SPACE = ' '

text = r"""
const int SOME_CONSTANT = 42;
void func(const int y, unsigned int z)
{
  x = 1;
}
"""

parser = c_parser.CParser()
ast = parser.parse(text)
print("Before:")
ast.show(offset=2)
#print("\nFirst external decl:")
#ast.ext[0].show(offset=2)

for i, ext_decl in enumerate(ast.ext):
    print("\nExternal decl %d:" % (i))
    ext_decl.show(offset=2)

    if (type(ext_decl) is c_ast.FuncDef):
        print("\n%sFunction decl:" % (SPACE * 2))
        ext_decl.decl.show(offset=4)
        print("\n%sFunction decl type" % (SPACE * 4))
        ext_decl.decl.type.show(offset=8)
        print("\n%sFunction decl args" % (SPACE * 4))
        ext_decl.decl.type.args.show(offset=8)
        print("\n%sFunction body" % (SPACE * 2))

        for decl in ext_decl.body.block_items:
            decl.show(offset=4)
