#include "BasicBlock.h"
#include <list>
#include <algorithm>
#include <iterator>


/* 活跃变量分析 */
void get_def_and_use(BasicBlock* block);
bool set_cmp(set<SymbolItem*> A, set<SymbolItem*> B);
void active_analysis(vector<BasicBlock*> blocks) {
	// step 1: 获取每个block的def和use
	for (auto it = blocks.begin(); it != blocks.end(); it++)
		get_def_and_use(*it);
	// step 2: 活跃变量分析 -  BFS
	auto start = blocks.end();
	while(start > blocks.begin()) {
		start = start - 1;
		string info = "Start a new bfs with " + (*start)->id;
		DEBUG_BASICBLOCK(info.c_str());
		list<BasicBlock*> queue;
		queue.push_back(*start);
		while (queue.size() > 0) {
			auto head = queue.front();
			queue.pop_front();
			set<SymbolItem*> temp1;
			set<SymbolItem*> temp2;
			// temp1 = out[B] - def[B]
			std::set_difference(
				head->active_out.begin(), head->active_out.end(),
				head->active_def.begin(), head->active_def.end(),
				std::inserter(temp1, temp1.end()));
			// temp2 = use[B] union (out[B] - def[B])
			std::set_union(
				head->active_use.begin(), head->active_use.end(),
				temp1.begin(), temp1.end(),
				std::inserter(temp2,temp2.end()));
			if (!set_cmp(head->active_in, temp2)) {
				set<SymbolItem*> check;
				std::set_difference(head->active_in.begin(), head->active_in.end(),
					temp2.begin(), temp2.end(),
					std::inserter(check,check.end()));
				if (check.size() > 0)
					DEBUG_PRINT("[WARNING] Active In is Shrinking which is not expected.");
				head->active_in = temp2;
				// 遍历其前继
				for (auto pre = head->pre.begin(); pre != head->pre.end(); pre++) {
					set<SymbolItem*> check;
					std::set_union(head->active_in.begin(), head->active_in.end(),
						(*pre)->active_out.begin(), (*pre)->active_out.end(),
						std::inserter(check, check.end()));
					// 如有更新才进入
					if (!set_cmp((*pre)->active_out, check)) {
						char buf[150];
						sprintf(buf, "Block %d enter queue by %d", (*pre)->id, head->id);
						DEBUG_BASICBLOCK(buf);
						queue.push_back(*pre);
						(*pre)->active_out = check;
					}
				}
			}
		}
	}
}

bool set_cmp(set<SymbolItem*> A, set<SymbolItem*> B) {
	auto it1 = A.begin();
	auto it2 = B.begin();
	while (it1 != A.end() && it2 != B.end()) {
		if (*it1 != *it2)
			return false;
		it1++;
		it2++;
	}
	if (it1 != A.end() || it2 != B.end())
		return false;
	return true;
}

/* 获取活跃变量分析所需要的Def集合和Use集合 */
void get_def_and_use(BasicBlock* block) {
	for (auto it = block->quaters.begin(); it != block->quaters.end(); it++)
	{
		// OpA
		if ((*it)->OpA != NULL && ((*it)->OpA->type == _variable || (*it)->OpA->type == temp_normal)) {
			// 须早于块内任意一处定义
			if (block->active_def.find((*it)->OpA) == block->active_def.end())
				block->active_use.insert((*it)->OpA);
		}
		// OpB
		if ((*it)->OpB != NULL && ((*it)->OpB->type == _variable || (*it)->OpB->type == temp_normal)) {
			// 须早于块内任意一处定义
			if (block->active_def.find((*it)->OpB) == block->active_def.end())
				block->active_use.insert((*it)->OpB);
		}
		// Result
		if ((*it)->Result != NULL && ((*it)->Result->type == _variable || (*it)->Result->type == temp_normal)) {
			// 须早于块内任意一处使用
			if (block->active_use.find((*it)->Result) == block->active_use.end())
				block->active_def.insert((*it)->Result);
		}
	}
}