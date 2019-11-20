#include "Block.h"

Block::Block() {
	pre = NULL;
	nexts = vector<Block*>();
	func_head = NULL;
}