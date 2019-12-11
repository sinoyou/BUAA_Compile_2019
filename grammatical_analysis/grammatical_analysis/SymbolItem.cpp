#include "SymbolItem.h"
#include "Block.h"
#include <iostream>
#include <cstdlib>
#include "debug.h"

using namespace std;

string SymbolItem::getname(bool simplify) {
	char buf[200];
	// default occassion
	string name = this->name;
	if (!simplify)
		return name;
	// case 1 : 整型常量
	if ((this->type == _const || this->type == temp_const) && this->basic_type == BasicType::_int) {
		name = to_string(this->value);
	}
	// case 2：字符型常量
	else if ((this->type == _const || this->type == temp_const) && this->basic_type == BasicType::_char) {
		if (this->value == '\n') {
			name = "'\\n'";
		}
		else {
			buf[0] = this->value;
			buf[1] = 0;
			name = "'" + string(buf) + "'";
		}
	}
	// case 3: 字符串
	else if (this->type == temp_strcon && this->basic_type == BasicType::_string) {
		name = "\"" + this->strcon + "\"";
	}

	if (this->type == SymbolItemType::label
		|| this->type == SymbolItemType::function
		|| this->type == SymbolItemType::temp_const
		|| this->type == SymbolItemType::temp_strcon)
		return name;
	else
		// temp_normal, variable, const
		return name + "@" + this->block->func_head->name; // 调试 - 查看各变量是否处在自定义的作用域中
		// return name;
}

/*
 * 返回该标识符的类型
*/
string SymbolItem::get_basictype() {
	string type;
	type = (this->basic_type == BasicType::_int) ? "int" :
		(this->basic_type == BasicType::_char) ? "char" :
		(this->basic_type == BasicType::_string) ? "string" :
		(this->basic_type == BasicType::_void) ? "void" : "unknown";
	return type;
}

/*
 * 判断当前SymbolItem是否为常量
*/
bool SymbolItem::is_const() {
	return (this->type == SymbolItemType::_const || this->type == SymbolItemType::temp_const);
}

/*
 * 更新常量类型的SymbolItem的值
*/
void SymbolItem::modify_value(int value) {
	if (this->type == SymbolItemType::temp_const) {
		if (this->basic_type != BasicType::_int) {
			char buf[100];
			sprintf(buf, "# Auto Convert %s From %s To int", this->getname().c_str(), this->get_basictype().c_str());
			DEBUG_PRINT(buf);
			this->basic_type = BasicType::_int;
		}
		this->value = value;
	}
	else {
		char buf[100];
		sprintf(buf, "[ERROR] Can not modify a non-const(temp) SymbolItem %s", this->getname().c_str());
		DEBUG_PRINT(buf);
	}
}

/*
 * 强制改动类型为int型
*/
void SymbolItem::modify_type_as_int() {
	this->basic_type = BasicType::_int;
}