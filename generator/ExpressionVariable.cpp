#include <cassert>

#include "CGContext.h"
#include "ExpressionVariable.h"

ExpressionVariable*
ExpressionVariable::make(CGContext &cg_context, const Type* type, bool as_param, bool as_return)
{
  Function *curr_func = cg_context.get_current_func();
  FactMgr* fm = get_fact_mgr_for_func(curr_func);
  vector<const Variable*> dummy;

  ExpressionVariable *ev = 0;
  
}
