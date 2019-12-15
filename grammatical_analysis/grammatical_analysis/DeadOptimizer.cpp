#include "DeadOptimizer.h"
#include "Block.h"
#include <map>


void DeadOptimizer::preprocess() {

}

void DeadOptimizer::optimizer() {
	map<SymbolItem*, int> usemap_in_block;
	BasicBlock* basic_block = NULL;
	list<Quaternary*> list;
	auto it = middle_code.end();
	while(it > middle_code.begin()) {
		it--;
		auto  result = (*it)->Result;
		auto t = quater_basic_block[*it];
		auto qtype = (*it)->type;
		// 换块
		if (t != basic_block) {
			usemap_in_block.clear();
		}
		basic_block = t;

		// 结果数存在 && 类型为变量 && 为函数局部变量
		if (result != NULL && 
			(result->type == SymbolItemType::_variable || result ->type == SymbolItemType::temp_normal) && 
			(qtype == Add || qtype == Sub || qtype == Mult || qtype == Div || qtype == Assign ) && 
			result->block->pre != NULL) {
			// 若从块尾到该符号没有被使用过 & 该符号不是活跃变量
			if (usemap_in_block.find(result) == usemap_in_block.end() &&
				basic_block->active_out.find(result) == basic_block->active_out.end()) {
				// 该四元式是死代码，删除
				vector<Quaternary*> temp;
				temp.push_back(*it);
				string info = "[DeadOptimizer] Delete Quater " + PrintQuater(&temp)[0];
				cout << info << endl;
				continue;
			}
			// 若有该符号之前的使用记录，予以清除
			usemap_in_block.erase(result);
		}
		// 操作数使用情况注册
		auto OpA = (*it)->OpA;
		auto OpB = (*it)->OpB;
		if (OpA != NULL)
			usemap_in_block[OpA] = 1;
		if (OpB != NULL)
			usemap_in_block[OpB] = 1;
		list.push_front(*it);
	}

	for (auto it = list.begin(); it != list.end(); it++)
		optimized_code.push_back(*it);
}