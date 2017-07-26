#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Metadata.h"

using namespace llvm;

namespace {
  
  struct FunctionRename : public ModulePass {
    static char ID; // Pass identification
    FunctionRename() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      for (auto it = M.global_begin(); it != M.global_end(); ++it)
      {
        GlobalVariable& gv = *it;
        if (!gv.isDeclaration())
          gv.setLinkage(GlobalValue::LinkerPrivateLinkage);
      }
      
      for (auto it = M.alias_begin(); it != M.alias_end(); ++it)
      {
        GlobalAlias& ga = *it;
        if (!ga.isDeclaration())
          ga.setLinkage(GlobalValue::LinkerPrivateLinkage);
      }

      // Rename all functions
      for (auto &F : M) {
        StringRef Name = F.getName();
        // Leave library functions alone because their presence or absence
        // could affect the behaviour of other passes.
        if (F.isDeclaration())
          continue;
        F.setLinkage(GlobalValue::WeakAnyLinkage);
        F.setName(Name + "_renamed");
      }
      return true;
    }
  };
}

char FunctionRename::ID = 0;
static RegisterPass<FunctionRename> X("functionrename", "Function Rename Pass");
// ===-------------------------------------------------------==//
//
// Function Renamer - Renames all functions
//
