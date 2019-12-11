#ifndef __REG_MIPS_GENERATOR_H__
#define __REG_MIPS_GENERATOR_H__

#include <map>
#include <vector>
#include "register_mips_function.h"
#include "SymbolItem.h"
#include "Block.h"
#include "Quaternary.h"
#include "register_mips_code_utils.h"
#include <set>


class RegMipsGenerator
{
private:
	// Input and Output
	vector<Quaternary*> middle_code;
	vector<string> object_code;

	// SymbolItem 抽象
	set<SymbolItem*> item_set;

	// 关系映射表
	map<SymbolItem*, string> global_data_map;			// 全局数据的SymbolItem对应的目标代码标签
	map<SymbolItem*, int> stack_usage;					// 函数SymbolItem对应的函数空间大小
	map<SymbolItem*, RegMipsFunction*> global_function;	// 函数SymbolItem对应的子目标代码生成器

	// 基本块
	map<Quaternary*, BasicBlock*> quater_basicblock;

	// step function
	void get_symbol_items();
	void dump_global_memory();
	void cal_function_stack();
	void function_sub_dumps();

public:
	RegMipsGenerator(vector<Quaternary*> middle_code, map<Quaternary*, BasicBlock*> quater_basicblock) :
		middle_code(middle_code), quater_basicblock(quater_basicblock) {}
	vector<string> simple_dump();
};


#endif // !__MIPS_GENERATOR_H__

