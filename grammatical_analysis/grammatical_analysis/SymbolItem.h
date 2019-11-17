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
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType return_type, vector<SymbolItem*> paramsList);
	// create as const
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int const_value);
	// create as var
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, bool is_array);
	// create as label
	SymbolItem(Block* block, string name, SymbolItemType type);
	// create as temp-var
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type);


	string name;
	SymbolItemType type;
	Block* block;

	// ����ͷ - speical
	BasicType return_type;
	vector<SymbolItem*> paramsList;

	// ���� and ���� and ��ʱ���� - public
	BasicType var_type;
	// ���� - special
	int const_value;
	// ���� - special
	bool isArray;

	// ��ǩ - special
	// None

	// ��ʱ���� - special
	// None

};

#endif // !__SYMBOL_ITEM_H__


