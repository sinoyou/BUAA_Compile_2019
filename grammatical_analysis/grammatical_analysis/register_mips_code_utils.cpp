#include "register_mips_function.h"
#include "register_mips_code_utils.h"
#include "debug.h"
#include <set>
#include <iostream>
#include <stdio.h>

#define MAXBUF 200

/* ---------------------------- OTHER --------------------------- */
SymbolItem* mod_judge(vector<Quaternary*>::iterator it, vector<Quaternary*>::iterator end) {
	if (it >= end - 2)
		return NULL;
	auto divee = (*it)->OpA;
	auto diver = (*it)->OpB;
	auto t1 = (*it)->Result;
	auto t2 = (*(it + 1))->Result;
	auto t3 = (*(it + 2))->Result;

	auto quater1 = *(it + 1);
	if (quater1->type == Mult && quater1->OpA == t1 && quater1->OpB == diver) {
		auto quater2 = *(it + 2);
		if (quater2->type == Sub && quater2->OpA == divee && quater2->OpB == t2)
			return t3;
	}
	return NULL;
}


/* ---------------------------- MIPS ----------------------------*/
static char buf[MAXBUF];

/* general */
void reg_mips_move(vector<string>* list, string src, string tar) {
	sprintf(buf, "move %s %s", tar.c_str(), src.c_str());
	list->push_back(string(buf));
}

/* tag */
void reg_mips_string(vector<string>* list, string str) {
	list->push_back(str);
}

/* None Instr Mips Code */
void reg_mips_comment(vector<string>* list, string comment) {
	sprintf(buf, "# %s", comment.c_str());
	list->push_back(string(buf));
}
// label: .space size 
void reg_mips_space_alloc(vector<string>* list, string label, int size) {
	sprintf(buf, "%s: .space %d", label.c_str(), size);
	list->push_back(string(buf));
}
// label: .asciiz strcon
void reg_mips_asciiz(vector<string>* list, string label, string strcon) {
	sprintf(buf, "%s: .asciiz \"%s\"", label.c_str(), strcon.c_str());
	list->push_back(string(buf));
	list->push_back(string(".align 2"));
}
// label: 
void reg_mips_label(vector<string>* list, string label) {
	sprintf(buf, "%s:", label.c_str());
	list->push_back(string(buf));
}

/* Memory */
// addi $sp, $sp, size
void reg_mips_sp_move(vector<string>* list, int offset) {
	sprintf(buf, "addiu $sp $sp %d", offset);
	list->push_back(buf);
}
// lw target, offset(sp) | lw target, glb_data_label | la target, string_label | li target, const
void reg_mips_load(vector<string>* list, string target, SymbolItem* item, RegMipsFunction* func) {
	// 情况1：常数
	if (item->type == SymbolItemType::temp_const || item->type == SymbolItemType::_const) {
		sprintf(buf, "li %s %d", target.c_str(), item->value);
	}
	else {
		bool local;
		int offset;
		string label;
		func->get_addr(item, &local, &offset, &label);
		// 情况2：字符串
		if (item->type == SymbolItemType::temp_strcon)
			sprintf(buf, "la %s %s", target.c_str(), label.c_str());
		else {
			// 情况3：局部变量
			if (local)
				sprintf(buf, "lw %s %d($sp)", target.c_str(), offset);
			// 情况4：全局变量
			else {
				sprintf(buf, "lw %s %s", target.c_str(), label.c_str());
			}
		}
	}
	list->push_back(string(buf));
}
// lw target, offset(sp)
void reg_mips_load(vector<string>* list, string target, int offset) {
	sprintf(buf, "lw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}
// sw target, offset(sp) | sw target, label 
void reg_mips_save(vector<string>* list, string target, SymbolItem* item, RegMipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(item, &local, &offset, &label);
	if (local)
		sprintf(buf, "sw %s %d($sp)", target.c_str(), offset);
	else
		sprintf(buf, "sw %s %s", target.c_str(), label.c_str());
	list->push_back(string(buf));
}
// sw target, offset(sp)
void reg_mips_save(vector<string>* list, string target, int offset) {
	sprintf(buf, "sw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}

/* Branch */
// jr target
void reg_mips_jr(vector<string>* list, string label) {
	sprintf(buf, "jr %s", label.c_str());
	list->push_back(string(buf));
}
void reg_mips_jal(vector<string>* list, string label) {
	sprintf(buf, "jal %s", label.c_str());
	list->push_back(string(buf));
}
void reg_mips_j(vector<string>* list, string label) {
	sprintf(buf, "j %s", label.c_str());
	list->push_back(string(buf));
}
void reg_mips_bnz(vector<string>* list, string lable, string condition) {
	sprintf(buf, "bne %s $0 %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}
void reg_mips_bz(vector<string>* list, string lable, string condition) {
	sprintf(buf, "beq %s $0 %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}
void reg_mips_branch(vector<string>* list, string label, string cmpA, string cmpB, QuaterType cmp_type) {
	if (cmp_type == QuaterType::EqlCmp)
		sprintf(buf, "beq %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else if (cmp_type == QuaterType::GeqCmp)
		sprintf(buf, "bge %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else if (cmp_type == QuaterType::GtCmp)
		sprintf(buf, "bgt %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else if (cmp_type == QuaterType::LeqCmp)
		sprintf(buf, "ble %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else if (cmp_type == QuaterType::LtCmp)
		sprintf(buf, "blt %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else if (cmp_type == QuaterType::NeqCmp)
		sprintf(buf, "bne %s %s %s", cmpA.c_str(), cmpB.c_str(), label.c_str());
	else
		DEBUG_PRINT("[ERROR] Unexpected Compare Type in reg_mips_branch");
	list->push_back(string(buf));
}

void reg_mips_branch_zero(vector<string>* list, string label, string cmpA, QuaterType cmp_type) {
	if (cmp_type == QuaterType::EqlCmp)
		sprintf(buf, "beq %s $0 %s", cmpA.c_str(), label.c_str());
	else if (cmp_type == QuaterType::GeqCmp)
		sprintf(buf, "bgez %s %s", cmpA.c_str(), label.c_str());
	else if (cmp_type == QuaterType::GtCmp)
		sprintf(buf, "bgtz %s %s", cmpA.c_str(), label.c_str());
	else if (cmp_type == QuaterType::LeqCmp)
		sprintf(buf, "blez %s %s", cmpA.c_str(), label.c_str());
	else if (cmp_type == QuaterType::LtCmp)
		sprintf(buf, "bltz %s %s", cmpA.c_str(), label.c_str());
	else if (cmp_type == QuaterType::NeqCmp)
		sprintf(buf, "bne %s $0 %s", cmpA.c_str(), label.c_str());
	else
		DEBUG_PRINT("[ERROR] Unexpected Compare Type in reg_mips_branch_zero");
	list->push_back(string(buf));
}

/* Archimetic */
void reg_mips_calc(vector<string>* list, string A, string B, string Result, QuaterType type) {
	switch (type)
	{
	case Add:
		sprintf(buf, "addu %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Sub:
		sprintf(buf, "subu %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Mult:
		sprintf(buf, "mul %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Div:
		sprintf(buf, "div %s %s", A.c_str(), B.c_str());
		list->push_back(string(buf));
		sprintf(buf, "mflo %s", Result.c_str());
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType MIP_cal");
		break;
	}
	list->push_back(string(buf));
}

void reg_mips_cali(vector<string>* list, string A, int immediate, string Result, QuaterType type) {

	switch (type)
	{
	case Add:
		sprintf(buf, "addiu %s %s %d", Result.c_str(), A.c_str(), immediate);
		break;
	case Sub:
		sprintf(buf, "addiu %s %s %d", Result.c_str(), A.c_str(), -immediate);
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType in reg_mips_cali");
		break;
	}
	list->push_back(string(buf));
}

void reg_mips_cmp(vector<string>* list, string A, string B, string Result, QuaterType type) {
	switch (type)
	{
	case EqlCmp:
		sprintf(buf, "seq %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case NeqCmp:
		sprintf(buf, "sne %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case GtCmp:
		sprintf(buf, "sgt %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case GeqCmp:
		sprintf(buf, "sge %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case LtCmp:
		sprintf(buf, "slt %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case LeqCmp:
		sprintf(buf, "sle %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType in reg_mips_cmp");
		break;
	}
	list->push_back(string(buf));
}

void reg_mips_li(vector<string>* list, int value, string Result) {
	sprintf(buf, "li %s %d", Result.c_str(), value);
	list->push_back(string(buf));
}

// 取模运算 - 目标代码优化，无中间码类型对应
void regs_mips_mod_special(vector<string>* list, string A, string B, string Result) {
	sprintf(buf, "div %s %s", A.c_str(), B.c_str());
	list->push_back(string(buf));
	sprintf(buf, "mfhi %s", Result.c_str());
	list->push_back(string(buf));
}


/* IO */
// scan : int|char
void reg_mips_scan(vector<string>* list, string target, BasicType type) {
	if (type == BasicType::_int)
		sprintf(buf, "li $v0 5");
	else if (type == BasicType::_char)
		sprintf(buf, "li $v0 12");
	else {
		DEBUG_PRINT("[ERROR]: Unknown Type in reg_mips_scan");
	}
	list->push_back(string(buf));
	list->push_back("syscall");
	sprintf(buf, "move %s $v0", target.c_str());
	list->push_back(string(buf));
}
// print : int|char|string
void reg_mips_print(vector<string>* list, string source, BasicType type) {
	if (type == BasicType::_int)
		sprintf(buf, "li $v0 1");
	else if (type == BasicType::_char)
		sprintf(buf, "li $v0 11");
	else if (type == BasicType::_string)
		sprintf(buf, "li $v0 4");
	list->push_back(string(buf));
	if (source != "$a0") {
		sprintf(buf, "move $a0, %s", source.c_str());
		list->push_back(string(buf));
	}
	list->push_back("syscall");
}

/* Array */
void reg_mips_array_assign(vector<string>* list, string source, SymbolItem* arr, string index, string temp,RegMipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(arr, &local, &offset, &label);
	if (local) {
		sprintf(buf, "sll %s %s 2", temp.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "addu %s %s $sp", temp.c_str(), temp.c_str());
		list->push_back(string(buf));
		sprintf(buf, "sw %s %d(%s)", source.c_str(), offset, temp.c_str());
		list->push_back(string(buf));
	}
	else {
		sprintf(buf, "sll %s %s 2", temp.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "sw %s %s(%s)", source.c_str(), label.c_str(), temp.c_str());
		list->push_back(string(buf));
	}
}
void reg_mips_array_query(vector<string>* list, string target, SymbolItem* arr, string index, string temp, RegMipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(arr, &local, &offset, &label);
	if (local) {
		sprintf(buf, "sll %s %s 2", temp.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "addu %s %s $sp", temp.c_str(), temp.c_str());
		list->push_back(string(buf));
		sprintf(buf, "lw %s %d(%s)", target.c_str(), offset, temp.c_str());
		list->push_back(string(buf));
	}
	else {
		sprintf(buf, "sll %s %s 2", temp.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "lw %s %s(%s)", target.c_str(), label.c_str(), temp.c_str());
		list->push_back(string(buf));
	}
}