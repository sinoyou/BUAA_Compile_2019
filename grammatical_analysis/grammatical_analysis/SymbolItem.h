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
	_void,
	_string
};

/* ���ű����ͳһ�ṹ */
class SymbolItem {
public:
	// create as function
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType return_type, vector<SymbolItem*> paramsList):
		block(block), name(name), type(type), return_type(return_type), paramsList(paramsList){}
	// create as const
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int const_value):
		block(block), name(name), type(type), var_type(var_type), const_value(const_value){}
	// create as var
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int size, bool is_array) :
		block(block), name(name), type(type), var_type(var_type), array_size(size), isArray(is_array){}
	// create as label
	SymbolItem(Block* block, string name, SymbolItemType type):
		block(block), name(name), type(type){}
	// create as temp-var - normal
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type) :
		block(block), name(name), type(type), var_type(var_type){}
	// create as temp-var - const
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, bool temp_const, int value):
		block(block), name(name), type(type), var_type(var_type), temp_const(temp_const), temp_const_value(value){}
	// create as temp-var - string
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, bool temp_const, string strcon):
		block(block), name(name), type(type), var_type(var_type), temp_const(temp_const), temp_strcon(strcon){}


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
	int array_size;

	// ��ǩ - special
	// None

	// ��ʱ���� - special
	bool temp_const = false;
	string temp_strcon;
	int temp_const_value;

};

#endif // !__SYMBOL_ITEM_H__


