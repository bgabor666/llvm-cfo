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
  struct CFO : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid

    typedef typename std::vector<const Instruction*> VectorType;

    CFO() : FunctionPass(ID) {}

    unsigned getHash(const Instruction& inst) {
      unsigned hash = 0;
      hash = inst.getOpcode() << 20;
      hash |= inst.getNumOperands() << 16;
      for (User::const_op_iterator oi = inst.op_begin(), on = inst.op_end(); oi != on; ++oi)
        hash |= (hash & 0xffff0000) | ((hash & 0xffff) << 4) | (intptr_t(oi->get()) & 0xffff);
      return hash;
    }

    bool runOnFunction(Function &F) override {
      MapVector<unsigned, VectorType> map;
      //SetVector<BasicBlock*> visited;
      MapVector<unsigned, VectorType> siblings;
      MapVector<unsigned, VectorType> families;

      ++CFOCounter;
      Function::BasicBlockListType& bbs = F.getBasicBlockList();
      for (Function::const_iterator bbi = bbs.begin(), bbe = bbs.end(); bbi != bbe; ++bbi) {
        errs() << "BB";
        const BasicBlock& bb = *bbi;
        //visited.insert

        const_pred_iterator pi = pred_begin(&bb), pe = pred_end(&bb);
        int pn = 0;
        for (; pi != pe; ++pi)
          ++pn;

        succ_const_iterator si = succ_begin(&bb), se = succ_end(&bb);
        int sn = 0;
        for (; si != se; ++si)
          ++sn;

        errs() << " (pred: " << pn << ", succ: " << sn << ")\n";

        const BasicBlock::InstListType& insts = bb.getInstList();
        for (BasicBlock::const_iterator ii = insts.begin() , ie = insts.end(); ii != ie; ++ii) {
          const Instruction& inst = *ii;
          unsigned hash = getHash(inst);
          errs() << "II: " << hash << " - " << inst << "\n";

          map[hash].push_back(&inst);
        }
      }

      errs() << "CFO: ";
      errs().write_escaped(F.getName()) << '\n';

      for (MapVector<unsigned, VectorType>::const_iterator bi = map.begin(), ei = map.end(); bi != ei; ++bi) {
        if (bi->second.size() < 2)
          continue;
        errs() << "Hash: " << bi->first << "\n";
        errs() << "Instructions: " << "\n";
        const VectorType& v = bi->second;
        errs() << "v.size(): " << v.size() << "\n";
        for (int i = 0, n = v.size(); i < n; ++i) {
          const Instruction* inst = v[i];
          const BasicBlock* parent = inst->getParent();
          inst->dump();
        }
      }
      return false;
    }
  };
}

char CFO::ID = 0;
static RegisterPass<CFO> X("cfo", "CFO World Pass");

namespace {
  struct CFO2 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    CFO2() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++CFOCounter;
      errs() << "CFO2: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char CFO2::ID = 0;
static RegisterPass<CFO2>
Y("cfo2", "CFO World Pass (with getAnalysisUsage implemented)");


// Rewrite CFO pass
namespace {
  struct CFO3 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid

    typedef typename std::vector<const Instruction*> VectorType;

    CFO3() : FunctionPass(ID) {}

    unsigned getHash(const Instruction& inst) {
      unsigned hash = 0;
      hash = inst.getOpcode() << 20;
      hash |= inst.getNumOperands() << 16;
      for (User::const_op_iterator oi = inst.op_begin(), on = inst.op_end(); oi != on; ++oi)
        hash |= (hash & 0xffff0000) | ((hash & 0xffff) << 4) | (intptr_t(oi->get()) & 0xffff);
      return hash;
    }

    bool runOnFunction(Function &F) override {
      MapVector<unsigned, VectorType> map;
      //SetVector<BasicBlock*> visited;
      MapVector<unsigned, VectorType> siblings;
      MapVector<unsigned, VectorType> families;

      ++CFOCounter;
      Function::BasicBlockListType& bbs = F.getBasicBlockList();
      for (Function::const_iterator bbi = bbs.begin(), bbe = bbs.end(); bbi != bbe; ++bbi) {
        errs() << "BB";
#if 0
        const BasicBlock& bb = *bbi;
        //visited.insert

        const_pred_iterator pi = pred_begin(&bb), pe = pred_end(&bb);
        int pn = 0;
        for (; pi != pe; ++pi)
          ++pn;

        succ_const_iterator si = succ_begin(&bb), se = succ_end(&bb);
        int sn = 0;
        for (; si != se; ++si)
          ++sn;

        errs() << " (pred: " << pn << ", succ: " << sn << ")\n";
#endif

        const BasicBlock::InstListType& insts = bbi->getInstList();
        for (BasicBlock::const_iterator ii = insts.begin() , ie = insts.end(); ii != ie; ++ii) {
          const Instruction& inst = *ii;
          unsigned hash = getHash(inst);
          errs() << "II: " << hash << " - " << inst << "\n";

          map[hash].push_back(&inst);
        }
      }

      errs() << "CFO: ";
      errs().write_escaped(F.getName()) << '\n';

      for (MapVector<unsigned, VectorType>::const_iterator bi = map.begin(), ei = map.end(); bi != ei; ++bi) {
        if (bi->second.size() < 2)
          continue;
        errs() << "Hash: " << bi->first << "\n";
        errs() << "Instructions: " << "\n";
        const VectorType& v = bi->second;
        errs() << "v.size(): " << v.size() << "\n";
        for (int i = 0, n = v.size(); i < n; ++i) {
          const Instruction* inst = v[i];
          const BasicBlock* parent = inst->getParent();
          inst->dump();
        }
      }
      return false;
    }
  };
}

char CFO3::ID = 0;
//static RegisterPass<CFO3>
//Z("cfo3", "CFO World Pass (with getAnalysisUsage implemented)");


// Rewrite CFO pass to inherit from ModulePass
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

  struct CFO4 : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    SmallVector<Instruction*, 128> DeleteList;

    CFO4() : ModulePass(ID) {}

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
        SmallVector<Value*, 4> OperandVector;
        errs() << "Operands:" << "\n";
        for (unsigned OperatorIndex = 0, NumOperands = I->getNumOperands(); OperatorIndex != NumOperands; ++OperatorIndex) {
          I->getOperand(OperatorIndex)->dump();
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
          if (F.getName() == "parametricSub" && I.getOpcode() == Instruction::Sub) {
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

char CFO4::ID = 0;
static RegisterPass<CFO4>
Z("cfo4", "CFO World Pass (ModulePass)");

