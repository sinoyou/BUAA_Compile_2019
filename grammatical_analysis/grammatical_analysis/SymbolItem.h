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
	_void
};

/* 符号表项的统一结构 */
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

	// 函数头 - speical
	BasicType return_type;
	vector<SymbolItem*> paramsList;

	// 常量 and 变量 and 临时变量 - public
	BasicType var_type;
	// 常量 - special
	int const_value;
	// 变量 - special
	bool isArray;

	// 标签 - special
	// None

	// 临时变量 - special
	// None

};

#endif // !__SYMBOL_ITEM_H__


