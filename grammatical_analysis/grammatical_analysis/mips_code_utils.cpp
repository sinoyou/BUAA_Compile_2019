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
	sprintf_s(buf, "# %s", comment.c_str());
	list->push_back(string(buf));
}
// label: .space size 
void mips_space_alloc(vector<string>* list, string label, int size) {
	sprintf_s(buf, "%s: .space %d", label.c_str(),size);
	list->push_back(string(buf));
}
// label: .asciiz strcon
void mips_asciiz(vector<string>* list, string label, string strcon) {
	sprintf_s(buf, "%s: .asciiz \"%s\"", label.c_str() , strcon.c_str());
	list->push_back(string(buf));
	list->push_back(string(".align 2"));
}
// label: 
void mips_label(vector<string>* list, string label) {
	sprintf_s(buf, "%s:", label.c_str());
	list->push_back(string(buf));
}

/* Memory */
// addi $sp, $sp, size
void mips_sp_move(vector<string>* list, int offset) {
	sprintf_s(buf, "addiu $sp $sp %d", offset);
	list->push_back(buf);
}
// lw target, offset(sp) | lw target, glb_data_label | la target, string_label | li target, const
void mips_load(vector<string>* list, string target, SymbolItem* item, MipsFunction* func) {
	// 情况1：常数
	if (item->type == SymbolItemType::temp_const || item->type == SymbolItemType::_const) {
		sprintf_s(buf, "li %s %d", target.c_str(), item->value);
	}
	else {
		bool local;
		int offset;
		string label;
		func->get_addr(item, &local, &offset, &label);
		// 情况2：字符串
		if (item->type == SymbolItemType::temp_strcon)
			sprintf_s(buf, "la %s %s", target.c_str(),label.c_str());
		else {
			// 情况3：局部变量
			if (local)
				sprintf_s(buf, "lw %s %d($sp)", target.c_str(), offset);
			// 情况4：全局变量
			else {
				sprintf_s(buf, "lw %s %s", target.c_str(), label.c_str());
			}
		}
	}
	list->push_back(string(buf));
}
// lw target, offset(sp)
void mips_load(vector<string>* list, string target, int offset) {
	sprintf_s(buf, "lw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}
// sw target, offset(sp) | sw target, label 
void mips_save(vector<string>* list, string target, SymbolItem* item, MipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(item, &local, &offset, &label);
	if (local)
		sprintf_s(buf, "sw %s %d($sp)", target.c_str(), offset);
	else
		sprintf_s(buf, "sw %s %s", target.c_str(), label.c_str());
	list->push_back(string(buf));
}
// sw target, offset(sp)
void mips_save(vector<string>* list, string target, int offset) {
	sprintf_s(buf, "sw %s %d($sp)", target.c_str(), offset);
	list->push_back(string(buf));
}

/* Branch */
// jr target
void mips_jr(vector<string>* list, string label) {
	sprintf_s(buf, "jr %s", label.c_str());
	list->push_back(string(buf));
}
void mips_jal(vector<string>* list, string label) {
	sprintf_s(buf, "jal %s", label.c_str());
	list->push_back(string(buf));
}
void mips_j(vector<string>* list, string label) {
	sprintf_s(buf, "j %s", label.c_str());
	list->push_back(string(buf));
}
void mips_bnz(vector<string>* list, string lable, string condition) {
	sprintf_s(buf, "bnez %s %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}
void mips_bz(vector<string>* list, string lable, string condition) {
	sprintf_s(buf, "bez %s %s", condition.c_str(), lable.c_str());
	list->push_back(string(buf));
}

/* Archimetic */
void mips_calc(vector<string>* list, string A, string B, string Result, QuaterType type) {
	switch (type)
	{
	case Add:
		sprintf_s(buf, "addu %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Sub:
		sprintf_s(buf, "subu %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Mult:
		sprintf_s(buf, "mulo %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case Div:
		sprintf_s(buf, "div %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType MIP_cal");
		break;
	}
	list->push_back(string(buf));
}
void mips_cmp(vector<string>* list, string A, string B, string Result, QuaterType type) {
	switch (type)
	{
	case EqlCmp:
		sprintf_s(buf, "seq %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case NeqCmp:
		sprintf_s(buf, "sne %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case GtCmp:
		sprintf_s(buf, "sgt %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case GeqCmp:
		sprintf_s(buf, "sge %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case LtCmp:
		sprintf_s(buf, "slt %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	case LeqCmp:
		sprintf_s(buf, "sle %s %s %s", Result.c_str(), A.c_str(), B.c_str());
		break;
	default:
		DEBUG_PRINT("[ERROR] Unexpected QuaterType in MIPS_cmp");
		break;
	}
}

/* IO */
// scan : int|char
void mips_scan(vector<string>* list, string target, BasicType type) {
	if (type == BasicType::_int)
		sprintf_s(buf, "li $v0 5");
	else if (type == BasicType::_char)
		sprintf_s(buf, "li $v0 12");
	else {
		DEBUG_PRINT("[ERROR]: Unknown Type in mips_scan");
	}
	list->push_back(string(buf));
	list->push_back("syscall");
	sprintf_s(buf, "move %s $v0", target.c_str());
	list->push_back(string(buf));
}
// print : int|char|string
void mips_print(vector<string>* list, string source, BasicType type) {
	if (type == BasicType::_int)
		sprintf_s(buf, "li $v0 1");
	else if (type == BasicType::_char)
		sprintf_s(buf, "li $v0 11");
	else if (type == BasicType::_string)
		sprintf_s(buf, "li $v0 4");
	list->push_back(string(buf));
	sprintf_s(buf, "move $a0, %s", source.c_str());
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
		sprintf_s(buf, "sll %s %s 2", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "addiu %s %s %d", index.c_str(), index.c_str(), offset);
		list->push_back(string(buf));
		sprintf_s(buf, "addu %s %s $sp", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "sw %s 0(%s)", source.c_str(), index.c_str());
	}
	else {
		sprintf_s(buf, "sll %s %s 2", index.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "sw %s %s(%s)", source.c_str(), label.c_str(), index.c_str());
		list->push_back(string(buf));
	}
}
void mips_array_query(vector<string>* list, string target, SymbolItem* arr, string index, MipsFunction* func) {
	bool local;
	int offset;
	string label;
	func->get_addr(arr, &local, &offset, &label);
	if (local) {
		sprintf_s(buf, "sll %s %s 2", target.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "addiu %s %s %d", target.c_str(), target.c_str(), offset);
		list->push_back(string(buf));
		sprintf_s(buf, "addu %s %s $sp", target.c_str(), target.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "lw %s 0(%s)", target.c_str(), target.c_str());
	}
	else {
		sprintf_s(buf, "sll %s %s 2", target.c_str(), index.c_str());
		list->push_back(string(buf));
		sprintf_s(buf, "lw %s %s(%s)", target.c_str(), label.c_str(), target.c_str());
		list->push_back(string(buf));
	}
}

/* --------------------------- 非代码类 -------------------------- */
/* 给定四元式，检查其Items的属于情况 */
// todo
void quater_block_check(Quaternary* q) {
	SymbolItem* A = q->OpA;
	SymbolItem* B = q->OpB;
	SymbolItem* R = q->Result;
	set<Block*> temp;
	temp.insert(q->block);
	if (A != NULL) temp.insert(A->block);
	if (B != NULL) temp.insert(B->block);
	if (R != NULL) temp.insert(R->block);
	if (temp.size() == 0) {
		if (q->type != QuaterType::FuncRet) {
			char buf[200];
			sprintf_s(buf, "[WARNING] Empty SymbolItem in Quaternary %d", q->type);
			DEBUG_PRINT(buf);
		}
	}
}

/* 根据函数获取该函数下的所有四元式 */
vector<Quaternary*> get_quaternary_by_function(SymbolItem* func, vector<Quaternary*>* list) {
	vector<Quaternary*> save;
	for (auto it = list->begin(); it != list->end(); it++) {
		if (func->block == (*it)->block)
			save.push_back(*it);
	}
	return save;
}

/* 给定四元式，返回其所有的SymbolItem */
map<string, SymbolItem*> get_items_by_quater(Quaternary* q) {
	map <string, SymbolItem*> map;
	if (q->OpA != NULL)
		map["A"] = q->OpA;
	if (q->OpB != NULL)
		map["B"] = q->OpB;
	if (q->Result != NULL)
		map["Result"] = q->Result;
	return map;
}