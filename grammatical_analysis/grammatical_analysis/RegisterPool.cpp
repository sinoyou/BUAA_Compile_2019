#include "RegisterPool.h"
#include "debug.h"
#include <algorithm>
#include <cmath>

RegisterPool::RegisterPool(RegMipsFunction* func,
	vector<Quaternary*> middle,
	map<Quaternary*, BasicBlock*> quater_basic_block,
	vector<string>* mips) {
	this->func = func;
	this->middle = middle;
	this->quater_basic_block = quater_basic_block;
	this->mips = mips;
	string free_temp[13] = { "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
	string free_save[11] = { "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};
	// Preprocess for Save-Type Registers
	vector<string> max_save_reg;
	for (int i = 0; i < 8; i++) {
		dirty[free_save[i]] = 0;
		max_save_reg.push_back(free_save[i]);
	}
	this->global_map = get_global_map(func->get_funchead()->getname(), middle, quater_basic_block, &max_save_reg);

	// Preprocess for Temp-Type Registers
	for (int i = 0; i < 10; i++) {
		dirty[free_temp[i]] = 0;
		free_list.push_back(free_temp[i]);
	}
	for (auto it = max_save_reg.begin(); it != max_save_reg.end(); it++)
		free_list.push_back(*it);
}

map<SymbolItem*, string> RegisterPool::request(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	set<string> forbid;
	map<SymbolItem*, string> alloc;
	if (A != NULL) {
		alloc[A] = apply(A, forbid, false);
		forbid.insert(alloc[A]);
	}
	if (B != NULL) {
		alloc[B] = apply(B, forbid, false);
		forbid.insert(alloc[B]);
	}
	if (Result != NULL) {
		alloc[Result] = apply(Result, forbid, true);
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

void RegisterPool::global_load() {
	for (auto it = global_map.begin(); it != global_map.end(); it++) {
		flush_and_link(it->second, it->first, false, false);
	}
}

void RegisterPool::clear_all_and_dump_temp_active(set<SymbolItem*> active_set) {
	auto it = allocated_list.begin();
	reg_mips_comment(mips, " XXX Begin clear and dump TEMP active XXX");
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
	reg_mips_comment(mips, " XXX End clear and dump TEMP active XXX");
}

void RegisterPool::clear_all_and_dump_all_active(set<SymbolItem*> active_set) {
	auto it = allocated_list.begin();
	reg_mips_comment(mips, " XXX Begin clear and dump ALL active XXX");
	// 临时寄存器
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
	// 全局寄存器
	for (auto it = global_map.begin(); it != global_map.end(); it++) {
		auto item = it->first;
		auto reg = it->second;
		if (item->block->pre == NULL) {
			ERROR_REGISTER(func->get_funchead()->getname(), "Global variable should not be freed at GLOBAL regs.");
		}
		if (dirty[reg] == 1 && active_set.find(item) != active_set.end())
			flush_and_link(reg, NULL, true, false);
		else
			flush_and_link(reg, NULL, false, false);
	}
	reg_mips_comment(mips, " XXX End clear and dump ALL active XXX");
}

// private
// 寄存器申请，临时：FIFO，全局：引用计数
string RegisterPool::apply(SymbolItem* item, set<string> forbid, bool forwrite) {
	string applied_reg;
	// if already have map relation
	if (symbol_register.find(item) != symbol_register.end()) {
		// unique check
		if (register_symbol[symbol_register[item]] != item)
			DEBUG_PRINT("[ERROR] register_symbol and symbol_register relation not unique.");
		applied_reg = symbol_register[item];
	}
	else {
		string target = "";
		// 临时寄存器
		if (global_map.find(item) == global_map.end()) {
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
			// 检查与替换加载临时寄存器
			if (target == "") {
				DEBUG_PRINT("[ERROR] Can not find situable register(free/allocated).");
			}
			else {
				allocated_list.push_back(target);
				if (dirty[target] == 1) {
					flush_and_link(target, item, true, forwrite);
				}
				else {
					flush_and_link(target, item, false, forwrite);
				}
			}
		}
		// 全局寄存器
		else {
			ERROR_REGISTER(func->get_funchead()->getname(), "Global register should be allocated at head of func.");
		}
		applied_reg = target;			// 返回经过查询/加载（临时或全局）得到的寄存器
	}
	if (forwrite)
		dirty[applied_reg] = 1;
	return applied_reg;
}

// private
bool RegisterPool::flush_and_link(string reg, SymbolItem* newitem, bool writeback, bool noload) {
	// if register is used then unmap and (write back if).
	if (register_symbol.find(reg) != register_symbol.end()) {
		auto old_item = register_symbol[reg];
		// if old_item is temp_variable or variable
		if (writeback && 
			(old_item->type == SymbolItemType::temp_normal || old_item->type == SymbolItemType::_variable)) {
			DEBUG_REGISTER(func->get_funchead()->getname(), "Dump" + reg + ": " + old_item->getname());
			if (dirty[reg] == 0)
				ERROR_REGISTER(func->get_funchead()->getname(), "[WARNING] Reg seems clean, really need write back?");
			reg_mips_save(mips, reg, old_item, func);
		}
		int cnt1 = symbol_register.erase(old_item);
		int cnt2 = register_symbol.erase(reg);
		dirty[reg] = 0;
		
		if (!(cnt1 == 1 && cnt2 == 1)) {
			string info = "Symbol <-> Register Unmap Number is not correct." + to_string(cnt1) + " and " + to_string(cnt2);
			ERROR_REGISTER(func->get_funchead()->getname(), info);
		}
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

bool cmp(const pair<SymbolItem*, int>& a, const pair<SymbolItem*, int>& b) {
	return a.second > b.second;
}

map<SymbolItem*, string> get_global_map(string func_name, 
	vector<Quaternary*> middle, map<Quaternary*, BasicBlock*> quater_block,
	vector<string>* max_save_reg) {
	int loop_level = 1;
	map<SymbolItem*, int> level;
	map<SymbolItem*, set<BasicBlock*>> item_block_set;
	// step1：引用权重统计
	// step2: 跨块统计
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == QuaterType::SetLabel) {
			auto label = (*it)->OpA;
			if (label->tags.find(Tag::repeat_begin) != label->tags.end())
				loop_level++;
			else if (label->tags.find(Tag::repeat_end) != label->tags.end())
				loop_level--;
		}
		else {
			auto A = (*it)->OpA, B = (*it)->OpB, Result = (*it)->Result;
			if (A != NULL) {
				level[A] = (level.find(A) == level.end()) ? pow(REPEAT_WEIGHT, (loop_level - 1)) :
					pow(REPEAT_WEIGHT, (loop_level - 1)) + level[A];
				item_block_set[A] = (item_block_set.find(A) == item_block_set.end()) ?
					set<BasicBlock*>() : item_block_set[A];
				item_block_set[A].insert(quater_block[*it]);

			}
			if (B != NULL) {
				level[B] = (level.find(B) == level.end()) ? pow(REPEAT_WEIGHT, (loop_level - 1)) :
					pow(REPEAT_WEIGHT, (loop_level - 1)) + level[B];
				item_block_set[B] = (item_block_set.find(B) == item_block_set.end()) ?
					set<BasicBlock*>() : item_block_set[B];
				item_block_set[B].insert(quater_block[*it]);
			}
		}
	}

	// step3: 权重排序与寄存器分配
	map<SymbolItem*, string> reg_map;
	vector<pair<SymbolItem*, int>> vec(level.begin(), level.end());
	std::sort(vec.begin(), vec.end(), cmp);
	for (auto it = vec.begin(); it != vec.end(); it ++) {
		// 非全局 && 跨块变量 && 权重大于1 && 还有剩余的s型寄存器 && 类型为变量(var/temp_normal）
		if (it->first->block->pre != NULL && 
			(it->first->type == temp_normal || it->first->type == _variable) &&
			item_block_set[it->first].size() > 1 &&
			it->second > 1 &&
			max_save_reg->size() > 0) {
			auto reg = max_save_reg->back();
			max_save_reg->pop_back();
			reg_map[it->first] = reg;
			string info = reg + " <=== " + "Name: " + it->first->getname() + " Weight: " + to_string(it->second);
			DEBUG_REGISTER(func_name, info);
		}
		else {
			string info = "X   <=== Name: " + it->first->getname() + " Weight: " + to_string(it->second);
			DEBUG_REGISTER(func_name, info);
		}
	}

	return reg_map;
}
