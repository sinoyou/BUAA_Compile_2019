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

	// SymbolItem ����
	set<SymbolItem*> item_set;

	// ��ϵӳ���
	map<SymbolItem*, string> global_data_map;			// ȫ�����ݵ�SymbolItem��Ӧ��Ŀ������ǩ
	map<SymbolItem*, int> stack_usage;					// ����SymbolItem��Ӧ�ĺ����ռ��С
	map<SymbolItem*, MipsFunction*> global_function;	// ����SymbolItem��Ӧ����Ŀ�����������

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

