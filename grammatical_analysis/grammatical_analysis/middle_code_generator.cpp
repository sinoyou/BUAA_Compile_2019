#include "Quaternary.h"
#include "debug.h"
#include "Block.h"	

// 存放四元式的容器
vector<Quaternary*> QuaterList;

// 工具函数 - assert_检查
void assert_vct(SymbolItem* item) {
	if (item->type == SymbolItemType::_variable ||
		item->type == SymbolItemType::_const ||
		item->type == SymbolItemType::temp) {
	}
	else {
		string mes = "[Error] Expected vct not match " + item->name + " @ " + item->block->func_head->name;
		DEBUG_PRINT(mes.c_str());
	}
}

void assert_vt(SymbolItem* item) {
	if (item->type == SymbolItemType::_variable ||
		item->type == SymbolItemType::temp) {
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
Quaternary* GetVarDeclarQuater(SymbolItem* result) {
	assert_v(result);
	Quaternary* p = new Quaternary(QuaterType::VarDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// ConstDeclar
Quaternary* GetConstDeclarQuater(SymbolItem* result) {
	assert_c(result);
	Quaternary* p = new Quaternary(QuaterType::ConstDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncDeclar
Quaternary* GetFuncDeclarQuater(SymbolItem* result) {
	assert_f(result);
	Quaternary* p = new Quaternary(QuaterType::FuncDeclar, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncParaPush
Quaternary* GetFuncParaPushQuater(SymbolItem* A) {
	assert_vct(A);
	Quaternary* p = new Quaternary(QuaterType::FuncParaPush, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// FuncCall
Quaternary* GetFuncCallQuater(SymbolItem* A) {
	assert_f(A);
	Quaternary* p = new Quaternary(QuaterType::FuncCall, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// AssignRet
Quaternary* GetAssignRetQuater(SymbolItem* result) {
	assert_vt(result);
	Quaternary* p = new Quaternary(QuaterType::AssignRet, NULL, NULL, result);
	QuaterList.push_back(p);
	return p;
}

// FuncRet
Quaternary* GetFuncRetQuater(SymbolItem* A) {
	if (A != NULL) 
		assert_vt(A);
	Quaternary* p = new Quaternary(QuaterType::FuncRet, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// Add
Quaternary* GetAddQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::Add, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Sub
Quaternary* GetSubQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	if(A != NULL)
		assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::Sub, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Mult
Quaternary* GetMultQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::Mult, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Div
Quaternary* GetDivQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::Div, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Assign
Quaternary* GetAssignQuater(SymbolItem* A, SymbolItem* Result) {
	assert_vct(A);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::Assign, A, NULL, Result);
	QuaterList.push_back(p);
	return p;
}

// EqlCmp
Quaternary* GetEqlCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::EqlCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// NeqCmp
Quaternary* GetNeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::NeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// GtCmp
Quaternary* GetGtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::GtCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// GeqCmp
Quaternary* GetGeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::GeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// LtCmp
Quaternary* GetLtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::LtCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// LeqCmp
Quaternary* GetLeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	assert_vct(A);
	assert_vct(B);
	assert_vt(Result);
	Quaternary* p = new Quaternary(QuaterType::LeqCmp, A, B, Result);
	QuaterList.push_back(p);
	return p;
}

// Goto
Quaternary* GetGotoQuater(SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(QuaterType::Goto, A, NULL,NULL);
	QuaterList.push_back(p);
	return p;
}

// Bnz
Quaternary* GetBnzQuater(SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(QuaterType::Bnz, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// Bz
Quaternary* GetBzQuater(SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(QuaterType::Bz, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}

// SetLabel
Quaternary* GetSetLabelQuater(SymbolItem* A) {
	assert_l(A);
	Quaternary* p = new Quaternary(QuaterType::SetLabel, A, NULL, NULL);
	QuaterList.push_back(p);
	return p;
}