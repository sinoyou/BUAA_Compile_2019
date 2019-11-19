#include "Quaternary.h"
#include "debug.h"
#include "Block.h"	

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

// VarDeclar
Quaternary* GetVarDeclarQuater(Block* block, SymbolItem* result) {
	assert_v(result);
	Quaternary* p = new Quaternary(block, QuaterType::VarDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// ConstDeclar
Quaternary* GetConstDeclarQuater(Block* block, SymbolItem* result) {
	assert_c(result);
	Quaternary* p = new Quaternary(block, QuaterType::ConstDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncDeclar
Quaternary* GetFuncDeclarQuater(Block* block, SymbolItem* result) {
	assert_f(result);
	Quaternary* p = new Quaternary(block, QuaterType::FuncDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncParaDeclar
Quaternary* GetFuncParaDeclarQuater(Block* block, SymbolItem* result) {
	assert_v(result);
	Quaternary* p = new Quaternary(block, QuaterType::FuncParaDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncParaPush
Quaternary* GetFuncParaPushQuater(Block* block, SymbolItem* A) {
	assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncParaPush, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// FuncCall
Quaternary* GetFuncCallQuater(Block* block, SymbolItem* A) {
	assert_f(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncCall, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// AssignRet
Quaternary* GetAssignRetQuater(Block* block, SymbolItem* result) {
	assert_vt(result);
	Quaternary* p = new Quaternary(block, QuaterType::AssignRet, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncRet
Quaternary* GetFuncRetQuater(Block* block, SymbolItem* A) {
	if (A != NULL) 
		assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::FuncRet, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// Add
Quaternary* GetAddQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Add, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Sub
Quaternary* GetSubQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	if(A != NULL)
		assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Sub, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Mult
Quaternary* GetMultQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Mult, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Div
Quaternary* GetDivQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Div, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Assign
Quaternary* GetAssignQuater(Block* block, SymbolItem* A, SymbolItem* Result) {
	assert_vct(A);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Assign, A, NULL, Result);
	QuaterList.push_back(p);
	return p;
}

// Assign Array
Quaternary* GetAssignArrayQuater(Block* block, SymbolItem* A, SymbolItem* B,SymbolItem* Result) {
	assert_vct(A);		// value
	assert_vct(B);		// index
	assert_v(Result);	// idenfr
	Quaternary* p = new Quaternary(block, QuaterType::AssignArray, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// EqlCmp
Quaternary* GetEqlCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::EqlCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// NeqCmp
Quaternary* GetNeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::NeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// GtCmp
Quaternary* GetGtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::GtCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// GeqCmp
Quaternary* GetGeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::GeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// LtCmp
Quaternary* GetLtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::LtCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// LeqCmp
Quaternary* GetLeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::LeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Goto
Quaternary* GetGotoQuater(Block* block, SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(block, QuaterType::Goto, A, NULL,NULL);
	QuaterList.push_back(p);
	return p;
}

// Bnz
Quaternary* GetBnzQuater(Block* block, SymbolItem* label, SymbolItem* condition) {
	assert_l(label);
	assert_vct(condition);
	Quaternary* p = new Quaternary(block, QuaterType::Bnz, label, condition, NULL);
	QuaterList.push_back(p);
	return p;
}

// Bz
Quaternary* GetBzQuater(Block* block, SymbolItem* label, SymbolItem* condition) {
	assert_l(label);
	assert_vct(condition);
	Quaternary* p = new Quaternary(block, QuaterType::Bz, label, condition, NULL);
	QuaterList.push_back(p);
	return p;
}

// SetLabel
Quaternary* GetSetLabelQuater(Block* block, SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(block, QuaterType::SetLabel, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// ArrayQuery
Quaternary* GetArrayQueryQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_v(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(block, QuaterType::ArrayQuery, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Scan
Quaternary* GetScanQuater(Block* block, SymbolItem* Result) {
	assert_v(Result);
	Quaternary* p = new Quaternary(block, QuaterType::Scan, NULL, NULL, Result);
	QuaterList.push_back(p);
	return p;
}

// Print
Quaternary* GetPrintQuater(Block* block, SymbolItem* A) {
	assert_vct(A);
	Quaternary* p = new Quaternary(block, QuaterType::Print, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}
