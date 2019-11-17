
#ifndef __BLOCK_H__
#define __BLOCK_H__

#include<vector>
#include<map>
#include "SymbolItem.h"

using namespace std;

/* ���ű�ṹ���Ľ�㣬����ǰ׺ָ�롣 */
class Block {
public:
	Block();
	// ���νṹ֧��
	Block* pre;
	vector<Block*> nexts;

	// ���Ա�ǩ
	SymbolItem* func_head;
	map<SymbolItemType, vector<SymbolItem*>> items;
};

#endif // !__BLOCK_H__

