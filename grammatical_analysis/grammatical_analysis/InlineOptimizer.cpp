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
 * ���������Ż�����˼·
 * 1. ����Caller��ɨ����������ĺ���������ÿ��������������¼push_list��ret_idenfr
 * 2. ����Callee
 *    Step 0: ɸѡ�������Ķ�Ӧ�м���
 *    Step 1: ���ں�����SymbolItem�����������������滻ӳ�䣨ע��name��labelҪ�仯��
 *    Step 2: ��Ԫʽ���������������滻ӳ��map��
 *    Step 3: FuncDeclar��ɾ����FuncPara��ɾ�����м��������滻
 *    Step 4: return�����ret_idenfr���滻����������ת����β����䡣
 *    Step 5: ��Callee�任�������б�������ӽ�����label��
*/

void InlineOptimizer::preprocess() {
	// ��ȫ���м���Ԥ������
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->block->pre == NULL)
			optimized_code.push_back(*it);
	}
}

void InlineOptimizer::optimizer() {
	// ��ȡ�м����е����к���
	auto funcs = get_funcs(&middle_code);
	for (auto it = funcs.begin(); it != funcs.end(); it++) {
		auto func = *it;
		auto func_middle = get_quaternary_by_function(func, &middle_code);		// funcԭʼ��middle code
		// Caller-1��Ѱ�����п�����������
		for (auto quater = func_middle.begin(); quater != func_middle.end(); quater++) {
			// �Ǻ���������䣬����
			if ((*quater)->type != QuaterType::FuncCall) continue;
			// ���ʺ�����������
			if (!inline_check(func, (*quater)->OpA, &middle_code)) continue;
			auto callee = (*quater)->OpA;
			/* �ʺ������ĺ��������� */
			printf("[Inline Optimizer] %s() in %s() is inline avaliable.\n", 
				callee->getname().c_str(), func->getname().c_str());
				// �󶨵����ߵĺ�����������
				auto head = quater;
				auto tail = quater;
				while ((*(head - 1))->type == QuaterType::FuncParaPush)
					head--;
				while ((*(tail + 1))->type == QuaterType::AssignRet)
					tail++;
				// ����ͳ��
				vector<SymbolItem*> parameters;
				for (auto para_it = head; para_it != quater; para_it++) parameters.push_back((*para_it)->OpA);
				SymbolItem* ret_idenfr = (quater == tail) ? NULL : (*tail)->Result;
				// ɾ��Caller�ĵ��ò���
				tail = func_middle.erase(head, tail + 1);
				// ��ȡinline�����м��벢ƴ��
				auto inline_code = inline_generator(func, callee, parameters, ret_idenfr, &middle_code);
				head = func_middle.insert(tail, inline_code.begin(), inline_code.end());
				// ����quaterָ��λ��
				quater = head + inline_code.size() - 1;
		}
		// ������������Ĵ���
		for (auto jt = func_middle.begin(); jt != func_middle.end(); jt++)
			optimized_code.push_back(*jt);
	}
}

/* �����ú������м����inline��ʽ������ 
 * caller: ������
 * func: �����ú���
 * paras�����ڵ����ߵĲ�������ʾ������Ĳ���
 * ret: ���ڵ����ߵĲ�������ʾ����ֵ��ֵ�ı�ʶ������ΪNULL
 * middle: �����м���
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
			// û��ӳ���������������inline�����Լ���SymbolItem
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


/* BFS��������A�Ƿ�����ں���B�б����� */
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

/* Ѱ��func�����б����õĺ��� */
set<SymbolItem*> called_func_in_one_func(SymbolItem* func, vector<Quaternary*>* middle) {
	set<SymbolItem*> called_func;
	for (auto it = middle->begin(); it != middle->end(); it++) {
		auto quater = *it;
		// ����func�ĺ���������Ԫʽ
		if (quater->block->func_head == func && quater->type == QuaterType::FuncCall) {
			called_func.insert(quater->OpA);
		}
	}
	return called_func;
}