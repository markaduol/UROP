#include <cassert>

#include "DefaultProgramGenerator.h"

DefaultProgramGenerator::DefaultProgramGenerator(int argc, char *argv[], unsigned long seed)
  : argc_(argc),
    argv_(argv),
    seed_(seed),
    good_count_(0),
    output_mgr_(NULL)
{

}

DefaultProgramGenerator::~DefaultProgramGenerator()
{
  Finalization::doFinalization();
  delete output_mgr_;
}

void
DefaultProgramGenerator::initialize()
{
  output_mgr_ = DefaultOutputMgr::CreateInstance();
  assert(output_mgr_);
}

void
DefaultProgramGenerator::goGenerator()
{
  output_mgr_->OutputHeader(argc_, argv_, seed_);

  GenerateAllTypes();
  GenerateAllFunctions();
  output_mgr_->Output();
}
