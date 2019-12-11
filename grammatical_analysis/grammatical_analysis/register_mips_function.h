#ifndef __REG_MIPS_FUNCTION_H__
#define __REG_MIPS_FUNCTION_H__

#include<vector>
#include "SymbolItem.h"
#include<map>
#include "Quaternary.h"
#include "register_mips_code_utils.h"
#include "BasicBlock.h"
#include "RegisterPool.h"

class RegisterPool;
class RegMipsFunction
{
private:
	vector<string> sub_object_code;
	SymbolItem* func_head;
	vector<Quaternary*> quater_list;
	map<SymbolItem*, int> offset_map;
	map<SymbolItem*, string>* global_data;
	map<Quaternary*, BasicBlock*> quater_basicblock;
	RegisterPool* register_pool;

	// ��ʽ��������ı���
	int ret_offset;
	int ra_offset;

public:
	// MipsFunction ��ʼ����������������������ʽ����������ʽ�����������$spָ���ƫ����
	RegMipsFunction(SymbolItem* func_head, 
		vector<Quaternary*> func_quater, 
		map<SymbolItem*, string>* global_data,
		map<Quaternary*, BasicBlock*> quater_basicblock);
	// ��ȡ����ͷ
	SymbolItem* get_funchead() { return func_head; };

	// ��ȡ����Ҫ��ջ�ռ��С - ��ʽ���� + ��ʽ����
	int stack_size();
	// �ж���λ�ã������ض�Ӧ�ĵ�ַ���ַ��ǩ
	void get_addr(SymbolItem* item, bool* local, int* offset, string* data_label);
	// �����˺������ֵ�Ŀ�����
	vector<string> dump();
};

#endif // !__MIPS_FUNCTION_H__


