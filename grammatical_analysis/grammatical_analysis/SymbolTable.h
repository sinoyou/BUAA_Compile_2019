#include <string>
#include <vector>

using namespace std;

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

// =========== ���ű��е�������ͳ�� =============
/*
 1. int|char�ͱ��������顣����ParamRecord����
 2. FuncHead����ͷ����ÿ���������Ӧ�š�
 */

struct ParamRecord {
	string name;					
	string type;					// int, char
	bool isConst;					// true / false
	bool isArray;					// true / false
};

struct FuncHead {
	string name;
	string returnType;				// int, char, void
	vector<string> paramsList;		
};

/* ���ű�ṹ���Ľ�㣬����ǰ׺ָ�롣 */
class Block {
public:
	// ���νṹ֧��
	Block* pre;	
	vector<Block*> nexts;

	// ���Ա�ǩ
	FuncHead func_head;
	vector<ParamRecord> records;
};

class SymbolTable
{
public:
	// members
	Block* root;
	Block* present;

	// methods
	SymbolTable();									// build and initial

	void add_one_block();							// ���ڵ�ǰ�ڵ�㼶������һ����
	void exit_present_block();						// �뿪��ǰ�ڵ�㼶��������һ����
	Block* get_present_block();						// ���ص�ǰ�ڵ�ṹ������ã�for - ��ѯ�����
};


// ========== ���ű�ĸ������� ==========
void* find_indefr(Block* block, string name);
void insert_one_record(Block* block, ParamRecord record);
void update_function_head(Block* block, FuncHead head);
void* find_indefr_current(Block* block, string name);



#endif // !__SYMBOL_TABLE_H__

