#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__

#include<vector>
#include<list>
#include<map>
#include<set>
#include "Quaternary.h"
#include "debug.h"

class BasicBlock {
public:
	BasicBlock(vector<Quaternary*> quaters);

	int id;
	vector<Quaternary*> quaters;

	// link
	set<BasicBlock*> pre;
	set<BasicBlock*> next;

	// 活跃变量分析支持
	set<SymbolItem*> active_in;
	set<SymbolItem*> active_out;
	set<SymbolItem*> active_def;
	set<SymbolItem*> active_use;

};

vector<BasicBlock*> generate_basic_blocks(vector<Quaternary*> middle, map<Quaternary*, BasicBlock*>* map);
void active_analysis(vector<BasicBlock*> block_list);

#endif // !__BASIC_GRAPH_H__
