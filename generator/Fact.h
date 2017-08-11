#ifndef FACT_H
#define FACT_H

#include <ostream>
#include <vector>

enum eFactCategory
{
  ePointTo=1,
  eUnionWrite=2
};

class Statement;
class StatementAssign;
class StatementReturn;
class Variable;
class Expression;
class Lhs;
class Function;
class ExpressionVariable;

class Fact
{
  public:
    Fact(eFactCategory e);

    virtual ~Fact(void);

    virtual Fact* clone(void) const = 0;

    virtual 
