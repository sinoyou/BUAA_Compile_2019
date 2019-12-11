#include "InlineOptimizer.h"
#include "Quaternary.h"
#include "debug.h"
#include <vector>
#include <set>
#include <map>
#include <string>
#include "Block.h"
#include <list>
#include "SymbolFactory.h"

using namespace std;

#define INLINE_MIDDLE_LIMIT 50

/* 
 * 内联函数优化整体思路
 * 1. 对于Caller，扫描可以内联的函数，对于每个可内联函数记录push_list和ret_idenfr
 * 2. 对于Callee
 *    Step 0: 筛选本函数的对应中间码
 *    Step 1: 属于函数的SymbolItem副本内联化并建立替换映射（注意name和label要变化）
 *    Step 2: 四元式的内联化（依托替换映射map）
 *    Step 3: FuncDeclar的删除，FuncPara的删除和中间码内容替换
 *    Step 4: return语句与ret_idenfr的替换，并新增跳转至结尾的语句。
 *    Step 5: 在Callee变换完后的新列表最后增加结束的label。
*/

void InlineOptimizer::preprocess() {
	// 将全局中间码预先推入
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->block->pre == NULL)
			optimized_code.push_back(*it);
	}
}

void InlineOptimizer::optimizer() {
	// 获取中间码中的所有函数
	auto funcs = get_funcs(&middle_code);
	for (auto it = funcs.begin(); it != funcs.end(); it++) {
		auto func = *it;
		auto func_middle = get_quaternary_by_function(func, &middle_code);		// func原始的middle code
		// Caller-1：寻找所有可行内联函数
		for (auto quater = func_middle.begin(); quater != func_middle.end(); quater++) {
			// 非函数调用语句，不管
			if ((*quater)->type != QuaterType::FuncCall) continue;
			// 不适合内联，不管
			if (!inline_check(func, (*quater)->OpA, &middle_code)) continue;
			auto callee = (*quater)->OpA;
			/* 适合内联的函数，开干 */
			printf("[Inline Optimizer] %s() in %s() is inline avaliable.\n", 
				callee->getname().c_str(), func->getname().c_str());
				// 绑定调用者的函数调用区间
				auto head = quater;
				auto tail = quater;
				while ((*(head - 1))->type == QuaterType::FuncParaPush)
					head--;
				while ((*(tail + 1))->type == QuaterType::AssignRet)
					tail++;
				// 参数统计
				vector<SymbolItem*> parameters;
				for (auto para_it = head; para_it != quater; para_it++) parameters.push_back((*para_it)->OpA);
				SymbolItem* ret_idenfr = (quater == tail) ? NULL : (*tail)->Result;
				// 删除Caller的调用部分
				tail = func_middle.erase(head, tail + 1);
				// 获取inline部分中间码并拼接
				auto inline_code = inline_generator(func, callee, parameters, ret_idenfr, &middle_code);
				head = func_middle.insert(tail, inline_code.begin(), inline_code.end());
				// 重置quater指针位置
				quater = head + inline_code.size() - 1;
		}
		// 插入内联化后的代码
		for (auto jt = func_middle.begin(); jt != func_middle.end(); jt++)
			optimized_code.push_back(*jt);
	}
}

/* 被调用函数的中间代码inline格式生成器 
 * caller: 调用者
 * func: 被调用函数
 * paras：属于调用者的参数，表示被推入的参数
 * ret: 属于调用者的参数，表示返回值赋值的标识符，可为NULL
 * middle: 所有中间码
*/
vector<Quaternary*> inline_generator(SymbolItem* caller, SymbolItem* func, 
	vector<SymbolItem*> paras, SymbolItem* ret, vector<Quaternary*>* middle) {
	auto begin_label = SymbolFactory::create_label(caller->block, "inline_" + func->getname() +"_begin");
	auto end_label = SymbolFactory::create_label(caller->block, "inline_" + func->getname() + "_end");
	vector<Quaternary*> inline_code;
	// step 0
	auto middle_code = get_quaternary_by_function(func, middle);
	// step 1
	map<SymbolItem*, SymbolItem*> copy_map;
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->type == QuaterType::FuncDeclar)
			continue;
		auto items = get_items_by_quater(*it);
		for (auto jt = items.begin(); jt != items.end(); jt++) {
			auto item = (*jt).second;
			// 没有映射过，并且是属于inline函数自己的SymbolItem
			if (copy_map.find(item) == copy_map.end() and item->block == func->block) {
				SymbolItem* inline_item = SymbolFactory::create_inline_copy(caller->block, item);
				copy_map[item] = inline_item;
			}
		}
	}
	// step 2
	if (middle_code[0]->type != QuaterType::FuncDeclar)
		DEBUG_PRINT("[ERROR]: Function Declar Is not the first Quater of Function Quaters.");
	for (auto it = middle_code.begin(); it != middle_code.end(); it++)
		if((*it)->type != QuaterType::FuncDeclar)
			inline_code.push_back(GetInlineCopyQuater(caller->block, *it, copy_map));
	// step 3
	int cnt = 0;
	auto head = inline_code.begin();
	while ((*head)->type == QuaterType::FuncParaDeclar) {
		SymbolItem* para = (*head)->Result;
		SymbolItem* para_out = paras[cnt];

		Quaternary* q = new Quaternary(caller->block, QuaterType::Assign, para_out, NULL, para);
		head = inline_code.erase(head);
		head = inline_code.insert(head, q);
		head += 1;
		cnt++;
	}
	if (cnt != paras.size())
		DEBUG_PRINT("[ERROR] Paramertes number not match");
	// step 4
	for (auto it = inline_code.begin(); it != inline_code.end(); it = (it == inline_code.end()) ? it : it + 1 ) {
		if ((*it)->type == QuaterType::FuncRet) {
			if (ret == NULL && (*it)->OpA == NULL)
			{
				it = inline_code.erase(it);
				it--;
			}
			else if(ret != NULL && (*it)->OpA != NULL){
				auto q = GetAssignQuater(caller->block, (*it)->OpA, ret, false);
				it = inline_code.insert(it, q);													// assign*, ret
				it++;																			// assign, ret*
				it = inline_code.erase(it);														// assign, other*
				if (it != inline_code.end()) {
					q = GetGotoQuater(caller->block, end_label, false);
					it = inline_code.insert(it, q);												// assign, goto*, other
				}
			}
			else {
				DEBUG_PRINT("[ERROR] Return Type not match.");
			}
		}
	}
	// step 5
	auto q = GetSetLabelQuater(caller->block, begin_label, false);
	inline_code.insert(inline_code.begin(), q);
	q = GetSetLabelQuater(caller->block, end_label, false);
	inline_code.push_back(q);

	return inline_code;
}


/* BFS搜索函数A是否可以在函数B中被内联 */
bool inline_check(SymbolItem* func, SymbolItem* func_inline, vector<Quaternary*>* middle) {
	// A = B
	if (func_inline == func)
		return false;
	// loop in func_inline
	auto stat_count = func_inline->block->statement_count;
	if (stat_count.find(StatementType::for_stat) != stat_count.end())
		return false;
	if (stat_count.find(StatementType::while_stat) != stat_count.end())
		return false;
	if (stat_count.find(StatementType::dowhile_stat) != stat_count.end())
		return false;
	// too many middle codes in func_inline
	vector<Quaternary*> sub_middle = get_quaternary_by_function(func_inline, middle);
	if (sub_middle.size() > INLINE_MIDDLE_LIMIT)
		return false;
	// Pass check
	return true;
}

/* 寻找func函数中被调用的函数 */
set<SymbolItem*> called_func_in_one_func(SymbolItem* func, vector<Quaternary*>* middle) {
	set<SymbolItem*> called_func;
	for (auto it = middle->begin(); it != middle->end(); it++) {
		auto quater = *it;
		// 属于func的函数调用四元式
		if (quater->block->func_head == func && quater->type == QuaterType::FuncCall) {
			called_func.insert(quater->OpA);
		}
	}
	return called_func;
}