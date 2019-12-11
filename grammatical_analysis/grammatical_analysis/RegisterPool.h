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

class RegMipsFunction;
class RegisterPool
{
public:
	RegisterPool(RegMipsFunction* func, vector<string>* mips);
	// SymbolItem�������_variable(global/local), _const(global/local), temp_normal, temp_const, temp_strcon
	/* �Ĵ������룺��Ĵ�������������2���������Ĵ������������1������Ĵ������� */
	map<SymbolItem*, string> request(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
	/* �ض��Ĵ������룺��ָ���ļĴ��������ǼĴ������еı��� */
	void specific_request(SymbolItem* item, string target);
	/* ����ӳ�� + ��д���еĻ�Ծ���� */
	void clear_all_and_dump_active(set<SymbolItem*> active_set);
	/* ����ӳ�� + ��д���б���������ȫ�֣� */
	void clear_all_and_dump_all();

private:
	// From Out
	RegMipsFunction* func;
	vector<string>* mips;

	// Inner
	map<SymbolItem*, string> symbol_register;			// Maintainer: flush_and_load
	map<string, SymbolItem*> register_symbol;			// Maintainer: flush_and_load
	list<string> free_list;								// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	list<string> allocated_list;						// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	map<string, int> dirty;								// ��λ Maintainer: temp_apply -> ��1�� flush_and_link -> ��0

	/* ����Ĵ���: ���н�ֹ��. forwrite - ����ļĴ����Ƿ���ҪΪдֵ���� */
	string temp_apply(SymbolItem* item, set<string> forbid, bool forwrite=false);
	/* ��ϴ��ǰ�Ĵ���(����)����������ֵ������). writeback - flushʱ�Ƿ�д��, noload - �Ƿ���ҪԤ�ȼ���ֵ
	 * flush = unmap + writeback
	 * link = map + link
	 */
	bool flush_and_link(string reg, SymbolItem* newitem, bool writeback=true, bool noload=false);
};

#endif // !__REGISTER_POOL_H__

