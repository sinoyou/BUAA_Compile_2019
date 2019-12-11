#include "BasicBlock.h"
#include "debug.h"
#include "Quaternary.h"
#include <iostream>

string print_symbolitem_set(string set_name, set<SymbolItem*> set);
string print_block_set(string set_name, set<BasicBlock*> set);

/* 将基本块信息导出到文件中 */
void dump_basic_blocks(vector<BasicBlock*> block_list, string filename) {
	ofstream f;
	f.open(filename);
	for (auto it = block_list.begin(); it != block_list.end(); it++) {
		auto block = *it;
		// Block Head Info
		f << ">>>> Block No. " << block->id << endl;
		f << print_block_set("PRE ", (*it)->pre) << endl;
		f << print_block_set("NEXT", (*it)->next) << endl;
		f << print_symbolitem_set("DEF", (*it)->active_def) << endl;
		f << print_symbolitem_set("USE", (*it)->active_use) << endl;
		f << print_symbolitem_set("IN", (*it)->active_in) << endl;
		f << print_symbolitem_set("OUT", (*it)->active_out) << endl;

		// Block Quaters Body
		auto quater_str = PrintQuater(&(block->quaters));
		for (auto jt = quater_str.begin(); jt != quater_str.end(); jt++)
			f << *jt << endl;

		// Block Tail with 3 empty \n
		f << endl << endl << endl;
	}
	f.close();
}

string print_symbolitem_set(string set_name, set<SymbolItem*> set) {
	string print = set_name + "{ ";
	for (auto it = set.begin(); it != set.end(); it++) {
		print = print + (*it)->getname() + ", ";
	}
	print = print + "}";
	return print;
}

string print_block_set(string set_name, set<BasicBlock*> set) {
	string print = set_name + "{ ";
	for (auto it = set.begin(); it != set.end(); it++) {
		print = print + to_string((*it)->id) + ", ";
	}
	print = print + "}";
	return print;
}
