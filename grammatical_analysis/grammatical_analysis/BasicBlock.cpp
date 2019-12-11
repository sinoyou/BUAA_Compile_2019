#include "BasicBlock.h"
#include "debug.h"
#include "Block.h"
#include <iostream>

static int basic_block_id_cnt = 0;

BasicBlock::BasicBlock(vector<Quaternary*> quaters) {
	basic_block_id_cnt++;
	this->id = basic_block_id_cnt;
	this->quaters = quaters;
}


/* 由原始中间码生成基本块
 * 1. 划分并生成基本块。
 * 2. 对每个中间代码生成<中间码，基本块>的映射。
 * 3. 寻找建立基本块之间的流图。
 */
void build_connection(BasicBlock* from, BasicBlock* to);
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle);
Quaternary* find_func_declar(SymbolItem* func, vector<Quaternary*> middle);
Quaternary* find_set_label(SymbolItem* label, vector<Quaternary*> middle);
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle);
vector<Quaternary*> find_func_call(SymbolItem* func, vector<Quaternary*> middle);

vector<BasicBlock*> generate_basic_blocks(vector<Quaternary*> middle, map<Quaternary*, BasicBlock*>* map)
{
	vector<BasicBlock*> block_list;

	// step1：寻找入口中间码
	// 规则一：第一句中间码；规则二：跳转语句的后一句；规则三：被跳转的标签位置。
	vector<int> entry_index;
	entry_index.push_back(0);
	for (auto it = middle.begin(); it != middle.end(); it++) {
		auto type = (*it)->type;
		if (type == FuncCall || type == FuncRet || type == Goto || type == Bz || type == Bnz) {
			if(it != middle.end() - 1)
				entry_index.push_back(it - middle.begin() + 1);
		}
		else if (type == FuncDeclar || (type == SetLabel && is_label_visited((*it)->OpA, middle))) {
			entry_index.push_back(it - middle.begin());
		}
	}
	// step2: 建立基本块和<Quater, BasicBlock>映射关系
	for (auto it = entry_index.begin(); it != entry_index.end(); it++) {
		int front = *it;
		int tail = (it == entry_index.end() - 1) ? middle.end() - middle.begin() : *(it + 1);
		vector<Quaternary*> temp;
		temp.insert(temp.end(), middle.begin() + front, middle.begin() + tail);
		if (temp.size() > 0) {
			BasicBlock* block = new BasicBlock(temp);
			block_list.push_back(block);
			DEBUG_BASICBLOCK("Block Established [" + to_string(front) + "," + to_string(tail - 1) + "]");
			// 建立映射
			for (auto jt = temp.begin(); jt != temp.end(); jt++) {
				if (map->find(*jt) != map->end()) {
					DEBUG_PRINT("[ERROR] Quater Already Exist in other BasicBlock");
				}
				(*map)[*jt] = block;
			}
		}
		else {
			string info = "[WARNING] Empty Block Appear at " + to_string(front);
			DEBUG_PRINT(info.c_str());
		}
	}
	// step3: 建立双向关系 - 从主动方开始
	for (auto it = block_list.begin(); it != block_list.end(); it++)
	{
		// 提取每个块中的最后一句中间码，判断类型
		auto last_quater = *((*it)->quaters.end() - 1);
		auto quater_type = last_quater->type;
		if (quater_type == QuaterType::Bz || quater_type == QuaterType::Bnz) {
			// 当前块 -> 后一个块 | 当前块 -> label所在的基本块
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
			build_connection(*it, (*map)[find_set_label(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::Goto) {
			// 当前块 -> label所在的基本块
			build_connection(*it, (*map)[find_set_label(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::FuncCall) {
			// 当前块 -> 后一个块
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
			// Ignore: 当前块 -> func头所在块
			// build_connection(*it, (*map)[find_func_declar(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::FuncRet) {
			// 当前块 -> next(i) for i in 所有调用该函数的基本块
			// Ignore - 分块与链接在函数内部进行
			/*
			auto called_quaters = find_func_call(last_quater->block->func_head, middle);
			for (auto jt = called_quaters.begin(); jt != called_quaters.end(); jt++)
			{
				auto t_block = (*map)[*jt];
				auto kt = find(block_list.begin(), block_list.end(), t_block);
				if (kt == block_list.end())
					DEBUG_PRINT("[ERROR] Block should be found here. ");
				if (kt != block_list.end() - 1)
					build_connection(*it, *(kt + 1));
			}
			*/
		}
		else {
			// 当前块 -> 下一个块
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
		}
	}
	// step4: 活跃变量分析
	active_analysis(block_list);

	return block_list;
}

/* 双向建立联系 */
void build_connection(BasicBlock* from, BasicBlock* to)
{
	from->next.insert(to);
	to->pre.insert(from);
}

/* 查询此label是否被访问过 */
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle)
{
	for (auto it = middle.begin(); it != middle.end(); it++){
		auto type = (*it)->type;
		if ((type == Bnz || type == Bz || type == Goto) && (*it)->OpA == label)
			return true;
	}
	return false;
}

/* 查询某个函数头的声明位置 */
Quaternary* find_func_declar(SymbolItem* func, vector<Quaternary*> middle)
{
	Quaternary* save = NULL;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == FuncDeclar && (*it)->Result == func) {
			if (save != NULL)
				DEBUG_PRINT("[ERROR] Find duplicated Func Declar Here.");
			save = *it;
		}
	}
	return save;
}

/* 查询某个label的SetLabel四元式 */
Quaternary* find_set_label(SymbolItem* label, vector<Quaternary*> middle)
{
	Quaternary* save = NULL;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == SetLabel && (*it)->OpA == label)
		{
			if (save != NULL)
				DEBUG_PRINT("[ERROR] Find Duplicated Set Label Quater.");
			save = *it;
		}
	}
	return save;
}

/* 查询某个函数头被调用的位置 */
vector<Quaternary*> find_func_call(SymbolItem* func, vector<Quaternary*> middle)
{
	vector<Quaternary*> func_call_list;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == FuncCall && (*it)->OpA == func)
			func_call_list.push_back(*it);
	}
	return func_call_list;
}