#ifndef __QUATERNARY_H__
#define __QUATERNARY_H__

#include "SymbolItem.h"

/* 除数值以外，其余均为SymbolItem类，f - function, c - const, v - variable, t - temp, l - label */
/* 四元式类型 ------------- 解释 ---------------------- 操作数A ----------- 操作数B ------------ 结果数 */
enum QuaterType
{
	Undefined,
	// 声明
	VarDeclar,		//   声明变量						None				None			v
	ConstDeclar,	//   常量声明						None				None			c		
	// 函数
	FuncDeclar,		//	 函数声明						None				None			f
	FuncParaPush,	//   函数变量推入						v/c/t				None			None
	FuncCall,		//   函数调用						f					None			None
	AssignRet,		//	 返回值赋值						None				None			v/t
	FuncRet,		//   函数返回						(v/t)				None			None
	// 算术
	Add,			//   算术加法						v/c/t				v/c/t			v/t
	Sub,			//   算术减法						(v/c/t)				v/c/t			v/t
	Mult,			//   算术乘法						v/c/t				v/c/t			v/t
	Div,			//	 算术除法						v/c/t				v/c/t			v/t
	Assign,			//   赋值							v/c/t				None			v/t
	// 条件
	EqlCmp,			//	 等值比较						v/c/t				v/c/t			v/t
	NeqCmp,			//	 不等比较						v/c/t				v/c/t			v/t
	GtCmp,			//	 大于比较						v/c/t				v/c/t			v/t
	GeqCmp,			//	 大于等于比较						v/c/t				v/c/t			v/t
	LtCmp,			//	 小于比较						v/c/t				v/c/t			v/t
	LeqCmp,			//	 小于等于比较						v/c/t				v/c/t			v/t
	// 分支
	Goto,			//   无条件跳转						l					None			None
	Bnz,			//	 有条件跳转(~0)					l					v/c/t			None
	Bz,				//	 有条件跳转(0)					l					v/c/t			None
	// 标签
	SetLabel		//   设置标签						l					None			None
};

// 四元式类
class Quaternary
{
public:
	Quaternary(QuaterType, SymbolItem*, SymbolItem*, SymbolItem*);
protected:
	QuaterType type;
	SymbolItem* OpA;
	SymbolItem* OpB;
	SymbolItem* Result;
};

// 存放四元式的容器
extern vector<Quaternary*> QuaterList;

Quaternary* GetVarDeclarQuater(SymbolItem* result);
Quaternary* GetConstDeclarQuater(SymbolItem* result);
Quaternary* GetFuncDeclarQuater(SymbolItem* result);
Quaternary* GetFuncParaPushQuater(SymbolItem* A);
Quaternary* GetFuncCallQuater(SymbolItem* A);
Quaternary* GetAssignRetQuater(SymbolItem* result);
Quaternary* GetFuncRetQuater(SymbolItem* A);
Quaternary* GetAddQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetSubQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetMultQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetDivQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetAssignQuater(SymbolItem* A, SymbolItem* Result);
Quaternary* GetEqlCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetNeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGotoQuater(SymbolItem* A);
Quaternary* GetBnzQuater(SymbolItem* A);
Quaternary* GetBzQuater(SymbolItem* A);
Quaternary* GetSetLabelQuater(SymbolItem* A);

#endif // !1

