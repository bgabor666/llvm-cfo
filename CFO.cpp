#include "llvm/ADT/MapVector.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "cfo"

STATISTIC(CFOCounter, "Counts number of functions greeted");

namespace {
  static Function *getMyConstantFunction(Module &M, LLVMContext &Context) {
    Function *MyConstantF = cast<Function>(M.getOrInsertFunction("myConstant", Type::getInt32Ty(Context)));

    BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", MyConstantF);

    Value *NumberOfTheBeast = ConstantInt::get(Type::getInt32Ty(Context), 666);

    //BasicBlock *RetBB = BasicBlock::Create(Context, "return", MyConstantF);

    //ReturnInst::Create(Context, NumberOfTheBeast, RetBB);
    ReturnInst::Create(Context, NumberOfTheBeast, BB);

    return MyConstantF;
  }

  struct CFO : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    SmallVector<Instruction*, 128> DeleteList;

    CFO() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      for (auto &F : M) {
        StringRef Name = F.getName();
        errs() << "Function<" << Name << ">" << ":\n";
        runOnFunction(F);
      }

#if 0
      for (auto I : DeleteList) {
        Function* MyConstantFunction = getMyConstantFunction(M, M.getContext());
        Instruction *MyConstantCallInst = CallInst::Create(MyConstantFunction);
        ReplaceInstWithInst(I, MyConstantCallInst);
      }
#endif

      for (auto I : DeleteList) {
        errs() << "I->getNumUses: " << I->getNumUses() << "\n";

        SmallVector<Value*, 4> OperandVector;

        errs() << "Operands:" << "\n";

        for (unsigned OperatorIndex = 0, NumOperands = I->getNumOperands(); OperatorIndex != NumOperands; ++OperatorIndex) {
          I->getOperand(OperatorIndex)->dump();
          I->getOperand(OperatorIndex)->getType()->dump();
          OperandVector.push_back(I->getOperand(OperatorIndex));
        }

        errs() << "OperandVector size:" << OperandVector.size() << "\n";

        Function* ParametricSubFunction = M.getFunction("parametricSub");
        //Instruction *ParametricSubCallInst = CallInst::Create(ParametricSubFunction);
        Instruction *ParametricSubCallInst = CallInst::Create(ParametricSubFunction, OperandVector);
        ReplaceInstWithInst(I, ParametricSubCallInst);
      }

      M.dump();

      return true;
    }

    bool runOnFunction(Function &F) {
      errs() << "Function name: " << F.getName() << "\n";

      for (auto &BB : F) {
        errs() << "  BasicBlock<" << BB.getName() << ">" << ":\n";

        for (auto &I : BB) {
          if (F.getName() != "parametricSub" && I.getOpcode() == Instruction::Sub) {
            errs() << "Found sub instruction...";
            DeleteList.push_back(&I);
            errs() << "getNumOperands(): " <<  I.getNumOperands() << "\n";
          }

          errs() << "  ";
          I.dump();
        }
        errs() << "\n";
      }

      return !DeleteList.empty();
    }
  };
}

char CFO::ID = 0;
static RegisterPass<CFO>
Z("cfo", "CFO World Pass (ModulePass)");

