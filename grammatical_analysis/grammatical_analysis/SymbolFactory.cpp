#include "SymbolFactory.h"

/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_const(Block* block, string name, BasicType type, int const_value) {
	SymbolItem* p = new SymbolItem(name, SymbolItemType::_const, type, const_value);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_variable(Block* block, string name, BasicType type, bool is_array) {
	SymbolItem* p = new SymbolItem(name, SymbolItemType::_variable, type, is_array);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_function(Block* block, string name, BasicType return_type, vector<SymbolItem*> paramsList) {
	SymbolItem* p = new SymbolItem(name, SymbolItemType::function, return_type, paramsList);
	insert_one_record(block, p);
	return p;
}

SymbolItem* SymbolFactory::create_label(Block* block, string comment) {

}
SymbolItem* SymbolFactory::create_temp(Block* block) {

}