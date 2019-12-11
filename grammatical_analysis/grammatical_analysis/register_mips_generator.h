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

	// SymbolItem ����
	set<SymbolItem*> item_set;

	// ��ϵӳ���
	map<SymbolItem*, string> global_data_map;			// ȫ�����ݵ�SymbolItem��Ӧ��Ŀ������ǩ
	map<SymbolItem*, int> stack_usage;					// ����SymbolItem��Ӧ�ĺ����ռ��С
	map<SymbolItem*, RegMipsFunction*> global_function;	// ����SymbolItem��Ӧ����Ŀ�����������

	// ������
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

