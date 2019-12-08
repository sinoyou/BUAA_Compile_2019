#ifndef __QUATERNARY_H__
#define __QUATERNARY_H__

#include "SymbolItem.h"
#include <map>

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
	Quaternary(Block* block, QuaterType type, SymbolItem* A, SymbolItem* B, SymbolItem* Result) :
		block(block), type(type), OpA(A), OpB(B), Result(Result) {}

	Block* block;

	QuaterType type;
	SymbolItem* OpA;
	SymbolItem* OpB;
	SymbolItem* Result;
};

// 存放四元式的容器
extern vector<Quaternary*> QuaterList;

// 四元式生成辅助函数
Quaternary* GetInlineCopyQuater(Block* block, Quaternary* original, map<SymbolItem*, SymbolItem*> copy_map);
Quaternary* GetVarDeclarQuater(Block* block, SymbolItem* result, bool push=true);
Quaternary* GetConstDeclarQuater(Block* block, SymbolItem* result, bool push = true);
Quaternary* GetFuncDeclarQuater(Block* block, SymbolItem* result, bool push = true);
Quaternary* GetFuncParaDeclarQuater(Block* block, SymbolItem* result, bool push = true);
Quaternary* GetFuncParaPushQuater(Block* block, SymbolItem* A, bool push = true);
Quaternary* GetFuncCallQuater(Block* block, SymbolItem* A, bool push = true);
Quaternary* GetAssignRetQuater(Block* block, SymbolItem* result, bool push = true);
Quaternary* GetFuncRetQuater(Block* block, SymbolItem* A, bool push = true);
Quaternary* GetAddQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetSubQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetMultQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetDivQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetAssignQuater(Block* block, SymbolItem* A, SymbolItem* Result, bool push = true);
Quaternary* GetAssignArrayQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetEqlCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push=true);
Quaternary* GetNeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push=true);
Quaternary* GetGtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push=true);
Quaternary* GetGeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push=true);
Quaternary* GetLtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push=true);
Quaternary* GetLeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetGotoQuater(Block* block, SymbolItem* A, bool push = true);
Quaternary* GetBnzQuater(Block* block, SymbolItem* label, SymbolItem* condition, bool push = true);
Quaternary* GetBzQuater(Block* block, SymbolItem* label, SymbolItem* condition, bool push = true);
Quaternary* GetSetLabelQuater(Block* block, SymbolItem* A, bool push = true);
Quaternary* GetArrayQueryQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result, bool push = true);
Quaternary* GetScanQuater(Block* block, SymbolItem* Result, bool push = true);
Quaternary* GetPrintQuater(Block* block, SymbolItem* A, bool push = true);

// 四元式输出辅助函数
vector<string> PrintQuater(vector<Quaternary*>* QuaterList);
string PrintQuaterHandler(Quaternary* q, QuaterType qtype);
string PutVarDeclarQuater(Quaternary* Q);
string PutConstDeclarQuater(Quaternary* Q);
string PutFuncDeclarQuater(Quaternary* Q);
string PutFuncParaDeclarQuater(Quaternary* Q);
string PutFuncParaPushQuater(Quaternary* Q);
string PutFuncCallQuater(Quaternary* Q);
string PutAssignRetQuater(Quaternary* Q);
string PutFuncRetQuater(Quaternary* Q);
string PutCalQuater(Quaternary* Q);
string PutAssignQuater(Quaternary* Q);
string PutAssignArrayQuater(Quaternary* Q);
string PutCmpQuater(Quaternary* Q);
string PutGotoQuater(Quaternary* Q);
string PutBnzQuater(Quaternary* Q);
string PutBzQuater(Quaternary* Q);
string PutSetLabelQuater(Quaternary* Q);
string PutArrayQueryQuater(Quaternary* Q);
string PutScanQuater(Quaternary* Q);
string PutPrintQuater(Quaternary* Q);

// 组织与优化 工具函数
/* 用于翻转CMP操作的类型，使其满足BZ型四元式的操作 */
QuaterType cmp_reverse(QuaterType cmp_type);
/* 四元式正确性检查：检查参数量 */
void quater_block_check(Quaternary* q);
/* 返回该段中间码的函数集合 */
vector<SymbolItem*> get_funcs(vector<Quaternary*>* list);
/* 指定函数，返回该函数下的所有四元式 */
vector<Quaternary*> get_quaternary_by_function(SymbolItem* func, vector<Quaternary*>* list);
/* 给定四元式，返回其所有的SymbolItem */
map<string, SymbolItem*> get_items_by_quater(Quaternary* q);

#endif // !1

