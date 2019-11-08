
#include "SymbolTable.h"
#include "debug.h"

/*
 * ��鵱ǰ���������ĳ���������Ƿ��Ѿ�����
 * ��鷶Χ����ǰ�鼰��ǰ�̵ģ���������������\���������Ӻ�������
*/
void* find_indefr(Block* block, string name)
{
	while (block != NULL) {
		// �����������Ƿ��ظ�
		if (block->func_head.name == name)
			return &(block->func_head);
		// ���ڱ���/�����Ƿ��ظ�
		vector<ParamRecord>::iterator it = block->records.begin();
		while (it != block->records.end()) {
			if (it->name == name)
				return &(*it);
			it++;
		}
		// ���ڿ����Ƿ��ظ�
		vector<Block*>::iterator it1 = block->nexts.begin();
		while (it1 != block->nexts.end()) {
			if ((*it1)->func_head.name == name)
				return &((*it1)->func_head);
			it1++;
		}
		block = block->pre;
	}
	return NULL;
}

/**
 * ��ǰ���в���һ����¼
*/
void insert_one_record(Block* block, ParamRecord record)
{
	void* p = find_indefr(block, record.name);
	// ��飬�����ظ�����
	if (p == NULL) {
		block->records.push_back(record);
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

/**
 * ���µ�ǰ���еĺ���ͷ
*/
void update_function_head(Block* block, FuncHead head) {
	void* p = find_indefr(block, head.name);
	if (p == NULL) {
		block->func_head = head;
	}
	else {
		DEBUG_PRINT("[ERROR] Duplicated Identifier")
	}
}

