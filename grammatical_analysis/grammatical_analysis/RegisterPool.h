#ifndef __REGISTER_POOL_H__
#define __REGISTER_POOL_H__

#include<map>
#include<vector>
#include<list>
#include<set>
#include "SymbolItem.h"
#include "Quaternary.h"
#include "register_mips_code_utils.h"
#include "register_mips_function.h"
#include "BasicBlock.h"

#define REPEAT_WEIGHT 5

class RegMipsFunction;
class RegisterPool
{
public:
	RegisterPool(RegMipsFunction* func, 
		vector<Quaternary*>middle,
		map<Quaternary*, BasicBlock*> quater_basic_block,
		vector<string>* mips);
	// SymbolItem�������_variable(global/local), _const(global/local), temp_normal, temp_const, temp_strcon
	/* �Ĵ������룺��Ĵ�������������2���������Ĵ������������1������Ĵ������� */
	map<SymbolItem*, string> request(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
	/* ȫ�ּĴ������أ�����Ҫͳһ���� */
	void global_dump();
	/* �ض��Ĵ������룺��ָ���ļĴ��������ǼĴ������еģ�����ȫ�ֺ���ʱ�� */
	void specific_request(SymbolItem* item, string target);
	/* ����ӳ�� + ��д��������ʱ���� - Called when leaving basic block of the function. */
	void clear_all_and_dump_temp_active(set<SymbolItem*> active_set);
	/* ����ӳ�� + ��д��������� - Called when leaving this function level. */
	void clear_all_and_dump_all_active(set<SymbolItem*> active_set);

	// �̶��Ĵ���
	const string a0 = "$a0";
	const string zero = "$0";
	const string ret = "$v0";
	const string array_temp = "$k0";
	const string ra = "$ra";


private:
	// From Out
	RegMipsFunction* func;
	vector<Quaternary*> middle;
	map<Quaternary*, BasicBlock*> quater_basic_block;	// ��Ԫʽ�������ӳ��
	vector<string>* mips;

	// Inner
	map<SymbolItem*, string> symbol_register;			// Maintainer: flush_and_load
	map<string, SymbolItem*> register_symbol;			// Maintainer: flush_and_load
	list<string> free_list;								// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	list<string> allocated_list;						// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	map<string, int> dirty;								// ��λ Maintainer: temp_apply -> ��1�� flush_and_link -> ��0
	map<SymbolItem*, string> global_map;			// Generated when initializing, read only.

	/* ������ʱ�Ĵ���: ���н�ֹ��. forwrite - ����ļĴ����Ƿ���ҪΪдֵ���� */
	string apply(SymbolItem* item, set<string> forbid, bool forwrite=false);
	
	/* ��ϴ��ǰ�Ĵ���(����)����������ֵ������). writeback - flushʱ�Ƿ�д��, noload - �Ƿ���ҪԤ�ȼ���ֵ
	 * flush = unmap + writeback
	 * link = map + load
	 */
	bool flush_and_link(string reg, SymbolItem* newitem, bool writeback=true, bool noload=false);
};

/* ���ü�����ȫ�ּĴ���Ԥ�� 
 * middle: ��ǰ�����ľֲ��м���
 * quater_basic_block: ��Ԫʽ��������ӳ��
 * max_save_reg: ��������ı���Ĵ��������Բ�ʹ���꣩
 */
map<SymbolItem*, string> get_global_map(string func_head,
	vector<Quaternary*> middle, map<Quaternary*, BasicBlock*> quater_basic_block,
	vector<string>* max_save_reg);

#endif // !__REGISTER_POOL_H__

