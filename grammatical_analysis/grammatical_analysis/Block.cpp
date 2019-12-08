#include "Block.h"

Block::Block() {
	pre = NULL;
	nexts = vector<Block*>();
	func_head = NULL;
}

void Block::register_statement(StatementType type) {
	if (statement_count.find(type) == statement_count.end())
		statement_count[type] = 1;
	else
		statement_count[type] += 1;
}