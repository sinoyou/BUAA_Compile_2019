#ifndef __MIPS_FUNCTION_H__
#define __MIPS_FUNCTION_H__

#include<vector>
#include "SymbolItem.h"
#include<map>
#include "Quaternary.h"
#include "mips_code_utils.h"

class MipsFunction
{
private:
	vector<string> sub_object_code;
	SymbolItem* func_head;
	vector<Quaternary*> quater_list;
	map<SymbolItem*, int> offset_map;
	map<SymbolItem*, string>* global_data;

	// ��ʽ��������ı���
	int ret_offset;
	int ra_offset;

public:
	// MipsFunction ��ʼ����������������������ʽ����������ʽ�����������$spָ���ƫ����
	MipsFunction(SymbolItem* func_head, vector<Quaternary*> func_quater, map<SymbolItem*, string>* global_data);

	// ��ȡ����Ҫ��ջ�ռ��С - ��ʽ���� + ��ʽ����
	int stack_size();
	// �ж���λ�ã������ض�Ӧ�ĵ�ַ���ַ��ǩ
	void get_addr(SymbolItem* item, bool *local, int* offset, string* data_label);
	// �����˺������ֵ�Ŀ�����
	vector<string> dump();
};

#endif // !__MIPS_FUNCTION_H__


