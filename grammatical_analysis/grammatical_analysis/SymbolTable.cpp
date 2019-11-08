#include "SymbolTable.h"

/*
 * ��ʼ��һ�����ű�����λһ��ȫ�ֵ�global�顣
 **/
SymbolTable::SymbolTable() {
	root = NULL;
	present = NULL;

	add_one_block("__program_global__");
	root = present;
}

/*
 * ����һ����, ����presentָ����λ�� 
 **/
void SymbolTable::add_one_block(string name) {
	Block* temp = new Block();
	temp->func_head.name = name;
	temp->pre = present;
	present = temp;
}

/*
 * �뿪��ǰ�飬������һ�㡣
 **/
void SymbolTable::exit_present_block() {
	present = present->pre;
}

// ���ص�ǰ�������
Block& SymbolTable::get_present_block() {
	return (*present);
}

