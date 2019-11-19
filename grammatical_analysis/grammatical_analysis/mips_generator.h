#ifndef __MIPS_GENERATOR_H__
#define __MIPS_GENERATOR_H__

#include <map>
#include <vector>
#include "mips_function.h"
#include "SymbolItem.h"
#include "Block.h"
#include "Quaternary.h"
#include "mips_code_utils.h"
#include <set>


class MipsGenerator
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
	map<SymbolItem*, MipsFunction*> global_function;	// 函数SymbolItem对应的子目标代码生成器

	// step function
	void get_symbol_items();
	void dump_global_memory();
	void cal_function_stack();
	void function_sub_dumps();

public:
	MipsGenerator(vector<Quaternary*> middle_code):middle_code(middle_code){}
	vector<string> simple_dump();
};


#endif // !__MIPS_GENERATOR_H__

