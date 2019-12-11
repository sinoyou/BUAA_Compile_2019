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
	// SymbolItem申请对象：_variable(global/local), _const(global/local), temp_normal, temp_const, temp_strcon
	/* 寄存器申请：向寄存器池提请至多2个操作数寄存器申请和至多1个结果寄存器申请 */
	map<SymbolItem*, string> request(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
	/* 特定寄存器申请：所指定的寄存器不能是寄存器池中的，包括全局和临时。 */
	void specific_request(SymbolItem* item, string target);
	/* 擦除映射 + 回写所有的临时变量 */
	void clear_all_and_dump_active(set<SymbolItem*> active_set);
	/* 擦除映射 + 回写所有变量（包括全局） */
	void clear_all_and_dump_all();

	// 固定寄存器
	const string a0 = "$a0";
	const string zero = "$0";
	const string ret = "$v0";
	const string array_temp = "$k0";
	const string ra = "$ra";


private:
	// From Out
	RegMipsFunction* func;
	vector<string>* mips;

	// Inner
	map<SymbolItem*, string> symbol_register;			// Maintainer: flush_and_load
	map<string, SymbolItem*> register_symbol;			// Maintainer: flush_and_load
	list<string> free_list;								// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	list<string> allocated_list;						// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	map<string, int> dirty;								// 脏位 Maintainer: temp_apply -> 置1， flush_and_link -> 置0

	/* 申请临时寄存器: 带有禁止符. forwrite - 申请的寄存器是否需要为写值而用 */
	string temp_apply(SymbolItem* item, set<string> forbid, bool forwrite=false);
	/* 冲洗当前寄存器(如有)，并加载新值（如有). writeback - flush时是否写回, noload - 是否需要预先加载值
	 * flush = unmap + writeback
	 * link = map + load
	 */
	bool flush_and_link(string reg, SymbolItem* newitem, bool writeback=true, bool noload=false);
};

#endif // !__REGISTER_POOL_H__

