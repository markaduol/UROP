#include <cassert>

#include "Expression.h"

unsigned int 
Expression::func_count(void) const
{
  std::vector<const FunctionInvocationUser*> funcs;
  get_called_funcs(funcs);
  return funcs.size();
}

std::string
Expression::to_string(void) const
{
  std::osstringstream oss;
  Output(oss);
  return oss.str();
}

void
Expression::indented_output(std::ostream& out, int indent) const
{
  output_tab(out, indent);
  Output(out);
}

Expression*
Expression::make(CGContext& cg_context, const Type* type, enum eTermType tt)
{

}
