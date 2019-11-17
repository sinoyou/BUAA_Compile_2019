#ifndef __SYMBOL_FACTORY_H__
#define __SYMBOL_FACTORY_H__

#include "SymbolItem.h"
#include "SymbolTable.h"

/* SymbolItem生成辅助工具 */
class SymbolFactory {
public:
	static SymbolItem* create_const(Block* block, string name, BasicType type, int const_value);
	static SymbolItem* create_variable(Block* block, string name, BasicType type, bool is_array);
	static SymbolItem* create_function(Block* block, string name, BasicType return_type, vector<SymbolItem*> paramsList);
	static SymbolItem* create_label(Block* block, string comment);
	static SymbolItem* create_temp(Block* block);
};

#endif // !__SYMBOL_FACTORY_H__
