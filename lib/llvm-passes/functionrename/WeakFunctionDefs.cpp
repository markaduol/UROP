#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

  struct WeakFunctionDefs : public ModulePass {
    static char ID;
    WeakFunctionDefs() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      for (auto it = M.begin(); it != M.end(); ++it)
      {
        Function &F = *it;
        if (F.isDeclaration())
          continue;
        F.setLinkage(GlobalValue::WeakAnyLinkage);
      }
      // Transform occurred
      return true;
    }
  };
}

char WeakFunctionDefs::ID = 0;
static RegisterPass<WeakFunctionDefs> X("weakfunctiondefs", "Make Function Definitions weak");
