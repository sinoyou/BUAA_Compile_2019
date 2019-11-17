#include "SymbolItem.h"

// function item
SymbolItem::SymbolItem(string name, SymbolItemType type, BasicType return_type, vector<SymbolItem*> paramsList) {
	this->name = name;
	this->type = type;
	this->return_type = return_type;
	this->paramsList = paramsList;
}

// const (array) item
SymbolItem::SymbolItem(string name, SymbolItemType type, BasicType var_type, int const_value) {
	this->name = name;
	this->type = type;
	this->var_type = var_type;
	this->const_value = const_value;
}

// var (array) item
SymbolItem::SymbolItem(string name, SymbolItemType type, BasicType var_type, bool is_array) {
	this->name = name;
	this->type = type;
	this->var_type = var_type;
	this->isArray = is_array;
}