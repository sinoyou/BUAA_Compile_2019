#include "SymbolFactory.h"
#include "debug.h"
#include <iostream>
#include <cstdlib>

// utils
string alloc_free_temp_name(Block* block, string prefix="t") {
	// ȷ��temp���ֵ�Ψһ��
	int cnt = 0;
	string ori = prefix;
	string target = ori;
	while (block->temp_map.find(target) != block->temp_map.end() || find_indefr(block, target) != NULL) {
		cnt++;
		target = ori + to_string(cnt);
	}
	block->temp_map[target] = 1;				// �ҵ�Ψһ�����֣�ռλ
	return target;
}

string alloc_free_label_name(Block* block, string comment) {
	string target = comment;
	int cnt = 0;
	while (block->label_map.find(target) != block->label_map.end()) {
		cnt++;
		target = comment + to_string(cnt);
	}
	block->label_map[target] = 1;				// �ҵ�Ψһ��ǩ��ռλ
	return target;
}


/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_const(Block* block, string name, BasicType type, int const_value) {
	SymbolItem* p = new SymbolItem(block, name, SymbolItemType::_const, type, const_value);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_variable(Block* block, string name, BasicType type, int size, bool isArray) {
	SymbolItem* p = new SymbolItem(block, name, SymbolItemType::_variable, type, size, isArray);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem�������ͣ�
*/
SymbolItem* SymbolFactory::create_function(Block* block, string name, BasicType return_type, vector<SymbolItem*> paramsList) {
	SymbolItem* p = new SymbolItem(block, name, SymbolItemType::function, return_type, paramsList);
	update_function_head(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem����ǩ�ͣ�
*/
SymbolItem* SymbolFactory::create_label(Block* block, string comment) {
	string block_name = block->func_head->name;
	// ȷ��label��Ψһ��
	string target = alloc_free_label_name(block, comment);

	// ���������label
	string full_name = block_name + "_" + target;
	SymbolItem* p = new SymbolItem(block, full_name, SymbolItemType::label);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem����ʱ������
*/
SymbolItem* SymbolFactory::create_temp(Block* block, BasicType var_type) {
	string block_name = block->func_head->name;
	string target = alloc_free_temp_name(block);

	// ������ʱ����
	SymbolItem* p = new SymbolItem(block, target, SymbolItemType::temp_normal, var_type);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem����ʱ�����������������ʱ�����Ǹ��̶�ֵ
*/
SymbolItem* SymbolFactory::create_temp_const(Block* block, BasicType var_type, int value) {
	string block_name = block->func_head->name;
	string target = alloc_free_temp_name(block);
	// ������ʱ����
	SymbolItem* p = new SymbolItem(block, target + "#const", SymbolItemType::temp_const, var_type, value);
	insert_one_record(block, p);
	return p;
}

/*
 * ����������ű��е�ָ�������һ��SymbolItem����ʱ�����������������ʱ�����Ǹ��ַ�����
*/
SymbolItem* SymbolFactory::create_temp_string(Block* block, string strcon) {
	string block_name = block->func_head->name;
	string target = alloc_free_temp_name(block);
	// ������ʱ����
	SymbolItem* p = new SymbolItem(block, block_name + "_" + target, SymbolItemType::temp_strcon, BasicType::_string, strcon);
	insert_one_record(block, p);
	return p;
}

/*
 * ��������SymbolItem, �������֣�������ȫһ�¡�
*/
SymbolItem* SymbolFactory::create_item_copy(SymbolItem* original) {
	SymbolItem* p = NULL;
	string new_name;
	if (original->block->pre == NULL) {
		char buf[100];
		sprintf(buf, "[WARNING] Creating a copy of GLOBAL variables %s.", original->getname().c_str());
		DEBUG_PRINT(buf);
	}
	switch (original->type)
	{
	case SymbolItemType::function:
		return original;
		break;
	case SymbolItemType::label:
		p = SymbolFactory::create_label(original->block, original->name);
		break;
	case SymbolItemType::temp_const:
		p = SymbolFactory::create_temp_const(original->block, original->basic_type, original->value);
		break;
	case SymbolItemType::temp_normal:
		p = SymbolFactory::create_temp(original->block, original->basic_type);
		break;
	case SymbolItemType::temp_strcon:
		p = SymbolFactory::create_temp_string(original->block, original->strcon);
		break;
	case SymbolItemType::_const:
		new_name = alloc_free_temp_name(original->block, original->name);
		p = SymbolFactory::create_const(original->block, new_name, original->basic_type, original->value);
		break;
	case SymbolItemType::_variable:
		new_name = alloc_free_temp_name(original->block, original->name);
		p = SymbolFactory::create_variable(original->block, new_name, original->basic_type, original->array_size, original->isArray);
		// p = SymbolFactory::create_temp(original->block, original->basic_type);
		break;
	default:
		DEBUG_PRINT("[ERROR] Unknown SymbolItem Type Here");
		break;
	}
	p->tags = original->tags;
	p->name = "#" + p->name;
	return p;
}

/*
 * ��������Inline����Ƕ��ĸ���SymbolItem
*/
SymbolItem* SymbolFactory::create_inline_copy(Block* block, SymbolItem* original) {
	SymbolItem* p = NULL;
	string new_name;
	if (original->block->pre == NULL) {
		char buf[100];
		sprintf(buf, "[WARNING] Creating a copy of GLOBAL variables %s.", original->getname().c_str());
		DEBUG_PRINT(buf);
	}
	switch (original->type)
	{
		case SymbolItemType::function:
			return original;
			break;
		case SymbolItemType::label:
			p = SymbolFactory::create_label(block, original->name);
			break;
		case SymbolItemType::temp_const:
			p = SymbolFactory::create_temp_const(block, original->basic_type, original->value);
			break;
		case SymbolItemType::temp_normal:
			p = SymbolFactory::create_temp(block, original->basic_type);
			break;
		case SymbolItemType::temp_strcon:
			p = SymbolFactory::create_temp_string(block, original->strcon);
			break;
		case SymbolItemType::_const:
			new_name = alloc_free_temp_name(block, original->name);
			p = SymbolFactory::create_const(block, new_name, original->basic_type, original->value);
			break;
		case SymbolItemType::_variable:
			new_name = alloc_free_temp_name(block, original->name);
			p = SymbolFactory::create_variable(block, new_name, original->basic_type, original->array_size, original->isArray);
			break;
	default:
		DEBUG_PRINT("[ERROR] Unknown SymbolItem Type Here");
		break;
	}
	p->name = "$" + p->name;
	return p;
}
