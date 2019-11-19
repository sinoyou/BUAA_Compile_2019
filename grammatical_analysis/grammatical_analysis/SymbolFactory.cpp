#include "SymbolFactory.h"

// utils
string alloc_free_temp_name(Block* block) {
	// ȷ��temp���ֵ�Ψһ��
	int cnt = 0;
	char buf[10];
	string ori = "t";
	string target = ori;
	while (block->temp_map.find(target) != block->temp_map.end()) {
		cnt++;
		_itoa_s(cnt, buf, 10);
		target = ori + string(buf);
	}
	block->temp_map[target] = 1;				// �ҵ�Ψһ�����֣�ռλ
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
	int cnt = 0;
	string target = comment;
	char buf[10];
	while (block->label_map.find(target) != block->label_map.end()) {
		cnt++;
		_itoa_s(cnt, buf, 10);
		target = comment + string(buf);
	}
	block->label_map[target] = 1;				// �ҵ�Ψһ��ǩ��ռλ

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
	SymbolItem* p = new SymbolItem(block, target + "@" + block_name, SymbolItemType::temp_normal, var_type);
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
	SymbolItem* p = new SymbolItem(block, block_name + "_" + target, SymbolItemType::temp_const, var_type, value);
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
