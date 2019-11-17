#ifndef __SYMBOL_ITEM_H__
#define __SYMBOL_ITEM_H__ 

#include<string>
#include<vector>
class Block;
using namespace std;

// =========== 符号表中的项类型统计 =============
enum SymbolItemType
{
	unknown,
	function,					// 函数头
	_const,						// 显式声明的常量与变量
	_variable,					// 显示声明变量
	temp,						// 中间码中的临时变量
	label						// 跳转标签
};

enum BasicType {
	_char,
	_int,
	_void,
	_string
};

/* 符号表项的统一结构 */
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

	// 函数头 - speical
	BasicType return_type;
	vector<SymbolItem*> paramsList;

	// 常量 and 变量 and 临时变量 - public
	BasicType var_type;
	// 常量 - special
	int const_value;
	// 变量 - special
	bool isArray;
	int array_size;

	// 标签 - special
	// None

	// 临时变量 - special
	bool temp_const = false;
	string temp_strcon;
	int temp_const_value;

};

#endif // !__SYMBOL_ITEM_H__


