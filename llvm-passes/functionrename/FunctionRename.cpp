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
        StringRef Name = gv.getName();
       if (!gv.isDeclaration())
         // gv.setLinkage(GlobalValue::PrivateLinkage);
         gv.setName(Name + "_renamed");
      }
      
      for (auto it = M.alias_begin(); it != M.alias_end(); ++it)
      {
        GlobalAlias& ga = *it;
        StringRef Name = ga.getName();
        if (!ga.isDeclaration())
          //ga.setLinkage(GlobalValue::PrivateLinkage);
          ga.setName(Name + "_renamed");
      }

      // Rename all functions
      for (auto &F : M) {
        StringRef Name = F.getName();
        // Leave library functions alone because their presence or absence
        // could affect the behaviour of other passes.
        if (F.isDeclaration())
          continue;
        F.setName(Name + "_renamed");
      }
      return true;
    }

    std::string rename(std::string &Name)
    {
      std::string search_str = std::string("(");
      std::string suffix_str = std::string("_renamed");
      size_t i = Name.find_first_of(search_str);
      if (i == std::string::npos)
        Name.append(suffix_str);
      else
        Name.insert(i, suffix_str);
      return Name;
    }
  };
}

char FunctionRename::ID = 0;
static RegisterPass<FunctionRename> X("functionrename", "Function Rename Pass");
// ===-------------------------------------------------------==//
//
// Function Renamer - Renames all functions
//
