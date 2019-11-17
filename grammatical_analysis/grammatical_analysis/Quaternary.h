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
	FuncParaDeclar,	//   函数变量声明						None				None			v
	FuncParaPush,	//   函数变量推入						v/c/t				None			None
	FuncCall,		//   函数调用						f					None			None
	AssignRet,		//	 返回值赋值						None				None			v/t
	FuncRet,		//   函数返回						(v/c/t)				None			None
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
	SetLabel,		//   设置标签						l					None			None
	// 数组
	ArrayQuery,		//	 根据索引取值						v					v/c/t			v/t			
	AssignArray,	//   数组赋值						v/c/t				v/c/t(indx)		v
	// IO
	Scan,			//   输入							None				None			v
	Print			//   输出							v/t/c				None			None
};

// 四元式类
class Quaternary
{
public:
	Quaternary(QuaterType type, SymbolItem* A, SymbolItem* B, SymbolItem* Result) :
		type(type), OpA(A), OpB(B), Result(Result) {}
protected:
	QuaterType type;
	SymbolItem* OpA;
	SymbolItem* OpB;
	SymbolItem* Result;
};

// 存放四元式的容器
extern vector<Quaternary*> QuaterList;

// 四元式生成辅助函数
Quaternary* GetVarDeclarQuater(SymbolItem* result);
Quaternary* GetConstDeclarQuater(SymbolItem* result);
Quaternary* GetFuncDeclarQuater(SymbolItem* result);
Quaternary* GetFuncParaDeclarQuater(SymbolItem* result);
Quaternary* GetFuncParaPushQuater(SymbolItem* A);
Quaternary* GetFuncCallQuater(SymbolItem* A);
Quaternary* GetAssignRetQuater(SymbolItem* result);
Quaternary* GetFuncRetQuater(SymbolItem* A);
Quaternary* GetAddQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetSubQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetMultQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetDivQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetAssignQuater(SymbolItem* A, SymbolItem* Result);
Quaternary* GetAssignArrayQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetEqlCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetNeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLtCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLeqCmpQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGotoQuater(SymbolItem* A);
Quaternary* GetBnzQuater(SymbolItem* label, SymbolItem* condition);
Quaternary* GetBzQuater(SymbolItem* label, SymbolItem* condition);
Quaternary* GetSetLabelQuater(SymbolItem* A);
Quaternary* GetArrayQueryQuater(SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetScanQuater(SymbolItem* Result);
Quaternary* GetPrintQuater(SymbolItem* A);

// 四元式输出辅助函数
void PutQuaterHandler(vector<string>* PutList, vector<Quaternary>* QuaterList);
string PutVarDeclarQuater(Quaternary* Q);
string PutConstDeclarQuater(Quaternary* Q);
string PutFuncDeclarQuater(Quaternary* Q);
string PutFuncParaDeclarQuater(Quaternary* Q);
string PutFuncParaPushQuater(Quaternary* Q);
string PutFuncCallQuater(Quaternary* Q);
string PutAssignRetQuater(Quaternary* Q);
string PutFuncRetQuater(Quaternary* Q);
string PutAddQuater(Quaternary* Q);
string PutSubQuater(Quaternary* Q);
string PutMultQuater(Quaternary* Q);
string PutDivQuater(Quaternary* Q);
string PutAssignQuater(Quaternary* Q);
string PutAssignArrayQuater(Quaternary* Q);
string PutEqlCmpQuater(Quaternary* Q);
string PutNeqCmpQuater(Quaternary* Q);
string PutGtCmpQuater(Quaternary* Q);
string PutGeqCmpQuater(Quaternary* Q);
string PutLtCmpQuater(Quaternary* Q);
string PutLeqCmpQuater(Quaternary* Q);
string PutGotoQuater(Quaternary* Q);
string PutBnzQuater(Quaternary* Q);
string PutBzQuater(Quaternary* Q);
string PutSetLabelQuater(Quaternary* Q);
string PutArrayQueryQuater(Quaternary* Q);
string PutScanQuater(Quaternary* Q);
string PutPrintQuater(Quaternary* Q);

#endif // !1

