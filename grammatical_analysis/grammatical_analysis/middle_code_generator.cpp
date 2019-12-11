#include "Quaternary.h"
#include "debug.h"
#include "Block.h"	
#include <iostream>

// 存放四元式的容器
vector<Quaternary*> QuaterList;

// 工具函数 - assert_检查
void assert_vct(SymbolItem* item) {
	if (item->type == SymbolItemType::_variable ||
		item->type == SymbolItemType::_const ||
		item->type == SymbolItemType::temp_normal ||
		item->type == SymbolItemType::temp_const ||
		item->type == SymbolItemType::temp_strcon) {
	}
	else {
		string mes = "[Error] Expected vct not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_vt(SymbolItem* item) {
	if (item->type == SymbolItemType::_variable ||
		item->type == SymbolItemType::temp_normal||
		item->type == SymbolItemType::temp_const ||
		item->type == SymbolItemType::temp_strcon) {
	}
	else {
		string mes = "[Error] Expected vt not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_v(SymbolItem* item) {
	if (item->type == SymbolItemType::_variable) {}
	else {
		string mes = "[Error] Expected v not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_c(SymbolItem* item) {
	if (item->type == SymbolItemType::_const) {}
	else {
		string mes = "[Error] Expected c not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_f(SymbolItem* item) {
	if (item->type == SymbolItemType::function) {}
	else {
		string mes = "[Error] Expected f not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_l(SymbolItem* item) {
	if (item->type == SymbolItemType::label) {}
	else {
		string mes = "[Error] Expected l not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

/* 产生用于inline嵌入的四元式副本 */
SymbolItem* ItemTransfer(SymbolItem* item, map<SymbolItem*, SymbolItem*> copy_map) {
	if (item == NULL)
		return NULL;
	else if (copy_map.find(item) != copy_map.end())
		return copy_map[item];
	else {
		if (item->block->pre != NULL && item->type != SymbolItemType::function) {
			char buf[200];
			sprintf(buf, "[Error]: Item inline transfer failed, neither in copy_map nor global/function");
			DEBUG_PRINT(buf);
		}
		return item;
	}
}
Quaternary* GetInlineCopyQuater(Block* block, Quaternary* original, map<SymbolItem*, SymbolItem*> copy_map) {
	SymbolItem* A = ItemTransfer(original->OpA, copy_map);
	SymbolItem* B = ItemTransfer(original->OpB, copy_map);
	SymbolItem* Result = ItemTransfer(original->Result, copy_map);
	Quaternary* p = new Quaternary(block, original->type, A, B, Result);
	return p; // 不推入四元式列表中
}

// VarDeclar
Quaternary* GetVarDeclarQuater(Block* block, SymbolItem* result, bool push) {
	assert_v(result);
	Quaternary* p = new Quaternary(block, QuaterType::VarDeclar, NULL, NULL, result);
	if(push)
		QuaterList.push_back(p);
	return p;
}

// ConstDeclar
Quaternary* GetConstDeclarQuater(Block* block, SymbolItem* result, bool push) {
	assert_c(result);
	Quaternary* p = new Quaternary(block, QuaterType::ConstDeclar, NULL, NULL, result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// FuncDeclar
Quaternary* GetFuncDeclarQuater(Block* block, SymbolItem* result, bool push) {
	assert_f(result);
	Quaternary* p = new Quaternary(block, QuaterType::FuncDeclar, NULL, NULL, result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// FuncParaDeclar
Quaternary* GetFuncParaDeclarQuater(Block* block, SymbolItem* result, bool push) {
	assert_v(result);
	Quaternary* p = new Quaternary(block, QuaterType::FuncParaDeclar, NULL, NULL, result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// FuncParaPush
Quaternary* GetFuncParaPushQuater(Block* block, SymbolItem* A, bool push) {
	assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncParaPush, A, NULL, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// FuncCall
Quaternary* GetFuncCallQuater(Block* block, SymbolItem* A, bool push) {
	assert_f(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncCall, A, NULL, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// AssignRet
Quaternary* GetAssignRetQuater(Block* block, SymbolItem* result, bool push) {
	assert_vt(result);
	Quaternary* p = new Quaternary(block, QuaterType::AssignRet, NULL, NULL, result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// FuncRet
Quaternary* GetFuncRetQuater(Block* block, SymbolItem* A, bool push) {
	if (A != NULL) 
		assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncRet, A, NULL, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Add
Quaternary* GetAddQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Add, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Sub
Quaternary* GetSubQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	if(A != NULL)
		assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Sub, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Mult
Quaternary* GetMultQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Mult, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Div
Quaternary* GetDivQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Div, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Assign
Quaternary* GetAssignQuater(Block* block, SymbolItem* A, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Assign, A, NULL, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Assign Array
Quaternary* GetAssignArrayQuater(Block* block, SymbolItem* A, SymbolItem* B,SymbolItem* Result, bool push) {
	assert_vct(A);		// value
	assert_vct(B);		// index
	assert_v(Result);	// idenfr
	Quaternary* p = new Quaternary(block, QuaterType::AssignArray, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// EqlCmp
Quaternary* GetEqlCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::EqlCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);	
	return p;
}

// NeqCmp
Quaternary* GetNeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::NeqCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// GtCmp
Quaternary* GetGtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::GtCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// GeqCmp
Quaternary* GetGeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::GeqCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// LtCmp
Quaternary* GetLtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::LtCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// LeqCmp
Quaternary* GetLeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::LeqCmp, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Goto
Quaternary* GetGotoQuater(Block* block, SymbolItem* A, bool push) {
	assert_l(A);
	Quaternary* p = new Quaternary(block, QuaterType::Goto, A, NULL,NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Bnz
Quaternary* GetBnzQuater(Block* block, SymbolItem* label, SymbolItem* condition, bool push) {
	assert_l(label);
	assert_vct(condition);
	Quaternary* p = new Quaternary(block, QuaterType::Bnz, label, condition, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Bz
Quaternary* GetBzQuater(Block* block, SymbolItem* label, SymbolItem* condition, bool push) {
	assert_l(label);
	assert_vct(condition);
	Quaternary* p = new Quaternary(block, QuaterType::Bz, label, condition, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// SetLabel
Quaternary* GetSetLabelQuater(Block* block, SymbolItem* A, bool push) {
	assert_l(A);
	Quaternary* p = new Quaternary(block, QuaterType::SetLabel, A, NULL, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// ArrayQuery
Quaternary* GetArrayQueryQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push) {
	assert_v(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::ArrayQuery, A, B, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Scan
Quaternary* GetScanQuater(Block* block, SymbolItem* Result, bool push) {
	assert_v(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Scan, NULL, NULL, Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Print
Quaternary* GetPrintQuater(Block* block, SymbolItem* A, bool push) {
	assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::Print, A, NULL, NULL);
	if (push)
		QuaterList.push_back(p);
	return p;
}

// Copy
Quaternary* GetCopyQuater(Block* block, Quaternary* original, bool push)
{
	Quaternary* p = new Quaternary(original->block, original->type, original->OpA, original->OpB, original->Result);
	if (push)
		QuaterList.push_back(p);
	return p;
}