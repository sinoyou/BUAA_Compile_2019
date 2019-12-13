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
	// SymbolItem申请对象：_variable(global/local), _const(global/local), temp_normal, temp_const, temp_strcon
	/* 寄存器申请：向寄存器池提请至多2个操作数寄存器申请和至多1个结果寄存器申请 */
	map<SymbolItem*, string> request(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
	/* 全局寄存器加载：必须要统一进行 */
	void global_dump();
	/* 特定寄存器申请：所指定的寄存器不能是寄存器池中的，包括全局和临时。 */
	void specific_request(SymbolItem* item, string target);
	/* 擦除映射 + 回写所有脏临时变量 - Called when leaving basic block of the function. */
	void clear_all_and_dump_temp_active(set<SymbolItem*> active_set);
	/* 擦除映射 + 回写所有脏变量 - Called when leaving this function level. */
	void clear_all_and_dump_all_active(set<SymbolItem*> active_set);

	// 固定寄存器
	const string a0 = "$a0";
	const string zero = "$0";
	const string ret = "$v0";
	const string array_temp = "$k0";
	const string ra = "$ra";


private:
	// From Out
	RegMipsFunction* func;
	vector<Quaternary*> middle;
	map<Quaternary*, BasicBlock*> quater_basic_block;	// 四元式与基本块映射
	vector<string>* mips;

	// Inner
	map<SymbolItem*, string> symbol_register;			// Maintainer: flush_and_load
	map<string, SymbolItem*> register_symbol;			// Maintainer: flush_and_load
	list<string> free_list;								// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	list<string> allocated_list;						// Maintainer: temp_apply, clear_all_and_dump_active, clear_all_and_dump_all
	map<string, int> dirty;								// 脏位 Maintainer: temp_apply -> 置1， flush_and_link -> 置0
	map<SymbolItem*, string> global_map;			// Generated when initializing, read only.

	/* 申请临时寄存器: 带有禁止符. forwrite - 申请的寄存器是否需要为写值而用 */
	string apply(SymbolItem* item, set<string> forbid, bool forwrite=false);
	
	/* 冲洗当前寄存器(如有)，并加载新值（如有). writeback - flush时是否写回, noload - 是否需要预先加载值
	 * flush = unmap + writeback
	 * link = map + load
	 */
	bool flush_and_link(string reg, SymbolItem* newitem, bool writeback=true, bool noload=false);
};

/* 引用计数：全局寄存器预绑定 
 * middle: 当前函数的局部中间码
 * quater_basic_block: 四元式与基本块的映射
 * max_save_reg: 最大数量的保存寄存器（可以不使用完）
 */
map<SymbolItem*, string> get_global_map(string func_head,
	vector<Quaternary*> middle, map<Quaternary*, BasicBlock*> quater_basic_block,
	vector<string>* max_save_reg);

#endif // !__REGISTER_POOL_H__

