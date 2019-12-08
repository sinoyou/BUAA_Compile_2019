#include "Quaternary.h"
#include "debug.h"
#include <map>
#include <set>

QuaterType cmp_reverse(QuaterType cmp_type) {
	switch (cmp_type)
	{
	case EqlCmp:
		return NeqCmp;
	case NeqCmp:
		return EqlCmp;
	case GtCmp:
		return LeqCmp;
	case GeqCmp:
		return LtCmp;
	case LtCmp:
		return GeqCmp;
	case LeqCmp:
		return GtCmp;
	default:
		char buf[100];
		sprintf(buf, "[ERROR] Unexpected Compare Type in cmp_fit_from_bnz_to_bz %d", cmp_type);
		DEBUG_PRINT(buf);
	}
	return QuaterType::Undefined;
}

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
			sprintf(buf, "[WARNING] Empty SymbolItem in Quaternary %d", q->type);
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

/* 返回该段中间码的函数集合 */
vector<SymbolItem*> get_funcs(vector<Quaternary*>* list) {
	set<SymbolItem*> func_set;
	for (auto it = list->begin(); it != list->end(); it++) {
		if ((*it)->type == QuaterType::FuncDeclar)
			func_set.insert(func_set.end(),(*it)->Result);
	}
	vector<SymbolItem*> func_list;
	for (auto it = func_set.begin(); it != func_set.end(); it++)
		func_list.push_back(*it);
	return func_list;
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