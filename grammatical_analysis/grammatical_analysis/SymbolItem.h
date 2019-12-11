#ifndef __SYMBOL_ITEM_H__
#define __SYMBOL_ITEM_H__ 

#include<string>
#include<vector>
using namespace std;

class Block;

// =========== ���ű��е�������ͳ�� =============
enum SymbolItemType
{
	unknown,
	function,					// ����ͷ
	_const,						// ��ʽ�����ĳ��������
	_variable,					// ��ʾ��������
	temp_normal,				// �м����е���ʱ����(�����汾)
	temp_const,					// �м����е���ʱ����(������)
	temp_strcon,				// �м����е���ʱ����(�ַ���)
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
		block(block), name(name), type(type), basic_type(return_type), paramsList(paramsList){}
	// create as const, create as temp-const
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int const_value):
		block(block), name(name), type(type), basic_type(var_type), value(const_value){}
	// create as var
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int size, bool is_array) :
		block(block), name(name), type(type), basic_type(var_type), array_size(size), isArray(is_array){}
	// create as label
	SymbolItem(Block* block, string name, SymbolItemType type):
		block(block), name(name), type(type){}
	// create as temp - normal
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type) :
		block(block), name(name), type(type), basic_type(var_type){}
	// create as temp - const
	// SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type,  int value):
	// 	block(block), name(name), type(type), basic_type(var_type), value(value){}
	// create as temp - string
	SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, string strcon):
		block(block), name(name), type(type), basic_type(var_type), strcon(strcon){}
	
	// ���ʽ��������֧�� 
	// ǿתint + �޸ĳ���ֵ
	void modify_type_as_int();
	void modify_value(int value);
	bool is_const();
	
	// ��������ַ���ʾ
	string getname(bool simplify = true);
	string get_basictype();

	Block* block;
	string name;
	SymbolItemType type;

	
	BasicType basic_type;				// ���� and ���� and ��ʱ���� and  ���� - public
	vector<SymbolItem*> paramsList;		// ����ͷ - speical
	int value;

	int array_size;
	bool isArray;
	string strcon;						// �ַ���


	/*				function	  var        const           label           temp-normal           temp-const           temp-string */
	/*
	name			   Y		   Y		   Y			   Y				  Y					  Y						   Y
	type			function	variable     const           label              temp                 temp                     temp
	block			   Y		   Y		   Y			   Y				  Y					  Y						   Y              
	params_list        Y		   X		   X			   X				  X					  X						   X
	basic_type		 v/i/c        i/c		  i/c			   X				 i/c				 i/c					  str
	value			   X		   X		   Y			   X				  X					  Y						   X
	isArray			   X		   Y		   X			   X				  X					  X						   X
	array_size	       X		   Y		   X			   X				  X					  X						   X
	strcon			   X		   X		   X			   X				  X					  X						   Y
	*/
};

#endif // !__SYMBOL_ITEM_H__


