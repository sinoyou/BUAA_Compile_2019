#include "SymbolItem.h"
#include "Block.h"
#include <iostream>
#include <cstdlib>

using namespace std;

string SymbolItem::getname(bool simplify) {
	char buf[200];
	// default occassion
	string name = this->name;
	if (!simplify)
		return name;
	// case 1 : ���ͳ���
	if ((this->type == _const || this->type == temp_const) && this->basic_type == BasicType::_int) {
		name = to_string(this->value);
	}
	// case 2���ַ��ͳ���
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
	// case 3: �ַ���
	else if (this->type == temp_strcon && this->basic_type == BasicType::_string) {
		name = "\"" + this->strcon + "\"";
	}

	if (this->type == SymbolItemType::label
		|| this->type == SymbolItemType::function
		|| this->type == SymbolItemType::temp_const
		|| this->type == SymbolItemType::temp_normal
		|| this->type == SymbolItemType::temp_strcon)
		return name;
	else
		return name + "@" + this->block->func_head->name; // ���� - �鿴�������Ƿ����Զ������������
		// return name;
}

/*
 * ���ظñ�ʶ��������
*/
string SymbolItem::get_basictype() {
	string type;
	type = (this->basic_type == BasicType::_int) ? "int" :
		(this->basic_type == BasicType::_char) ? "char" :
		(this->basic_type == BasicType::_string) ? "string" :
		(this->basic_type == BasicType::_void) ? "void" : "unknown";
	return type;
}