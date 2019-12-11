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
		printf("[Inline Optimizer] End Inline Optimizing count(middle) = %d \n", int(optimized_code.size()));
	}
protected:
	void preprocess();
	void optimizer();
};

/* 给定函数及其调用者，将函数的中间码转换为inline后返回 */
vector<Quaternary*> inline_generator(SymbolItem* caller, SymbolItem* func,
	vector<SymbolItem*> paras, SymbolItem* ret, vector<Quaternary*>* middle);
/* 返回一个函数中所调用的函数集合 */
set<SymbolItem*> called_func_in_one_func(SymbolItem* func, vector<Quaternary*>* middle);
/* 检查是否适合作为inline函数 */
bool inline_check(SymbolItem* func, SymbolItem* func_inline, vector<Quaternary*>* middle);

#endif // !__INLINE_OPTIMIZER_H__
