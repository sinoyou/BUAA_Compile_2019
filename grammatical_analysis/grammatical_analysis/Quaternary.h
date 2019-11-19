#ifndef __QUATERNARY_H__
#define __QUATERNARY_H__

#include "SymbolItem.h"

/* ����ֵ���⣬�����ΪSymbolItem�࣬f - function, c - const, v - variable, t - temp, l - label */
/* ��Ԫʽ���� ------------- ���� ---------------------- ������A ----------- ������B ------------ ����� */
enum QuaterType
{
	Undefined,
	// ����
	VarDeclar,		//   ��������						None				None			v
	ConstDeclar,	//   ��������						None				None			c		
	// ����
	FuncDeclar,		//	 ��������						None				None			f
	FuncParaDeclar,	//   ������������						None				None			v
	FuncParaPush,	//   ������������						v/c/t				None			None
	FuncCall,		//   ��������						f					None			None
	AssignRet,		//	 ����ֵ��ֵ						None				None			v/t
	FuncRet,		//   ��������						(v/c/t)				None			None
	// ����
	Add,			//   �����ӷ�						v/c/t				v/c/t			v/t
	Sub,			//   ��������						(v/c/t)				v/c/t			v/t
	Mult,			//   �����˷�						v/c/t				v/c/t			v/t
	Div,			//	 ��������						v/c/t				v/c/t			v/t
	Assign,			//   ��ֵ							v/c/t				None			v/t
	// ����
	EqlCmp,			//	 ��ֵ�Ƚ�						v/c/t				v/c/t			v/t
	NeqCmp,			//	 ���ȱȽ�						v/c/t				v/c/t			v/t
	GtCmp,			//	 ���ڱȽ�						v/c/t				v/c/t			v/t
	GeqCmp,			//	 ���ڵ��ڱȽ�						v/c/t				v/c/t			v/t
	LtCmp,			//	 С�ڱȽ�						v/c/t				v/c/t			v/t
	LeqCmp,			//	 С�ڵ��ڱȽ�						v/c/t				v/c/t			v/t
	// ��֧
	Goto,			//   ��������ת						l					None			None
	Bnz,			//	 ��������ת(~0)					l					v/c/t			None
	Bz,				//	 ��������ת(0)					l					v/c/t			None
	// ��ǩ
	SetLabel,		//   ���ñ�ǩ						l					None			None
	// ����
	ArrayQuery,		//	 ��������ȡֵ						v					v/c/t			v/t			
	AssignArray,	//   ���鸳ֵ						v/c/t				v/c/t(indx)		v
	// IO
	Scan,			//   ����							None				None			v
	Print			//   ���							v/t/c				None			None
};

// ��Ԫʽ��
class Quaternary
{
public:
	Quaternary(Block* block, QuaterType type, SymbolItem* A, SymbolItem* B, SymbolItem* Result) :
		block(block), type(type), OpA(A), OpB(B), Result(Result) {}

	QuaterType type;
	Block* block;
	SymbolItem* OpA;
	SymbolItem* OpB;
	SymbolItem* Result;
};

// �����Ԫʽ������
extern vector<Quaternary*> QuaterList;

// ��Ԫʽ���ɸ�������
Quaternary* GetVarDeclarQuater(Block* block, SymbolItem* result);
Quaternary* GetConstDeclarQuater(Block* block, SymbolItem* result);
Quaternary* GetFuncDeclarQuater(Block* block, SymbolItem* result);
Quaternary* GetFuncParaDeclarQuater(Block* block, SymbolItem* result);
Quaternary* GetFuncParaPushQuater(Block* block, SymbolItem* A);
Quaternary* GetFuncCallQuater(Block* block, SymbolItem* A);
Quaternary* GetAssignRetQuater(Block* block, SymbolItem* result);
Quaternary* GetFuncRetQuater(Block* block, SymbolItem* A);
Quaternary* GetAddQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetSubQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetMultQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetDivQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetAssignQuater(Block* block, SymbolItem* A, SymbolItem* Result);
Quaternary* GetAssignArrayQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetEqlCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetNeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLtCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetLeqCmpQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetGotoQuater(Block* block, SymbolItem* A);
Quaternary* GetBnzQuater(Block* block, SymbolItem* label, SymbolItem* condition);
Quaternary* GetBzQuater(Block* block, SymbolItem* label, SymbolItem* condition);
Quaternary* GetSetLabelQuater(Block* block, SymbolItem* A);
Quaternary* GetArrayQueryQuater(Block* block, SymbolItem* A, SymbolItem* B, SymbolItem* Result);
Quaternary* GetScanQuater(Block* block, SymbolItem* Result);
Quaternary* GetPrintQuater(Block* block, SymbolItem* A);

// ��Ԫʽ�����������
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

#endif // !1

