#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <ostream>
#include <vector>
#include <string>

enum eTermType
{
  eConstant,
  eVariable,
  eFunction,
  eAssignment,
  eCommaExpr,
  eLhs
};
#define MAX_TERM_TYPES ((eTermType) (eCommaExpr+1))

class Expression
{
  public:
    static Expression *make(CGContext &cg_context, const Type* type, enum eTermType tt=MAX_TERM_TYPES);

    Expression(eTermType e);

    Expression(const Expression &expr);

    virtual ~Expression(void);

    virtual Expression *clone() const = 0;

    virtual const Type& get_type(void) const = 0;

    virtual void get_eval_to_subexps(vector<const Expression*>& subs) const = 0;

    virtual void get_called_funcs(std::vector<const FunctionInvocationUser*>& funcs) const {};

    virtual const FunctionInvocation* get_invoke(void) const {return NULL;};

    virtual bool visit_facts(vector<const Fact*>& facts, CGContext& cg_context) const {return true;};

    virtual bool use_var(const Variable* v) const {return false;}

    virtual void Output(std::ostream&) const = 0;

    virtual void indented_output(std::ostream& out, int indent) const;

    unsigned int func_count(void) const;

    std::string to_string(void) const;

    virtual bool is_compatible(const Expression*) const {return false;}

    virtual bool is_compatible(const Variable*) const {return false;}

    void check_and_set_cast(const Type* t);

    void output_cast(std::ostream& out) const;

    // Fields
    enum eTermType term_type;
    int expr_id;
    const Type* cast_type;
};
