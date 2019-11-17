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
	Quaternary(QuaterType type, SymbolItem* A, SymbolItem* B, SymbolItem* Result) :
		type(type), OpA(A), OpB(B), Result(Result) {}
protected:
	QuaterType type;
	SymbolItem* OpA;
	SymbolItem* OpB;
	SymbolItem* Result;
};

// �����Ԫʽ������
extern vector<Quaternary*> QuaterList;

// ��Ԫʽ���ɸ�������
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

// ��Ԫʽ�����������
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

