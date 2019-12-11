#include "register_mips_generator.h"
#include "debug.h"
#include <set>
#include <iostream>

// ���Ż���mips��������
vector<string> RegMipsGenerator::simple_dump() {
	get_symbol_items();
	dump_global_memory();
	cal_function_stack();
	function_sub_dumps();
	return object_code;
}

// step function
/*
 * ���ÿһ����Ԫʽ�ĸ����������Ƿ�����ͬһ������(block)
 * ���ÿ����Ԫʽ���γɰ�������SymbolItem���б�
*/
void RegMipsGenerator::get_symbol_items() {
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		SymbolItem* A = (*it)->OpA;
		SymbolItem* B = (*it)->OpB;
		SymbolItem* Result = (*it)->Result;
		// item �б�ע��
		if (A != NULL) item_set.insert(A);
		if (B != NULL) item_set.insert(B);
		if (Result != NULL) item_set.insert(Result);
	}
}

/*
 * ��.data�ε�ȫ�����ݿռ䶨��ã�������<SymbolItem, label>��ӳ���ϵ��Ŀǰ���ǣ�
 * 1. ȫ�ֱ���
 * 2. �ַ�������
 * 2. �ַ�������
*/
void RegMipsGenerator::dump_global_memory() {
	reg_mips_string(&object_code, ".data");
	// ����ȫ���ӽ��µı���
	for (auto it = item_set.begin(); it != item_set.end(); it++) {
		if ((*it)->type == SymbolItemType::_variable && (*it)->block->pre == NULL) {
			string label = "GLOBAL_" + (*it)->getname(false);
			int byte_space = 4;
			if ((*it)->isArray)
				byte_space = (*it)->array_size * 4;
			reg_mips_space_alloc(&object_code, label, byte_space);
			global_data_map[(*it)] = label;
		}
	}
	// ����ȫ���ӽ��µ��ַ�������
	for (auto it = item_set.begin(); it != item_set.end(); it++) {
		if ((*it)->type == SymbolItemType::temp_strcon) {
			string label = "STRCON_" + (*it)->getname(false);
			reg_mips_asciiz(&object_code, label, (*it)->strcon);
			global_data_map[(*it)] = label;
		}
	}
}

/*
 * 1. ��ʼ��mips_function�ࣨ�贫�뺯����SymbolItem���ú����������м��룬�ú���������SymbolItem��
 * 2. �ɶ�Ӧ��mips_function����㺯���ڲ�ƫ�ƺͺ�������ջ�ռ������С��
*/
void RegMipsGenerator::cal_function_stack() {
	// �������SymbolItem��vector
	vector<SymbolItem*> func_list = get_funcs(&middle_code);

	// ���ÿ����������mips_function��
	for (auto it = func_list.begin(); it != func_list.end(); it++) {
		vector<Quaternary*> quater_under_func = get_quaternary_by_function(*it, &middle_code);
		RegMipsFunction* mips_function = new RegMipsFunction(*it, quater_under_func, &global_data_map, quater_basicblock);
		// ����ӳ��
		global_function[*it] = mips_function;
		stack_usage[*it] = mips_function->stack_size();
	}
}

void RegMipsGenerator::function_sub_dumps() {
	reg_mips_string(&object_code, "");
	reg_mips_string(&object_code, ".text");
	reg_mips_string(&object_code, "jal main");
	reg_mips_string(&object_code, "li $v0, 10");
	reg_mips_string(&object_code, "syscall");

	// �������SymbolItem��vector
	vector<SymbolItem*> func_list = get_funcs(&middle_code);

	for (auto it = func_list.begin(); it != func_list.end(); it++) {
		vector<string> sub_dump = global_function[*it]->dump();
		for (auto itt = sub_dump.begin(); itt != sub_dump.end(); itt++) {
			reg_mips_string(&object_code, *itt);
		}
	}
}
