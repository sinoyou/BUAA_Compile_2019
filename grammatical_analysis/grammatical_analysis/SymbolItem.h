#ifndef __SYMBOL_ITEM_H__
#define __SYMBOL_ITEM_H__ 

#include<string>
#include<vector>
using namespace std;

class Block;

// =========== 符号表中的项类型统计 =============
enum SymbolItemType
{
	unknown,
	function,					// 函数头
	_const,						// 显式声明的常量与变量
	_variable,					// 显示声明变量
	temp_normal,				// 中间码中的临时变量(正常版本)
	temp_const,					// 中间码中的临时变量(常量类)
	temp_strcon,				// 中间码中的临时变量(字符串)
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
	
	// 表达式常量传播支持 
	// 强转int + 修改常量值
	void modify_type_as_int();
	void modify_value(int value);
	bool is_const();
	
	// 生成输出字符表示
	string getname(bool simplify = true);
	string get_basictype();

	Block* block;
	string name;
	SymbolItemType type;

	
	BasicType basic_type;				// 常量 and 变量 and 临时变量 and  函数 - public
	vector<SymbolItem*> paramsList;		// 函数头 - speical
	int value;

	int array_size;
	bool isArray;
	string strcon;						// 字符串


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


