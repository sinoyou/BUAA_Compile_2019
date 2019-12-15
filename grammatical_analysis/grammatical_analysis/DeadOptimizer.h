#ifndef __DEAD_OPTIMIZER_H__
#define __DEAD_OPTIMIZER_H__

#include "Optimizer.h"
#include "BasicBlock.h"

class DeadOptimizer : public Optimizer
{
public:
	DeadOptimizer(vector<Quaternary*> list, map<Quaternary*, BasicBlock*> quater_basic_block)
		:Optimizer("Dead", list) {
		this->quater_basic_block = quater_basic_block;
		printf("[Dead Optimizer] Begin Dead Optimizing count(middle) = %d \n", int(middle_code.size()));
		preprocess();
		optimizer();
		printf("[Dead Optimizer] End Dead Optimizing count(middle) = %d \n", int(optimized_code.size()));
	}
protected:
	map<Quaternary*, BasicBlock*> quater_basic_block;
	void preprocess();
	void optimizer();
};

#endif // !__DEAD_OPTIMIZER_H__
