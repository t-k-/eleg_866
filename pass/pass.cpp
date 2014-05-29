#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {

	bool replay = false; //in case the first statement is optimized. replay the first statement for optimization.

	void makeTheChanges(BasicBlock::iterator &ib, Value* val){
		ib->replaceAllUsesWith(val);
		BasicBlock::iterator j = ib;
		++ib;
		j->eraseFromParent();
		if(ib != ib->getParent()->begin()){
			--ib;
		}else{
			replay = true;
		}
	}


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


	class LocalOpts : public FunctionPass{

		public:
			static char ID;
			LocalOpts() : FunctionPass(ID) { } //default constructor
			~LocalOpts() {  }

			/* Method to print details for a all the functions in a module */
			void printFunctionDetails(Function &F) {
				int arg_size = F.arg_size();
				int num_call_sites = F.getNumUses();
				int num_basic_blocks = F.size(); //defined in value class.
				/*count the number of instructions.*/
				int number_of_instructions = 0;
				for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I)
					number_of_instructions += 1;

				errs() << F.getName() <<": arguments=" << arg_size << " call sites=" <<  num_call_sites << " basic blocks=" << num_basic_blocks << " instructions=" << number_of_instructions << "\n\n";
			}



			void runOnBasicBlock(BasicBlock &blk){
				for (BasicBlock::iterator i = blk.begin(), e = blk.end(); i != e; ++i){
					if(replay){
						i = blk.begin(); 
						replay = false;
					}

					Instruction *ii= dyn_cast<Instruction>(i);


					//Constant Folding
					if(ii->getNumOperands() == 2 && isa<Constant>(ii->getOperand(0)) && isa<Constant>(ii->getOperand(1))){
						Value *v1 = fold_constants(ii->getOpcode(), dyn_cast<ConstantInt>(ii->getOperand(0)), dyn_cast<ConstantInt>(ii->getOperand(1)));
						if(v1) {  
							makeTheChanges(i,v1);
							continue;
						}
					}

				}
			}

			virtual bool runOnFunction(Function &func){
				errs() << "Function: " << func.getName() << "\n";
				printFunctionDetails(func);
				for (Function::iterator i = func.begin(), e = func.end(); i != e; ++i){ //iterating over the basic blocks                    
					runOnBasicBlock(*i);
					print_basic_block_info(*i);
				}
				return true;
			}


			// We don't modify the program, so we preserve all analyses
			virtual void getAnalysisUsage(AnalysisUsage &AU) const {
				AU.setPreservesAll();
			}

		private:
			//helper functions
			void print_basic_block_info(BasicBlock &b){
				errs() << "Basic block (name=" << b.getName() << ") has " << b.size() << " instructions.\n";

			}
	};

	char LocalOpts::ID = 0;

	static void registerMyPass(const PassManagerBuilder &,
			PassManagerBase &PM) {
		PM.add(new LocalOpts());
	}
	RegisterStandardPasses
		RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
				registerMyPass);

	RegisterPass<LocalOpts> X("const-fold", "hahah");

}
