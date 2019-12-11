#include "BasicBlock.h"
#include "debug.h"
#include "Block.h"
#include <iostream>

static int basic_block_id_cnt = 0;

BasicBlock::BasicBlock(vector<Quaternary*> quaters) {
	basic_block_id_cnt++;
	this->id = basic_block_id_cnt;
	this->quaters = quaters;
}


/* ��ԭʼ�м������ɻ�����
 * 1. ���ֲ����ɻ����顣
 * 2. ��ÿ���м��������<�м��룬������>��ӳ�䡣
 * 3. Ѱ�ҽ���������֮�����ͼ��
 */
void build_connection(BasicBlock* from, BasicBlock* to);
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle);
Quaternary* find_func_declar(SymbolItem* func, vector<Quaternary*> middle);
Quaternary* find_set_label(SymbolItem* label, vector<Quaternary*> middle);
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle);
vector<Quaternary*> find_func_call(SymbolItem* func, vector<Quaternary*> middle);

vector<BasicBlock*> generate_basic_blocks(vector<Quaternary*> middle, map<Quaternary*, BasicBlock*>* map)
{
	vector<BasicBlock*> block_list;

	// step1��Ѱ������м���
	// ����һ����һ���м��룻���������ת���ĺ�һ�䣻������������ת�ı�ǩλ�á�
	vector<int> entry_index;
	entry_index.push_back(0);
	for (auto it = middle.begin(); it != middle.end(); it++) {
		auto type = (*it)->type;
		if (type == FuncCall || type == FuncRet || type == Goto || type == Bz || type == Bnz) {
			if(it != middle.end() - 1)
				entry_index.push_back(it - middle.begin() + 1);
		}
		else if (type == FuncDeclar || (type == SetLabel && is_label_visited((*it)->OpA, middle))) {
			entry_index.push_back(it - middle.begin());
		}
	}
	// step2: �����������<Quater, BasicBlock>ӳ���ϵ
	for (auto it = entry_index.begin(); it != entry_index.end(); it++) {
		int front = *it;
		int tail = (it == entry_index.end() - 1) ? middle.end() - middle.begin() : *(it + 1);
		vector<Quaternary*> temp;
		temp.insert(temp.end(), middle.begin() + front, middle.begin() + tail);
		if (temp.size() > 0) {
			BasicBlock* block = new BasicBlock(temp);
			block_list.push_back(block);
			DEBUG_BASICBLOCK("Block Established [" + to_string(front) + "," + to_string(tail - 1) + "]");
			// ����ӳ��
			for (auto jt = temp.begin(); jt != temp.end(); jt++) {
				if (map->find(*jt) != map->end()) {
					DEBUG_PRINT("[ERROR] Quater Already Exist in other BasicBlock");
				}
				(*map)[*jt] = block;
			}
		}
		else {
			string info = "[WARNING] Empty Block Appear at " + to_string(front);
			DEBUG_PRINT(info.c_str());
		}
	}
	// step3: ����˫���ϵ - ����������ʼ
	for (auto it = block_list.begin(); it != block_list.end(); it++)
	{
		// ��ȡÿ�����е����һ���м��룬�ж�����
		auto last_quater = *((*it)->quaters.end() - 1);
		auto quater_type = last_quater->type;
		if (quater_type == QuaterType::Bz || quater_type == QuaterType::Bnz) {
			// ��ǰ�� -> ��һ���� | ��ǰ�� -> label���ڵĻ�����
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
			build_connection(*it, (*map)[find_set_label(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::Goto) {
			// ��ǰ�� -> label���ڵĻ�����
			build_connection(*it, (*map)[find_set_label(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::FuncCall) {
			// ��ǰ�� -> ��һ����
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
			// Ignore: ��ǰ�� -> funcͷ���ڿ�
			// build_connection(*it, (*map)[find_func_declar(last_quater->OpA, middle)]);
		}
		else if (quater_type == QuaterType::FuncRet) {
			// ��ǰ�� -> next(i) for i in ���е��øú����Ļ�����
			// Ignore - �ֿ��������ں����ڲ�����
			/*
			auto called_quaters = find_func_call(last_quater->block->func_head, middle);
			for (auto jt = called_quaters.begin(); jt != called_quaters.end(); jt++)
			{
				auto t_block = (*map)[*jt];
				auto kt = find(block_list.begin(), block_list.end(), t_block);
				if (kt == block_list.end())
					DEBUG_PRINT("[ERROR] Block should be found here. ");
				if (kt != block_list.end() - 1)
					build_connection(*it, *(kt + 1));
			}
			*/
		}
		else {
			// ��ǰ�� -> ��һ����
			if (it != block_list.end() - 1)
				build_connection(*it, *(it + 1));
		}
	}
	// step4: ��Ծ��������
	active_analysis(block_list);

	return block_list;
}

/* ˫������ϵ */
void build_connection(BasicBlock* from, BasicBlock* to)
{
	from->next.insert(to);
	to->pre.insert(from);
}

/* ��ѯ��label�Ƿ񱻷��ʹ� */
bool is_label_visited(SymbolItem* label, vector<Quaternary*> middle)
{
	for (auto it = middle.begin(); it != middle.end(); it++){
		auto type = (*it)->type;
		if ((type == Bnz || type == Bz || type == Goto) && (*it)->OpA == label)
			return true;
	}
	return false;
}

/* ��ѯĳ������ͷ������λ�� */
Quaternary* find_func_declar(SymbolItem* func, vector<Quaternary*> middle)
{
	Quaternary* save = NULL;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == FuncDeclar && (*it)->Result == func) {
			if (save != NULL)
				DEBUG_PRINT("[ERROR] Find duplicated Func Declar Here.");
			save = *it;
		}
	}
	return save;
}

/* ��ѯĳ��label��SetLabel��Ԫʽ */
Quaternary* find_set_label(SymbolItem* label, vector<Quaternary*> middle)
{
	Quaternary* save = NULL;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == SetLabel && (*it)->OpA == label)
		{
			if (save != NULL)
				DEBUG_PRINT("[ERROR] Find Duplicated Set Label Quater.");
			save = *it;
		}
	}
	return save;
}

/* ��ѯĳ������ͷ�����õ�λ�� */
vector<Quaternary*> find_func_call(SymbolItem* func, vector<Quaternary*> middle)
{
	vector<Quaternary*> func_call_list;
	for (auto it = middle.begin(); it != middle.end(); it++) {
		if ((*it)->type == FuncCall && (*it)->OpA == func)
			func_call_list.push_back(*it);
	}
	return func_call_list;
}