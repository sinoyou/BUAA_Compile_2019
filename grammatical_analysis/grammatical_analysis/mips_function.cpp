﻿#include "mips_function.h"
#include <math.h>
#include "debug.h"
#include "Quaternary.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <list>

/* Stack Distribution */
/*
		TOP
|###################|
|					|	first_para
|		para		|
|					|	last_para
|-------------------|
|		ret			|
|-------------------|
|		$ra			|
|-------------------|
|					|
|					|
|                   |
|     Variables		|
|                   |
|					|
|					|
|					|
|###################|  <- $sp
	   BUTTOM
*/

extern ofstream log_file;

MipsFunction::MipsFunction(
	SymbolItem* func_head, vector<Quaternary*> func_quater, map<SymbolItem*, string>* global_data) :
	func_head(func_head), quater_list(func_quater), global_data(global_data) {
	int offset = 0;
	// varibales (including array)
	for (auto it = func_quater.begin(); it != func_quater.end(); it++) {
		auto list = get_items_by_quater(*it);
		for (auto itt = list.begin(); itt != list.end(); itt++) {
			int size = 4;			// size by default
			SymbolItem* t;
			// fix bug : 只考虑函数内的局部变量，全局变量不考虑.
			if ((*itt).second->block != this->func_head->block)
				continue;
			// fix bug : 只考虑函数内变量，函数参数不考虑
			if (std::find(this->func_head->paramsList.begin(), this->func_head->paramsList.end(), (*itt).second)
				!= this->func_head->paramsList.end())
				continue;

			// 查找到函数内局部变量，计算大小.
			if ((*itt).second->type == SymbolItemType::temp_normal) t = (*itt).second;
			else if ((*itt).second->type == SymbolItemType::_variable) {
				t = (*itt).second;
				if (t->isArray)
					size = t->array_size * 4;
			}
			else
				continue;

			// judge if space already alloc ?
			if (offset_map.find(t) == offset_map.end()) {
				char buf[100];
				sprintf(buf, "[Function Alloc] %s -> %d, size = %d\n", t->getname().c_str(), offset, size);
				log_file << string(buf);
				offset_map[t] = offset;
				offset += size;
			}
		}
	}

	// ra
	ra_offset = offset;
	offset += 4;

	// ret
	ret_offset = offset;
	offset += 4;

	// parameter - must be mapped to stack in a reversed order. (because para push order when call)
	list<Quaternary*> para_declar_quater_list;
	for (auto it = func_quater.begin(); it != func_quater.end(); it++) {
		if ((*it)->type == QuaterType::FuncParaDeclar)
			para_declar_quater_list.push_front((*it));
	}
	for (auto it = para_declar_quater_list.begin(); it != para_declar_quater_list.end(); it++) {
		SymbolItem* t = (*it)->Result;
		char buf[100];
		sprintf(buf, "[Function Alloc] %s -> %d, size = %d\n", t->getname().c_str(), offset, 4);
		log_file << string(buf);
		offset_map[t] = offset;
		offset += 4;
	}
};

/*
 * 计算并返回该函数所需要的栈空间大小
*/
int MipsFunction::stack_size() {
	int size = (ret_offset > ra_offset) ? ret_offset : ra_offset;
	for (auto it = offset_map.begin(); it != offset_map.end(); it++) {
		if (abs((*it).second) > size)
			size = abs((*it).second);
	}
	return size + 4;
}

/*
 * 导出此函数部分的中间码
*/
vector<string> MipsFunction::dump() {
	// 需要考虑的指令:
	// 函数定义：FuncDeclar, FuncRet
	// 函数调用：FuncCall, FuncParaPush, AssignRet
	// 算术运算：Add, Sub, Mult, Div
	// 赋值：Assign, AssignArray
	// 比较：EQL, NEQ, LT, LEQ, GT, GEL
	// 分支跳转：GOTO, BN, BNZ
	// 标签设置：SetLabel
	// 数组访问：ArrayQuery
	// IO：Print, Scan
	vector<string> codes;

	// simple dump情况下，仅使用$s0, $s1, $s2
	string t1 = "$s0";
	string t2 = "$s1";
	string t3 = "$s2";
	string ra = "$ra";
	string zero = "$0";

	// 函数
	int func_para_cnt = 0;				// 参数推入的计数器，当推入一个参数后+1，当调用了函数后置0.
	SymbolItem* called_func = NULL;		// 保存最近被call的函数

	// 函数栈空间总大小
	int func_stack_size = stack_size();

	// 增加函数开始注释
	mips_string(&codes, "");
	mips_comment(&codes, "Function Begins Here " + func_head->name);

	for (auto quater = quater_list.begin(); quater != quater_list.end(); quater++) {
		QuaterType type = (*quater)->type;
		Quaternary* q = *quater;
		Quaternary* peek = (quater + 1 == quater_list.end()) ? NULL : *(quater + 1);
		mips_comment(&codes, PrintQuaterHandler(q, type));
		if (type == FuncDeclar) {
			mips_label(&codes, q->Result->name);
			mips_sp_move(&codes, -func_stack_size);
			mips_save(&codes, ra, ra_offset);
		}
		else if (type == FuncRet) {
			if (q->OpA != NULL)
			{
				mips_load(&codes, t1, q->OpA, &(*this));
				mips_save(&codes, t1, ret_offset);
			}
			mips_load(&codes, t1, ra_offset);
			mips_sp_move(&codes, func_stack_size);		// $sp + stack_size
			mips_jr(&codes, t1);
		}
		else if (type == FuncParaPush) {
			func_para_cnt++;
			mips_load(&codes, t1, q->OpA, &(*this));
			mips_save(&codes, t1, -func_para_cnt * 4);
		}
		else if (type == FuncCall) {
			func_para_cnt = 0;
			mips_jal(&codes, q->OpA->name);
			called_func = q->OpA;
		}
		else if (type == AssignRet) {
			mips_load(&codes, t1, -1 * (called_func->paramsList.size() + 1) * 4);
			mips_save(&codes, t1, q->Result, &(*this));
		}
		else if (type == AssignArray) {
			mips_load(&codes, t1, q->OpA, &(*this));				// source
			mips_load(&codes, t2, q->OpB, &(*this));				// index
			mips_array_assign(&codes, t1, q->Result, t2, &(*this));
		}
		else if (type == Add) {
			if (q->OpA->is_const() || q->OpB->is_const()) {
				int value = 0;
				if (q->OpA->is_const()) {
					value = q->OpA->value;
					mips_load(&codes, t1, q->OpB, &(*this));
				}
				else {
					value = q->OpB->value;
					mips_load(&codes, t1, q->OpA, &(*this));
				}
				mips_cali(&codes, t1, value, t3, type);
			}
			else {
				mips_load(&codes, t1, q->OpA, &(*this));
				mips_load(&codes, t2, q->OpB, &(*this));
				mips_calc(&codes, t1, t2, t3, type);
			}
			mips_save(&codes, t3, q->Result, &(*this));
		}
		else if (type == Sub && q->OpA == NULL) {
			if (!q->OpB->is_const()) {
				mips_load(&codes, t2, q->OpB, &(*this));
				mips_calc(&codes, zero, t2, t3, type);
			}
			else {
				mips_cali(&codes, zero, q->OpB->value, t3, type);
			}
			mips_save(&codes, t3, q->Result, &(*this));
		}
		else if (type == Sub && q->OpA != NULL) {
			mips_load(&codes, t1, q->OpA, &(*this));
			if (!q->OpB->is_const()) {
				mips_load(&codes, t2, q->OpB, &(*this));
				mips_calc(&codes, t1, t2, t3, type);
			}
			else {
				mips_cali(&codes, t1, q->OpB->value, t3, type);
			}
			mips_save(&codes, t3, q->Result, &(*this));
		}
		else if (type == Mult || type == Div) {
			mips_load(&codes, t1, q->OpA, &(*this));
			mips_load(&codes, t2, q->OpB, &(*this));
			mips_calc(&codes, t1, t2, t3, type);
			mips_save(&codes, t3, q->Result, &(*this));
		}
		else if (type == Assign) {
			mips_load(&codes, t1, q->OpA, &(*this));
			mips_save(&codes, t1, q->Result, &(*this));
		}
		else if (type == EqlCmp || type == NeqCmp || type == GtCmp || type == GeqCmp || type == LtCmp || type == LeqCmp) {
			mips_load(&codes, t1, q->OpA, &(*this));
			mips_load(&codes, t2, q->OpB, &(*this));
			// 该CMP结果仅用于跳转操作，则可以合并，进行跳转优化。
			// branch if compare result is true.
			if (peek && peek->type == QuaterType::Bnz
				&& peek->OpB == q->Result && q->Result->type == SymbolItemType::temp_normal) {
				QuaterType cmp_type = q->type;								// cmp+bnz，cmp类型保持原样即可
				mips_branch(&codes, peek->OpA->name, t1, t2, cmp_type);
				quater += 1;
			}
			// branch if compare result if false.
			else if (peek && peek->type == QuaterType::Bz
				&& peek->OpB == q->Result && q->Result->type == SymbolItemType::temp_normal) {
				QuaterType cmp_type = cmp_reverse(type);			// cmp+bz，cmp的类型需要取反
				mips_branch(&codes, peek->OpA->name, t1, t2, cmp_type);
				quater += 1;
			}
			// compare is not for a branch
			else {
				mips_cmp(&codes, t1, t2, t3, type);
				mips_save(&codes, t3, q->Result, &(*this));
			}
		}
		else if (type == Goto) {
			mips_j(&codes, q->OpA->name);
		}
		else if (type == Bz) {
			mips_load(&codes, t1, q->OpB, &(*this));
			mips_bz(&codes, q->OpA->name, t1);
		}
		else if (type == Bnz) {
			mips_load(&codes, t1, q->OpB, &(*this));
			mips_bnz(&codes, q->OpA->name, t1);
		}
		else if (type == SetLabel) {
			mips_label(&codes, q->OpA->name);
		}
		else if (type == ArrayQuery) {
			mips_load(&codes, t2, q->OpB, &(*this));
			mips_array_query(&codes, t3, q->OpA, t2, &(*this));
			mips_save(&codes, t3, q->Result, &(*this));
		}
		else if (type == Scan) {
			mips_scan(&codes, t1, q->Result->basic_type);
			mips_save(&codes, t1, q->Result, &(*this));
		}
		else if (type == Print) {
			mips_load(&codes, t1, q->OpA, &(*this));
			mips_print(&codes, t1, q->OpA->basic_type);
		}
		else {
			// DEBUG_PRINT("[ERROR] Unknown Quater Type.");
		}
	}

	// 增加函数结束注释
	mips_comment(&codes, "Function Ends Here " + func_head->name);
	return codes;
}

void MipsFunction::get_addr(SymbolItem* item, bool* local, int* offset, string* data_label) {
	if (this->offset_map.find(item) != this->offset_map.end()) {
		*local = true;
		*offset = this->offset_map[item];
	}
	else if (this->global_data->find(item) != this->global_data->end()) {
		*local = false;
		*data_label = (*this->global_data->find(item)).second;
	}
	else {
		char buf[100];
		sprintf(buf, "[ERROR] No found %s in %s", item->name.c_str(), this->func_head->name.c_str());
		DEBUG_PRINT(buf);
	}
}