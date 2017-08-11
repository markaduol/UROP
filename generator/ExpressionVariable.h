#ifndef EXPRESSION_VARIABLE_H
#define EXPRESSION_VARIABLE_H

#include <ostream>
#include <vector>

#include "Expression.h"

class ExpressionVariable : public Expression
{
  public:
    static ExpressionVariable *make(CGContext &cg_context, const Type* type, 
                                    bool as_param=false, bool as_return=false);

    explicit ExpressionVariable(const Variable &v);

    ExpressionVariable(const Variable &v, const Type* t);
    
    virtual ~ExpressionVariable(void);

    virtual Expression* clone() const;

    virtual void get_eval_to_subexps(vector<const Expression*>& subs) const {subs.push_back(this);}

    const Variable* get_var(void) const {return &var;}

    virtual void get_referenced_ptrs(std::vector<const Variable*>& ptrs) const;

    virtual std::vector<const ExpressionVariable*> get_dereferenced_ptrs(void) const;

    virtual bool visit_facts(vector<const Fact*>& inputs, CGContext& cg_context) const;

    virtual const Type &get_type(void) const;

    virtual bool is_compatible(const Expression *exp) const;

    virtual bool is_compatible(const Varibale *v) const;

    virtual bool uses_var(const Variable* v) const {return v == &var;}

    virtual void Output(std::ostream &) const;

  private:
    explicit ExpressionVarialbe(const ExpressionVariable &expr);

    const Variable &var;

    const Type* type;
};

#endif
