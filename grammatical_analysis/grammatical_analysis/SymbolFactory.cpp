#include "SymbolFactory.h"

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
SymbolItem* SymbolFactory::create_variable(Block* block, string name, BasicType type, bool is_array) {
	SymbolItem* p = new SymbolItem(block, name, SymbolItemType::_variable, type, is_array);
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

	// ������ʱ����
	SymbolItem* p = new SymbolItem(block, target, SymbolItemType::temp, var_type);
	insert_one_record(block, p);
	return p;
}