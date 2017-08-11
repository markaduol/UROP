#ifndef FACT_MGR_H
#define FACT_MGR_H

class FactMgr
{
  public:
    FactMgr(const Function *f);

    FactMgr(const Function *f, const FactVec& facts);

    static void doFinalization();

    static void add_interested_facts(int interests);

    bool validate_fact(const Fact *f, const FactVec& facts);

    bool validate_assign(const Lhd*

#endif
