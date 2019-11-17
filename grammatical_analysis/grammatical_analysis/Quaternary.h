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
	FuncParaPush,	//   ������������						v/c/t				None			None
	FuncCall,		//   ��������						f					None			None
	AssignRet,		//	 ����ֵ��ֵ						None				None			v/t
	FuncRet,		//   ��������						(v/t)				None			None
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
	SetLabel		//   ���ñ�ǩ						l					None			None
};

// ��Ԫʽ��
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

// �����Ԫʽ������
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

