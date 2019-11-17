#include "SymbolTable.h"
#include "debug.h"
#include <iostream>
/*
 * 检查当前块的视域下某个变量名是否已经存在
 * 检查范围：当前块及其前继的（本函数名、变量\常量名、子函数名）
*/
void* find_indefr(Block* block, string name)
{
	void* p1 = find_param(block, name, true);
	void* p2 = find_func(block, name, true);
	if (p1) {
		return p1;
	}
	else if (p2) {
		return p2;
	}
	else {
		return NULL;
	}
}
/*
 * 检查当前块的视域下某个变量名是否已经存在
 * 检查范围：只有当前块
*/
void* find_indefr_current(Block* block, string name)
{
	void* p1 = find_param(block, name, false);
	void* p2 = find_func(block, name, false);
	if (p1) {
		return p1;
	}
	else if (p2) {
		return p2;
	}
	return NULL;
}

SymbolItem* find_param(Block* block, string name, bool recur=true)
{	
	while (block != NULL) {
		// 1 const
		auto ite = block->items.find(SymbolItemType::_const);
		if (ite != block->items.end()) {
			vector<SymbolItem*>::iterator it = ite->second.begin();
			while (it != ite->second.end()) {
				if ((*it)->name == name) {
					return *it;
				}
				it += 1;
			}
		}
		// 2 var
		ite = block->items.find(SymbolItemType::_variable);
		if (ite != block->items.end()) {
			vector<SymbolItem*>::iterator it = ite->second.begin();
			while (it != ite->second.end()) {
				if ((*it)->name == name) {
					return *it;
				}
				it += 1;
			}
		}
		block = block->pre;
		if (!recur)
			break;
	}
	return NULL;
}

SymbolItem* find_func(Block* block, string name, bool recur=true) {
	while (block != NULL) {
		// 块自身域名是否重复
		if (block->func_head != NULL && block->func_head->name == name) {
			return block->func_head;
		}
		// 块内块名是否重复
		vector<Block*>::iterator it1 = block->nexts.begin();
		while (it1 != block->nexts.end()) {
			if ((*it1)->func_head != NULL && (*it1)->func_head->name == name) {
				return (*it1)->func_head;
			}
			it1++;
		}
		block = block->pre;
		if (!recur)
			break;
	}
	return NULL;
}

/**
 * 向当前块中插入一条记录
*/
void insert_one_record(Block* block, SymbolItem* record)
{
	void* p = find_indefr_current(block, record->name);
	// 检查，避免重复插入
	if (p == NULL) {
		if (block->items.find(record->type) == block->items.end()) {
			block->items[record->type] = vector<SymbolItem*>();
		}
		record->block = block;
		block->items[record->type].push_back(record);
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

/**
 * 更新当前块中的函数头
*/
void update_function_head(Block* block, SymbolItem* head) {
	void* p = find_indefr_current(block, head->name);
	if (p == NULL) {
		if (head->type == SymbolItemType::function) {
			head->block = block;
			block->func_head = head;
		}
		else {
			DEBUG_PRINT("[ERROR] Func head type haven't matched.")
		}
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

