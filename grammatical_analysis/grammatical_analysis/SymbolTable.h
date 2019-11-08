#include <string>
#include <vector>

using namespace std;

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

// 符号表中的项类型统计
/*
 1. int|char型变量或数组。交由ParamRecord管理。
 2. FuncHead函数头。与每个块意义对应着。
 */

struct ParamRecord {
	string name;					
	string type;					// int, char
	bool isConst;					// true / false
};

struct FuncHead {
	string name;
	string returnType;				// int, char, void
	vector<string> paramsList;		
};

/* 符号表结构树的结点，含有前缀指针。 */
class Block {
public:
	// 树形结构支持
	Block* pre;	
	vector<Block*> nexts;

	// 属性标签
	FuncHead func_head;
	vector<ParamRecord> records;
};

class SymbolTable
{
public:
	// members
	Block* root;
	Block* present;

	// methods
	SymbolTable();									// build and initial

	void add_one_block(string name);				// 基于当前节点层级，新增一个块
	void exit_present_block();						// 离开当前节点层级，返回上一个块
	Block& get_present_block();						// 返回当前节点结构体的引用，for - 查询与更新
};

#endif // !__SYMBOL_TABLE_H__

