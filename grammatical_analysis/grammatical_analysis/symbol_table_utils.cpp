
#include "SymbolTable.h"
#include "debug.h"

/*
 * 检查当前块的视域下某个变量名是否已经存在
 * 检查范围：当前块及其前继的（本函数名、变量\常量名、子函数名）
*/
void* find_indefr(Block* block, string name)
{
	while (block != NULL) {
		// 块自身域名是否重复
		if (block->func_head.name == name)
			return &(block->func_head);
		// 块内变量/常量是否重复
		vector<ParamRecord>::iterator it = block->records.begin();
		while (it != block->records.end()) {
			if (it->name == name)
				return &(*it);
			it++;
		}
		// 块内块名是否重复
		vector<Block*>::iterator it1 = block->nexts.begin();
		while (it1 != block->nexts.end()) {
			if ((*it1)->func_head.name == name)
				return &((*it1)->func_head);
			it1++;
		}
		block = block->pre;
	}
	return NULL;
}

/**
 * 向当前块中插入一条记录
*/
void insert_one_record(Block* block, ParamRecord record)
{
	void* p = find_indefr(block, record.name);
	// 检查，避免重复插入
	if (p == NULL) {
		block->records.push_back(record);
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

/**
 * 更新当前块中的函数头
*/
void update_function_head(Block* block, FuncHead head) {
	void* p = find_indefr(block, head.name);
	if (p == NULL) {
		block->func_head = head;
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

