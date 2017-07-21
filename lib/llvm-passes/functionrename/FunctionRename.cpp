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
// TargetLibraryInfo not in llvm 3.4
// #include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/Argument.h"

using namespace llvm;

namespace {
  
  struct FunctionRename : public ModulePass {
    static char ID; // Pass identification
    FunctionRename() : ModulePass(ID) {}

    /* void getAnalysisUsage(AnalysisUsage &AU) const override {
      // A TargetLibraryInfoWrapperPass is required by this FunctionRename
      // pass
      AU.addRequired<TargetLibraryInfoWrapperPass>();
      AU.setPreservesAll();
    } */

    bool runOnModule(Module &M) override {
      // Rename all functions
      //const TargetLibraryInfo &TLI =
        //getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();
      for (auto &F : M) {
        StringRef Name = F.getName();
        // Leave library functions alone because their presence or absence
        // could affect the behaviour of other passes.
        if (F.isDeclaration())
          continue;

        generateTestDriver(F);
        F.setName(Name + "_renamed");
      }
      return true;
    }

    bool generateTestDriver(Function &F) 
    {
      // Get function arguments
      std::vector<Argument*> Arguments;
      for (auto Arg = F.arg_begin(); Arg != F.arg_end(); ++Arg) 
      {
        Type *ArgTy = Arg->getType();
        
        if (ArgTy->isFloatingPointTy()) 
        {
          //errs() << "Cannot test function: " << F.getName() << " (floating point arguments)\n";
          return false;
        }
        // Grab a pointer to the argument reference
        Argument *ArgPtr = &*Arg;
        Arguments.push_back(ArgPtr);
      }

      // Get function return type
      Type *RetType = F.getReturnType();
      if (RetType->Type::isVoidTy()) 
      {
        //errs() << "Cannot test function: " << F.getName() << " (void return type)\n";
        return false;
      }

      // Create and open output file
      /*
      std::string outfileName = "_" + F.getName().str() + "_test_driver.c";
      std::ofstream outfile (outfileName);
      if (!outfile.is_open())
        outfile.open(outfileName);
      
      errs() << "Test driver output file open: " << outfileName << "\n";
      // Write klee header
      outfile << "#include <klee/klee.h>\n\n";

      // Write beginning of main function body
      outfile << "int main(int argc, char **argv)\n{";

      // Declare two variables for each function argument
      for (auto& Arg : Arguments) 
      {
        Type *ArgTy = Arg->getType();

        generateArgumentCode(Arg, outfile);
      }
      // For each pair of argument variables, assume they are equal
      // For each pair of argument variables, call function and renamed function, 
      generateFunctionCode(F, Arguments, outfile);
      // and klee assert that results are equal
      outfile << "}";
      outfile.close();
      */
      return true;
    }

    /* Returns true if generated successfully */
    bool generateFunctionCode(Function &F, std::vector<Argument*>& Arguments, std::ofstream& outfile)
    {
      char *buffer = new char[1024];
      std::stringstream ss1;
      std::stringstream ss2;
      for (size_t i = 0; i < Arguments.size(); ++i)
      {
        if (i != 0)
        {
          ss1 << ", ";
          ss2 << ", ";
        }
        ss1 << Arguments[i]->getName().data() << "_1";
        ss2 << Arguments[i]->getName().data() << "_2";
      }
      const char* argStr1 = ss1.str().c_str();
      const char* argStr2 = ss2.str().c_str();
      // Pre: Function has not already been renamed when executing this code
      const char* funcStr = F.getName().str().c_str();
      sprintf(buffer, "\tklee_assert(%s(%s) == %s_renamed(%s))\n", funcStr, argStr1, funcStr, argStr2); 
      outfile << buffer;
      return true;
    }



    /* Returns true if generated successfully */
    bool generateArgumentCode(Argument *Arg, std::ofstream& outfile)
    {
      Type *ArgTy = Arg->getType();

      char *buffer = new char[1024];
      switch (ArgTy->getTypeID())
      {
        case Type::IntegerTyID:
          sprintf(buffer, "\tint %s_1, %s_2;\n", Arg->getName().data(), Arg->getName().data());
          break;
        case Type::StructTyID:
          sprintf(buffer, "\tstruct %s_1, %s_2;\n", Arg->getName().data(), Arg->getName().data());
          break;
        case Type::ArrayTyID:
          sprintf(buffer, "\t[] %s_1, %s_2;\n", Arg->getName().data(), Arg->getName().data());
          break;
        case Type::PointerTyID:
          sprintf(buffer, "\t* %s_1, %s_2;\n", Arg->getName().data(), Arg->getName().data());
          break;
        default:
          break;
      }
      if (buffer[0] != '\0')
      {
        outfile << buffer;
        strcpy(buffer, "");
      }
      // Make function arguments symbolic
      switch (ArgTy->getTypeID())
      {
        case Type::IntegerTyID:
        case Type::StructTyID:
        case Type::ArrayTyID:
          sprintf(buffer + strlen(buffer), "\tklee_make_symbolic(&%s_1, sizeof %s_1, \"%s_1\");\n", Arg->getName().data(), Arg->getName().data(), Arg->getName().data());
          sprintf(buffer + strlen(buffer), "\tklee_make_symbolic(&%s_2, sizeof %s_2, \"%s_2\");\n", Arg->getName().data(), Arg->getName().data(), Arg->getName().data());
          sprintf(buffer + strlen(buffer), "\tklee_assume(%s_1 == %s_2);\n", Arg->getName().data(), Arg->getName().data());
          break;
        case Type::PointerTyID:
          sprintf(buffer + strlen(buffer), "\tklee_make_symbolic(%s_1, sizeof %s_1, \"%s_1\");\n", Arg->getName().data(), Arg->getName().data(), Arg->getName().data());
          sprintf(buffer + strlen(buffer), "\tklee_make_symbolic(%s_2, sizeof %s_2, \"%s_2\");\n", Arg->getName().data(), Arg->getName().data(), Arg->getName().data());
          sprintf(buffer + strlen(buffer), "\tklee_assume(%s_1 == %s_2);\n", Arg->getName().data(), Arg->getName().data());
          break;
      }
      if (buffer[0] != '\0')
        outfile << buffer;
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
