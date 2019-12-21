#include "SeekOptimizer.h"
#include "Block.h"

void SeekOptimizer::preprocess() {

}

// 计算赋值操作数能够传播的区间范围，如果不合理会直接返回起点。
vector<Quaternary*>::iterator copy_zone(vector<Quaternary*>::iterator start,
	vector<Quaternary*>::iterator end,
	map<Quaternary*, BasicBlock*> quater_block, 
	bool *check) {
	auto assigned = (*start)->Result;
	auto assigner = (*start)->OpA;
	auto it_assigned = start + 1;
	// 计算assigned变量理想传播区间
	while (it_assigned != end) {
		// 跨块了
		if (quater_block[*it_assigned] != quater_block[*start])
			break;
		// 产生赋值了
		if ((*it_assigned)->Result == assigned)
			break;
		it_assigned += 1;
	}
	// 计算assigner变量的有效区间（距离下一次被赋值）
	auto it_assigner = start + 1;
	while (it_assigner != end) {
		// 跨块了
		if (quater_block[*it_assigner] != quater_block[*start])
			break;
		// 产生赋值处
		if ((*it_assigner)->Result == assigner)
			break;
		it_assigner += 1;
	}
	
	// 判断：assigned的理想传播区间必须在assigner区间之内，否则不能进行赋值传播
	if (it_assigned <= it_assigner) {
		*check = true;
		return it_assigned;
	}
	else {
		*check = false;
		return start + 1;
	}
}

void SeekOptimizer::optimizer() {
	// Assign的操作数直接向下传播
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->type == QuaterType::Assign) {
			auto result = (*it)->Result;
			auto quater = *it;
			auto basic_block = quater_basic_block[quater];
			// 如果这个被赋值变量不活跃于其他块 & 局部
			if (basic_block->active_out.find(result) == basic_block->active_out.end()
				&& result->block->pre != NULL) {
				auto jt = it + 1;
				bool check = false;
				auto zone = copy_zone(it, middle_code.end(), quater_basic_block, &check);
				// copy_zone返回有效的区间，如果无效就不删除。
				if (check) {
					while (jt != zone) {
						if (quater_basic_block[*jt] != basic_block)				// 跳块了
							DEBUG_PRINT("[ERROR] Block should not be different here.");
						if ((*jt)->OpA == result)
							(*jt)->OpA = quater->OpA;
						if ((*jt)->OpB == result)
							(*jt)->OpB = quater->OpA;
						if ((*jt)->Result == result)							// 出现新的赋值, 放在赋值以后因为新赋值操作数可能是原来的
							break;
						jt++;
					}
					vector<Quaternary*> temp;
					temp.push_back(*it);
					cout << "[Seek Optimizer] Spread Assign " + PrintQuater(&temp)[0] << endl;
				}
				else {
					optimized_code.push_back(*it);
				}
			}
			else {
				optimized_code.push_back(*it);
			}
		}
		else {
			optimized_code.push_back(*it);
		}
	}


	// 优化二：Assign的结果数充当赋值的最后一步
	// BEFORE:
	// temp_normal = X calc_op Y
	// target = temp_normal
	// AFTER:
	// target = X calc_op Y
	auto optimize_code_copy = optimized_code;
	optimized_code.clear();
	for (auto it = optimize_code_copy.begin(); it != optimize_code_copy.end(); it++) {
		if ((*it)->type == QuaterType::Assign && it != optimize_code_copy.begin()) {
			auto it_pre = it - 1;
			// 只是一个赋值操作，并且衔接量是temp型
			if ((*it)->OpA == (*it_pre)->Result && (*it_pre)->Result->type == SymbolItemType::temp_normal) {
				// 不再添加，将尾巴上的运算结果换成(*it)->Result
				(*(optimized_code.end() - 1))->Result = (*it)->Result;
				vector<Quaternary*> temp;
				temp.push_back(*it);
				cout << "[Seek Optimizer] Delete Assign " + PrintQuater(&temp)[0] << endl;
				continue;
			}
		}

		optimized_code.push_back(*it);
	}
}