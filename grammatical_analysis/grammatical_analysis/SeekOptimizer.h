#ifndef __SEEK_OPTIMIZER_H__
#define __SEEK_OPTIMIZER_H__

#include "Optimizer.h"
#include "BasicBlock.h"

class SeekOptimizer : public Optimizer
{
public:
	SeekOptimizer(vector<Quaternary*> list, map<Quaternary*, BasicBlock*> quater_basic_block) 
		:Optimizer("Seek", list) {
		this->quater_basic_block = quater_basic_block;
		printf("[Seek Optimizer] Begin Seek Optimizing count(middle) = %d \n", int(middle_code.size()));
		preprocess();
		optimizer();
		printf("[Seek Optimizer] End Seek Optimizing count(middle) = %d \n", int(optimized_code.size()));
	}
protected:
	map<Quaternary*, BasicBlock*> quater_basic_block;
	void preprocess();
	void optimizer();
};

#endif // !__SEEK_OPTIMIZER_H__
