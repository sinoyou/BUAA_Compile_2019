#include "mips_function.h"
#include "mips_code_utils.h"
#include "debug.h"
#include <set>
#include <iostream>
#include <stdio.h>

#define MAXBUF 200

/* ---------------------------- MIPS ----------------------------*/
static char buf[MAXBUF];

/* tag */
void mips_string(vector<string>* list, string str) {
	list->push_back(str);
}

/* None Instr Mips Code */
void mips_comment(vector<string>* list, string comment) {
	sprintf(buf, "# %s", comment.c_str());
	list->push_back(string(buf));
}
// label: .space size 
void mips_space_alloc(vector<string>* list, string label, int size) {
	sprintf(buf, "%s: .space %d", label.c_str(), size);
	list->push_back(string(buf));
}
// label: .asciiz strcon
void mips_asciiz(vector<string>* list, string label, string strcon) {
	sprintf(buf, "%s: .asciiz \"%s\"", label.c_str(), strcon.c_str());
	list->push_back(string(buf));
	list->push_back(string(".align 2"));
}
// label: 
void mips_label(vector<string>* list, string label) {
	sprintf(buf, "%s:", label.c_str());
	list->push_back(string(buf));
}

/* Memory */
// addi $sp, $sp, size
void mips_sp_move(vector<string>* list, int offset) {
	sprintf(buf, "addiu $sp $sp %d", offset);
	list->push_back(buf);
}
// lw target, offset(sp) | lw target, glb_data_label | la target, string_label | li target, const
void mips_load(vector<string>* list, string target, SymbolItem* item, MipsFunction* func) {
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
void mips_load(vector<string>* list, string target, int offset) {
	sprintf(buf, "lw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}
// sw target, offset(sp) | sw target, label 
void mips_save(vector<string>* list, string target, SymbolItem* item, MipsFunction* func) {
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
void mips_save(vector<string>* list, string target, int offset) {
	sprintf(buf, "sw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}

/* Branch */
// jr target
void mips_jr(vector<string>* list, string label) {
	sprintf(buf, "jr %s", label.c_str());
	list->push_back(string(buf));
}
void mips_jal(vector<string>* list, string label) {
	sprintf(buf, "jal %s", label.c_str());
	list->push_back(string(buf));
}
void mips_j(vector<string>* list, string label) {
	sprintf(buf, "j %s", label.c_str());
	list->push_back(string(buf));
}
void mips_bnz(vector<string>* list, string lable, string condition) {
	sprintf(buf, "bne %s $0 %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}
void mips_bz(vector<string>* list, string lable, string condition) {
	sprintf(buf, "beq %s $0 %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}
void mips_branch(vector<string>* list, string label, string cmpA, string cmpB, QuaterType cmp_type) {
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
		DEBUG_PRINT("[ERROR] Unexpected Compare Type in mips_branch");
	list->push_back(string(buf));
}

/* Archimetic */
void mips_calc(vector<string>* list, string A, string B, string Result, QuaterType type) {
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

void mips_cali(vector<string>* list, string A, int immediate, string Result, QuaterType type) {

	switch (type)
	{
	case Add:
		sprintf(buf, "addiu %s %s %d", Result.c_str(), A.c_str(), immediate);
		break;
	case Sub:
		sprintf(buf, "subiu %s %s %d", Result.c_str(), A.c_str(), immediate);
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType in MIPS_cali");
		break;
	}
	list->push_back(string(buf));
}

void mips_cmp(vector<string>* list, string A, string B, string Result, QuaterType type) {
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
		DEBUG_PRINT("[ERROR] Unexpected QuaterType in MIPS_cmp");
		break;
	}
	list->push_back(string(buf));
}

/* IO */
// scan : int|char
void mips_scan(vector<string>* list, string target, BasicType type) {
	if (type == BasicType::_int)
		sprintf(buf, "li $v0 5");
	else if (type == BasicType::_char)
		sprintf(buf, "li $v0 12");
	else {
		DEBUG_PRINT("[ERROR]: Unknown Type in mips_scan");
	}
	list->push_back(string(buf));
	list->push_back("syscall");
	sprintf(buf, "move %s $v0", target.c_str());
	list->push_back(string(buf));
}
// print : int|char|string
void mips_print(vector<string>* list, string source, BasicType type) {
	if (type == BasicType::_int)
		sprintf(buf, "li $v0 1");
	else if (type == BasicType::_char)
		sprintf(buf, "li $v0 11");
	else if (type == BasicType::_string)
		sprintf(buf, "li $v0 4");
	list->push_back(string(buf));
	sprintf(buf, "move $a0, %s", source.c_str());
	list->push_back(string(buf));
	list->push_back("syscall");
}

/* Array */
void mips_array_assign(vector<string>* list, string source, SymbolItem* arr, string index, MipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(arr, &local, &offset, &label);
	if (local) {
		sprintf(buf, "sll %s %s 2", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "addiu %s %s %d", index.c_str(), index.c_str(), offset);
		list->push_back(string(buf));
		sprintf(buf, "addu %s %s $sp", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "sw %s 0(%s)", source.c_str(), index.c_str());
		list->push_back(string(buf));
	}
	else {
		sprintf(buf, "sll %s %s 2", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "sw %s %s(%s)", source.c_str(), label.c_str(), index.c_str());
		list->push_back(string(buf));
	}
}
void mips_array_query(vector<string>* list, string target, SymbolItem* arr, string index, MipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(arr, &local, &offset, &label);
	if (local) {
		sprintf(buf, "sll %s %s 2", target.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "addiu %s %s %d", target.c_str(), target.c_str(), offset);
		list->push_back(string(buf));
		sprintf(buf, "addu %s %s $sp", target.c_str(), target.c_str());
		list->push_back(string(buf));
		sprintf(buf, "lw %s 0(%s)", target.c_str(), target.c_str());
		list->push_back(string(buf));
	}
	else {
		sprintf(buf, "sll %s %s 2", target.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf(buf, "lw %s %s(%s)", target.c_str(), label.c_str(), target.c_str());
		list->push_back(string(buf));
	}
}