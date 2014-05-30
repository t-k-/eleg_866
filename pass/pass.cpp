#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

	ConstantInt* fold_constants(unsigned operation, ConstantInt *op1, ConstantInt *op2){
		switch(operation){
			case Instruction::Add:
				return ConstantInt::get(op1->getContext(), op1->getValue() + op2->getValue());
			case Instruction::Sub:
				return ConstantInt::get(op1->getContext(), op1->getValue() + op2->getValue());
			case Instruction::Mul:
				return ConstantInt::get(op1->getContext(), op1->getValue() * op2->getValue());
			case Instruction::UDiv:
				return ConstantInt::get(op1->getContext(), op1->getValue().udiv(op2->getValue()));
			case Instruction::SDiv:
				return ConstantInt::get(op1->getContext(), op1->getValue().sdiv(op2->getValue()));

		}
		return NULL;
	}


	class const_fold : public FunctionPass {
		public:
			static char ID;
			const_fold() : FunctionPass(ID) { } 

			void runOnBasicBlock(BasicBlock &blk)
			{
				for (BasicBlock::iterator i = blk.begin(); i != blk.end(); ++i) {
					Instruction *ii= dyn_cast<Instruction>(i);

					if(ii->getNumOperands() == 2 && isa<Constant>(ii->getOperand(0)) 
									&& isa<Constant>(ii->getOperand(1))) {
						Value *v = fold_constants(ii->getOpcode(), 
										dyn_cast<ConstantInt>(ii->getOperand(0)), 
										dyn_cast<ConstantInt>(ii->getOperand(1)));
						if(v) {  
							int flag=0;
							if (i == blk.end())
								flag=1;
							BasicBlock::iterator n = i; n++;
							i->replaceAllUsesWith(v);
							i->eraseFromParent();
							if (flag)
									break;
							i = n;
						}
					}
				}
			}

			virtual bool runOnFunction(Function &func)
			{
				for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i)
					runOnBasicBlock(*i);
				return true;
			}
	};

	char const_fold::ID = 0;
	RegisterPass<const_fold> X("const-fold", "my constant folding pass");
}
