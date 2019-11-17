#ifndef __SYMBOL_ITEM_H__
#define __SYMBOL_ITEM_H__ 

#include<string>
#include<vector>
class Block;
using namespace std;

// =========== ���ű��е�������ͳ�� =============
enum SymbolItemType
{
	unknown,
	function,					// ����ͷ
	_const,						// ��ʽ�����ĳ��������
	_variable,					// ��ʾ��������
	temp,						// �м����е���ʱ����
	label						// ��ת��ǩ
};

enum BasicType {
	_char,
	_int,
	_void
};

/* ���ű����ͳһ�ṹ */
class SymbolItem {
public:
	// create as function
	SymbolItem(string name, SymbolItemType type, BasicType return_type, vector<SymbolItem*> paramsList);
	// create as const
	SymbolItem(string name, SymbolItemType type, BasicType var_type, int const_value);
	// create as var
	SymbolItem(string name, SymbolItemType type, BasicType var_type, bool is_array);


	string name;
	SymbolItemType type;

	// ����ͷ - speical
	Block* block;
	BasicType return_type;
	vector<SymbolItem*> paramsList;

	// ���� and ����- public
	BasicType var_type;
	// ���� - special
	int const_value;
	// ���� - special
	bool isArray;


};

#endif // !__SYMBOL_ITEM_H__


