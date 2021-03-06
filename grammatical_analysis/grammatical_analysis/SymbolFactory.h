#ifndef __SYMBOL_FACTORY_H__
#define __SYMBOL_FACTORY_H__

#include "SymbolItem.h"
#include "SymbolTable.h"

/* SymbolItem���ɸ������� */
class SymbolFactory {
public:
	static SymbolItem* create_const(Block* block, string name, BasicType type, int const_value);
	static SymbolItem* create_variable(Block* block, string name, BasicType type, int size, bool isArray);
	static SymbolItem* create_function(Block* block, string name, BasicType return_type, vector<SymbolItem*> paramsList);
	static SymbolItem* create_label(Block* block, string comment);
	static SymbolItem* create_temp(Block* block, BasicType var_type);
	static SymbolItem* create_temp_const(Block* block, BasicType var_type, int value);
	static SymbolItem* create_temp_string(Block* block, string strcon);
	static SymbolItem* create_item_copy(SymbolItem* original);
	static SymbolItem* create_inline_copy(Block* block, SymbolItem* original);
};

#endif // !__SYMBOL_FACTORY_H__
