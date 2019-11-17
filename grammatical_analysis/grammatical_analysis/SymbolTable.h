#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__
#include <string>
#include <vector>
#include <map>
#include "SymbolItem.h"
#include "Block.h"

using namespace std;
class SymbolTable
{
public:
	// members
	Block* root;
	Block* present;

	// methods
	SymbolTable();									// build and initial

	void add_one_block();							// 基于当前节点层级，新增一个块
	void exit_present_block();						// 离开当前节点层级，返回上一个块
	Block* get_present_block();						// 返回当前节点结构体的引用，for - 查询与更新
};


// ========== 符号表的辅助操作 ==========
void* find_indefr(Block* block, string name);
void insert_one_record(Block* block, SymbolItem* record);
void update_function_head(Block* block, SymbolItem* head);
void* find_indefr_current(Block* block, string name);
SymbolItem* find_param(Block* block, string name, bool recur);
SymbolItem* find_func(Block* block, string name, bool recur);
#endif // !__SYMBOL_TABLE_H__

