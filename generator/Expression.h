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
    static Expression *

