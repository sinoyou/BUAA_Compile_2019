#include "SymbolTable.h"

/*
 * 初始化一个符号表，并置位一个全局的global块。
 **/
SymbolTable::SymbolTable() {
	root = NULL;
	present = NULL;

	add_one_block("__program_global__");
	root = present;
}

/*
 * 插入一个块, 并将present指针置位。 
 **/
void SymbolTable::add_one_block(string name) {
	Block* temp = new Block();
	temp->func_head.name = name;
	temp->pre = present;
	present = temp;
}

/*
 * 离开当前块，返回上一层。
 **/
void SymbolTable::exit_present_block() {
	present = present->pre;
}

// 返回当前块的引用
Block& SymbolTable::get_present_block() {
	return (*present);
}

