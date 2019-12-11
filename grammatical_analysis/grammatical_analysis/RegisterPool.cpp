#include "RegisterPool.h"
#include "debug.h"

RegisterPool::RegisterPool(RegMipsFunction* func, vector<string>* mips) {
	this->func = func;
	this->mips = mips;
	string free[20] = { "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
		"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7" };
	for (int i = 0; i < 18; i++) {
		dirty[free[i]] = 0;
		free_list.push_back(free[i]);
	}
}

map<SymbolItem*, string> RegisterPool::request(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	set<string> forbid;
	map<SymbolItem*, string> alloc;
	if (A != NULL) {
		alloc[A] = temp_apply(A, forbid, false);
		forbid.insert(alloc[A]);
	}
	if (B != NULL) {
		alloc[B] = temp_apply(B, forbid, false);
		forbid.insert(alloc[B]);
	}
	if (Result != NULL) {
		alloc[Result] = temp_apply(Result, forbid, true);
	}
	return alloc;
}

void RegisterPool::specific_request(SymbolItem* item, string target) {
	if (dirty.find(target) != dirty.end()) {
		ERROR_REGISTER(func->get_funchead()->getname(), "Specific Register can not be in reg pool");
	}
	else {
		if (symbol_register.find(item) != symbol_register.end())
			reg_mips_move(mips, symbol_register[item], target);
		else
			reg_mips_load(mips, target, item, func);
	}
}

void RegisterPool::clear_all_and_dump_active(set<SymbolItem*> active_set) {
	auto it = allocated_list.begin();
	reg_mips_comment(mips, " XXX Begin clear and dump active XXX");
	while (allocated_list.size() > 0) {
		auto reg = *it;
		auto item = register_symbol[reg];
		// 若为全局变量或活跃变量, 且出现过写操作
		if (item->block->pre == NULL || active_set.find(item) != active_set.end()) {
			if (dirty[reg] == 1) {
				flush_and_link(reg, NULL, true, false);
			}
			else {
				flush_and_link(reg, NULL, false, false);
			}
		}
		else
			flush_and_link(reg, NULL, false, false);
		
		it = allocated_list.erase(it);
		free_list.push_back(reg);
	}
	reg_mips_comment(mips, " XXX End clear and dump active XXX");
}

void RegisterPool::clear_all_and_dump_all() {
	reg_mips_comment(mips, " XXX Begin clear and dump all XXX");
	auto it = allocated_list.begin();
	while (allocated_list.size() > 0) {
		auto reg = *it;
		auto item = register_symbol[reg];
		// 出现过写操作的寄存器才写回
		if (dirty[reg] == 1) {
			flush_and_link(reg, NULL, true, false);
		}
		else {
			flush_and_link(reg, NULL, false, false);
		}

		it = allocated_list.erase(it);
		free_list.push_back(reg);
	}
	reg_mips_comment(mips, " XXX End clear and dump all XXX");
}

// private
// 寄存器申请，临时：FIFO，全局：引用计数
string RegisterPool::temp_apply(SymbolItem* item, set<string> forbid, bool forwrite) {
	string applied_reg;
	// if already have map relation
	if (symbol_register.find(item) != symbol_register.end()) {
		// unique check
		if (register_symbol[symbol_register[item]] != item)
			DEBUG_PRINT("[ERROR] register_symbol and symbol_register relation not unique.");
		applied_reg = symbol_register[item];
	}
	// if not
	else {
		string target = "";
		// 在空闲列表中寻找
		if (free_list.size() > 0) {
			auto it = free_list.begin();
			while (it != free_list.end() && forbid.find(*it) != forbid.end())
				it++;
			if (it != free_list.end()) {
				target = *it;
				free_list.erase(it);
			}
		}
		// 在已分配列表中寻找替换
		if (target == "") {
			auto it = allocated_list.begin();
			while (it != allocated_list.end() && forbid.find(*it) != forbid.end())
				it++;
			if (it != allocated_list.end()) {
				target = *it;
				allocated_list.erase(it);
			}
		}
		// 检查与替换加载
		if (target == "") {
			DEBUG_PRINT("[ERROR] Can not find situable register(free/allocated).");
		}
		else {
			allocated_list.push_back(target);
			flush_and_link(target, item, true, forwrite);
		}
		applied_reg = target;
	}
	if (forwrite)
		dirty[applied_reg] = 1;
	return applied_reg;
}

// private
bool RegisterPool::flush_and_link(string reg, SymbolItem* newitem, bool writeback, bool noload) {
	// if register is dirty then unmap and (write back).
	if (register_symbol.find(reg) != register_symbol.end()) {
		auto old_item = register_symbol[reg];
		dirty[reg] = 0;
		// if old_item is temp_variable or variable
		if (writeback && 
			(old_item->type == SymbolItemType::temp_normal || old_item->type == SymbolItemType::_variable)) {
			DEBUG_REGISTER(func->get_funchead()->getname(), "Dump" + reg + ": " + old_item->getname());
			reg_mips_save(mips, reg, old_item, func);
		}
		int cnt1 = symbol_register.erase(old_item);
		int cnt2 = register_symbol.erase(reg);
		if (!(cnt1 == 1 && cnt2 == 1)) {
			string info = "Symbol <-> Register Unmap Number is not correct." + to_string(cnt1) + " and " + to_string(cnt2);
			ERROR_REGISTER(func->get_funchead()->getname(), info);
		}
	}
	else {
		ERROR_REGISTER(func->get_funchead()->getname(), "Unknown Reg " + reg);
	}
	// if new item exist
	if (newitem != NULL) {
		DEBUG_REGISTER(func->get_funchead()->getname(), "Load" + reg + ": " + newitem->getname());
		// unique check
		if (symbol_register.find(newitem) != symbol_register.end()) {
			char buf[150];
			sprintf(buf, "[ERROR] Item %s already Mapped to %s", newitem->getname().c_str(), symbol_register[newitem].c_str());
			ERROR_REGISTER(func->get_funchead()->getname(),string(buf));
		}
		if(noload == false)
			reg_mips_load(mips, reg, newitem, func);
		symbol_register[newitem] = reg;
		register_symbol[reg] = newitem;
	}
	return true;
}
