#ifndef __BLOCK_H__
#define __BLOCK_H__

#include<vector>
#include<map>
#include<string>
#include "SymbolItem.h"

using namespace std;
/* 符号表结构树的结点，含有前缀指针。 */
class Block {
public:
	Block();
	// 树形结构支持
	Block* pre;
	vector<Block*> nexts;

	// 属性标签
	SymbolItem* func_head;
	map<SymbolItemType, vector<SymbolItem*>> items;

	// 中间代码生成辅助标签
	map<string, int> label_map;
	map<string, int> temp_map;
};

#endif // !__BLOCK_H__

