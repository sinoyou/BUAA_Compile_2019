#include "register_mips_function.h"
#include <math.h>
#include "debug.h"
#include "Quaternary.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <list>
#include "config.h"

/*
 * 寄存器分配踩过的坑：
 * 1. 除了两个隐性的ra和ret，其他任何值都不要用reg_mips_load来操作，因为内存中的值不一定新。(特别是参数推入和返回值)
 * 2. 
*/

/* Stack Distribution */
/*
		TOP
|###################|
|					|	first_para
|		para		|
|					|	last_para
|-------------------|
|		ret			|	(作废，未使用)
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

RegMipsFunction::RegMipsFunction(
	SymbolItem* func_head, 
	vector<Quaternary*> func_quater,
	map<SymbolItem*, string>* global_data, 
	map<Quaternary*, BasicBlock*> quater_basicblock) :
	func_head(func_head), quater_list(func_quater), global_data(global_data), quater_basicblock(quater_basicblock) {
	/* 寄存器池定义 */
	register_pool = new RegisterPool(this, quater_list, quater_basicblock, &sub_object_code);
	/* 内存偏移计算 */
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
				sprintf(buf, "[Function Alloc] %s -> offset = %d, size = %d\n", t->getname().c_str(), offset, size);
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
		sprintf(buf, "[Function Alloc] %s -> offset = %d, size = %d\n", t->getname().c_str(), offset, 4);
		log_file << string(buf);
		offset_map[t] = offset;
		offset += 4;
	}
};

/*
 * 计算并返回该函数所需要的栈空间大小
*/
int RegMipsFunction::stack_size() {
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
vector<string> RegMipsFunction::dump() {
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
	vector<string>* codes = &sub_object_code;

	string ra = register_pool->ra;
	string a0 = register_pool->a0;
	string zero = register_pool->zero;
	string ret = register_pool->ret;
	string array_temp = register_pool->array_temp;

	// 函数
	int func_para_cnt = 0;				// 参数推入的计数器，当推入一个参数后+1，当调用了函数后置0.
	SymbolItem* called_func = NULL;		// 保存最近被call的函数

	// 基本块注释
	int current_block = -1;

	// 函数栈空间总大小
	int func_stack_size = stack_size();

	// 增加函数开始注释
	reg_mips_string(codes, "");
	reg_mips_comment(codes, "Function Begins Here " + func_head->name);

	for (auto quater = quater_list.begin(); quater != quater_list.end(); quater++) {
		QuaterType type = (*quater)->type;
		Quaternary* q = *quater;
		Quaternary* peek = (quater + 1 == quater_list.end()) ? NULL : *(quater + 1);
		// 块id输出
		if (current_block != quater_basicblock[q]->id) {
			current_block = quater_basicblock[q]->id;
			reg_mips_comment(codes, ">>>> Block No. " + to_string(current_block));
		}
		// 中间码输出
		reg_mips_comment(codes, PrintQuaterHandler(q, type));
		// 分类执行
		if (type == FuncDeclar) {
			reg_mips_label(codes, q->Result->name);
			reg_mips_sp_move(codes, -func_stack_size);
			reg_mips_save(codes, ra, ra_offset);
			register_pool->global_load();
		}
		else if (type == FuncRet) {
			if (q->OpA != NULL)
			{
				auto map = register_pool->request(q->OpA, NULL, NULL);
				reg_mips_move(codes, map[q->OpA], ret);
			}
			register_pool->clear_all_and_dump_temp_active(quater_basicblock[*quater]->active_out);
			reg_mips_load(codes, ra, ra_offset);
			// !!!
			reg_mips_sp_move(codes, func_stack_size);		// $sp + stack_size
			reg_mips_jr(codes, ra);
		}
		else if (type == FuncParaPush) {
			func_para_cnt++;
			auto map = register_pool->request(q->OpA, NULL, NULL);
			reg_mips_save(codes, map[q->OpA], -func_para_cnt * 4);
		}
		else if (type == FuncCall) {
			func_para_cnt = 0;
			// !!!
			register_pool->clear_all_and_dump_all_active(quater_basicblock[*quater]->active_out);
			reg_mips_jal(codes, q->OpA->name);
			called_func = q->OpA;
			register_pool->global_load();
		}
		else if (type == AssignRet) {
			auto map = register_pool->request(NULL, NULL, q->Result);
			reg_mips_move(codes, ret, map[q->Result]);
		}
		else if (type == AssignArray) {			
			auto map = register_pool->request(q->OpA, q->OpB, NULL);
			// opa = source, opb = index
			reg_mips_array_assign(codes, map[q->OpA], q->Result, map[q->OpB], array_temp, &(*this));
		}
		else if (type == Add) {					
			map<SymbolItem*, string> map;
			string t1;
			if (q->OpA->is_const() || q->OpB->is_const()) {
				int value = 0;
				if (q->OpA->is_const()) {
					value = q->OpA->value;
					map = register_pool->request(q->OpB, NULL, q->Result);
					t1 = map[q->OpB];
				}
				else {
					value = q->OpB->value;
					map = register_pool->request(q->OpA, NULL, q->Result);
					t1 = map[q->OpA];
				}
				reg_mips_cali(codes, t1, value, map[q->Result], type);
			}
			else {
				map = register_pool->request(q->OpA, q->OpB, q->Result);
				reg_mips_calc(codes, map[q->OpA], map[q->OpB], map[q->Result], type);
			}
		}
		else if (type == Sub && q->OpA == NULL) {			
			if (!q->OpB->is_const()) {
				auto map = register_pool->request(NULL, q->OpB, q->Result);
				reg_mips_calc(codes, zero, map[q->OpB], map[q->Result], type);
			}
			else {
				auto map = register_pool->request(NULL, NULL, q->Result);
				reg_mips_cali(codes, zero, q->OpB->value, map[q->Result], type);
			}
		}
		else if (type == Sub && q->OpA != NULL) {		
			if (!q->OpB->is_const()) {
				auto map = register_pool->request(q->OpA, q->OpB, q->Result);
				reg_mips_calc(codes, map[q->OpA], map[q->OpB], map[q->Result], type);
			}
			else {
				auto map = register_pool->request(q->OpA, NULL, q->Result);
				reg_mips_cali(codes, map[q->OpA], q->OpB->value, map[q->Result], type);
			}
		}
		else if (type == Mult || type == Div) {
			// MOD操作优化
			if (MOD_OPTIMIZE == 1) {
				auto mod = mod_judge(quater, quater_list.end());
				if (type == Div && mod != NULL) {
					auto map = register_pool->request(q->OpA, q->OpB, mod);
					regs_mips_mod_special(codes, map[q->OpA], map[q->OpB], map[mod]);
					quater = quater + 2;
				}
				else {
					auto map = register_pool->request(q->OpA, q->OpB, q->Result);
					reg_mips_calc(codes, map[q->OpA], map[q->OpB], map[q->Result], type);
				}
			}
			else {
				auto map = register_pool->request(q->OpA, q->OpB, q->Result);
				reg_mips_calc(codes, map[q->OpA], map[q->OpB], map[q->Result], type);
			}
		}
		else if (type == Assign) {
			if (q->OpA->is_const()) {
				auto map = register_pool->request(NULL, NULL, q->Result);
				reg_mips_li(codes, q->OpA->value, map[q->Result]);
			}
			else {
				auto map = register_pool->request(q->OpA, NULL, q->Result);
				reg_mips_move(codes, map[q->OpA], map[q->Result]);
			}
		}
		else if (type == EqlCmp || type == NeqCmp || type == GtCmp || type == GeqCmp || type == LtCmp || type == LeqCmp) {
			// 该CMP结果仅用于跳转操作，则可以合并，进行跳转优化。
			// branch if compare result is true.
			if (peek && peek->type == QuaterType::Bnz
				&& peek->OpB == q->Result && q->Result->type == SymbolItemType::temp_normal) {
				reg_mips_comment(codes, "Together with Bnz");
				if (q->OpB->is_const() && q->OpB->value == 0 && BRANCH_ZERO_OPTIMIZE == 1) {
					auto map = register_pool->request(q->OpA, NULL, NULL);
					QuaterType cmp_type = q->type;						// cmp+bnz，cmp类型保持原样即可
					// !!!
					register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
					reg_mips_branch_zero(codes, peek->OpA->name, map[q->OpA], cmp_type);
					quater += 1;
				}
				else {
					auto map = register_pool->request(q->OpA, q->OpB, NULL);
					QuaterType cmp_type = q->type;						// cmp+bnz，cmp类型保持原样即可
					// !!!
					register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
					reg_mips_branch(codes, peek->OpA->name, map[q->OpA], map[q->OpB], cmp_type);
					quater += 1;
				}
			}
			// branch if compare result if false.
			else if (peek && peek->type == QuaterType::Bz
				&& peek->OpB == q->Result && q->Result->type == SymbolItemType::temp_normal) {
				reg_mips_comment(codes, "Together with Bz");
				if (q->OpB->is_const() && q->OpB->value == 0 && BRANCH_ZERO_OPTIMIZE == 1) {
					auto map = register_pool->request(q->OpA, NULL, NULL);
					QuaterType cmp_type = cmp_reverse(type);			// cmp+bz，cmp的类型需要取反
					// !!!
					register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
					reg_mips_branch_zero(codes, peek->OpA->name, map[q->OpA], cmp_type);
					quater += 1;
				}
				else {
					auto map = register_pool->request(q->OpA, q->OpB, NULL);
					QuaterType cmp_type = cmp_reverse(type);			// cmp+bz，cmp的类型需要取反
					// !!!
					register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
					reg_mips_branch(codes, peek->OpA->name, map[q->OpA], map[q->OpB], cmp_type);
					quater += 1;
				}
			}
			// compare is not for a branch
			else {
				auto map = register_pool->request(q->OpA, q->OpB, q->Result);
				reg_mips_cmp(codes, map[q->OpA], map[q->OpB], map[q->Result], type);
			}
		}
		else if (type == Goto) {
			// !!!
			register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
			reg_mips_j(codes, q->OpA->name);
		}
		else if (type == Bz) {
			auto map = register_pool->request(NULL, q->OpB, NULL);
			register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
			reg_mips_bz(codes, q->OpA->name, map[q->OpB]);
		}
		else if (type == Bnz) {
			auto map = register_pool->request(NULL, q->OpB, NULL);
			register_pool->clear_all_and_dump_temp_active(quater_basicblock[q]->active_out);
			reg_mips_bnz(codes, q->OpA->name, map[q->OpB]);
		}
		else if (type == SetLabel) {
			reg_mips_label(codes, q->OpA->name);
		}
		else if (type == ArrayQuery) {		
			auto map = register_pool->request(NULL, q->OpB, q->Result);
			reg_mips_array_query(codes, map[q->Result], q->OpA, map[q->OpB], array_temp, &(*this));
		}
		else if (type == Scan) {
			auto map = register_pool->request(NULL, NULL, q->Result);
			reg_mips_scan(codes, map[q->Result], q->Result->basic_type);
		}
		else if (type == Print) {
			register_pool->specific_request(q->OpA, a0);
			reg_mips_print(codes, a0, q->OpA->basic_type);
		}
		else {
			// DEBUG_PRINT("[ERROR] Unknown Quater Type.");
		}

		// !!! 如果被识别为基本块的最后一句话，则需要dump活跃变量
		if ((quater != quater_list.end() - 1 && quater_basicblock[*quater] != quater_basicblock[*(quater + 1)]) ||
			quater == quater_list.end() - 1) {
			// 如果是跳转类语句（branch, goto, funcall, funcret) 则无需处理，因此已经处理过了
			auto type = (*quater)->type;
			if (type == Goto || type == Bnz || type == Bz || type == FuncCall || type == FuncRet){}
			else {
				register_pool->clear_all_and_dump_temp_active(quater_basicblock[*quater]->active_out);
			}
			reg_mips_string(codes, "");
			reg_mips_string(codes, "");
		}
	}

	// 增加函数结束注释
	reg_mips_comment(codes, "Function Ends Here " + func_head->name);
	return *codes;
}

void RegMipsFunction::get_addr(SymbolItem* item, bool* local, int* offset, string* data_label) {
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