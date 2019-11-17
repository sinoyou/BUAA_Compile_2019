#include "SymbolItem.h"

// function item
SymbolItem::SymbolItem(Block* block,string name, SymbolItemType type, BasicType return_type, vector<SymbolItem*> paramsList) {
	this->block = block;
	this->name = name;
	this->type = type;
	this->return_type = return_type;
	this->paramsList = paramsList;
}

// const (array) item
SymbolItem::SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, int const_value) {
	this->block = block;
	this->name = name;
	this->type = type;
	this->var_type = var_type;
	this->const_value = const_value;
}

// var (array) item
SymbolItem::SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type, bool is_array) {
	this->block = block;
	this->name = name;
	this->type = type;
	this->var_type = var_type;
	this->isArray = is_array;
}

// create as label
SymbolItem::SymbolItem(Block* block, string name, SymbolItemType type) {
	this->block = block;
	this->name = name;
	this->type = type;
}
// create as temp-var
SymbolItem::SymbolItem(Block* block, string name, SymbolItemType type, BasicType var_type) {
	this->block = block;
	this->name = name;
	this->type = type;
	this->var_type = var_type;
}