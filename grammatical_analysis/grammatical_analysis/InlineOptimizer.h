#ifndef __INLINE_OPTIMIZER_H__
#define __INLINE_OPTIMIZER_H__

#include "Optimizer.h"
#include <set>

class InlineOptimizer: public Optimizer
{
public:
	InlineOptimizer(vector<Quaternary*> list) :Optimizer("Inline", list){
		printf("[Inline Optimizer] Begin Inline Optimizing \n");
		preprocess();
		optimizer();
		printf("[Inline Optimizer] End Inline Optimizing count(middle) = %d \n", optimized_code.size());
	}
protected:
	void preprocess();
	void optimizer();
};

vector<Quaternary*> inline_generator(SymbolItem* caller, SymbolItem* func,
	vector<SymbolItem*> paras, SymbolItem* ret, vector<Quaternary*>* middle);
set<SymbolItem*> called_func_in_one_func(SymbolItem* func, vector<Quaternary*>* middle);
bool inline_check(SymbolItem* func, SymbolItem* func_inline, vector<Quaternary*>* middle);

#endif // !__INLINE_OPTIMIZER_H__
