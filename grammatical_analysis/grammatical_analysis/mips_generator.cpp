#include "mips_generator.h"
#include "debug.h"
#include <set>
#include <iostream>

// 无优化的mips代码生成
vector<string> MipsGenerator::simple_dump() {
	get_symbol_items();
	dump_global_memory();
	cal_function_stack();
	function_sub_dumps();
	return object_code;
}

// step function
/*
 * 检查每一个四元式的各个操作数是否属于同一个函数(block)
 * 拆分每个四元式，形成包含所有SymbolItem的列表
*/
void MipsGenerator::get_symbol_items() {
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		SymbolItem* A = (*it)->OpA;
		SymbolItem* B = (*it)->OpB;
		SymbolItem* Result = (*it)->Result;
		// item 列表注册
		if (A != NULL) item_set.insert(A);
		if (B != NULL) item_set.insert(B);
		if (Result != NULL) item_set.insert(Result);
	}
}

/*
 * 将.data段的全局数据空间定义好，并生成<SymbolItem, label>的映射关系，目前主要考虑：
 * 1. 全局变量
 * 2. 字符串常量
*/
void MipsGenerator::dump_global_memory() {
	mips_string(&object_code, ".data");
	// 遍历全局视角下的变量
	for (auto it = item_set.begin(); it != item_set.end(); it++) {
		if ((*it)->type == SymbolItemType::_variable && (*it)->block->pre == NULL) {
			string label = "GLOBAL_" + (*it)->getname(false);
			int byte_space = 4;
			if ((*it)->isArray)
				byte_space = (*it)->array_size * 4;
			mips_space_alloc(&object_code, label, byte_space);
			global_data_map[(*it)] = label;
		}
	}
	// 遍历全局视角下的字符串常量
	for (auto it = item_set.begin(); it != item_set.end(); it++) {
		if ((*it)->type == SymbolItemType::temp_strcon) {
			string label = "STRCON_" + (*it)->getname(false);
			mips_asciiz(&object_code, label, (*it)->strcon);
			global_data_map[(*it)] = label;
		}
	}
}

/*
 * 1. 初始化mips_function类（需传入函数的SymbolItem，该函数的所有中间码，该函数的所有SymbolItem）
 * 2. 由对应的mips_function类计算函数内部偏移和函数整体栈空间请求大小。
*/
void MipsGenerator::cal_function_stack() {
	// 求出函数SymbolItem的vector
	vector<SymbolItem*> func_list = get_funcs(&item_set);

	// 针对每个函数创建mips_function类
	for (auto it = func_list.begin(); it != func_list.end(); it++) {
		vector<Quaternary*> quater_under_func = get_quaternary_by_function(*it, &middle_code);
		MipsFunction* mips_function = new MipsFunction(*it, quater_under_func, &global_data_map);
		// 加入映射
		global_function[*it] = mips_function;
		stack_usage[*it] = mips_function->stack_size();
	}
}

void MipsGenerator::function_sub_dumps() {
	mips_string(&object_code, "");
	mips_string(&object_code,".text");
	mips_string(&object_code,"jal main");
	mips_string(&object_code, "li $v0, 10");
	mips_string(&object_code, "syscall");
	for (auto it = global_function.begin(); it != global_function.end(); it++) {
		vector<string> sub_dump = (*it).second->dump();
		for (auto itt = sub_dump.begin(); itt != sub_dump.end(); itt++) {
			mips_string(&object_code, *itt);
		}
	}
}
