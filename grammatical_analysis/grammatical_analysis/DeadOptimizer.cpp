#include "DeadOptimizer.h"
#include "Block.h"
#include <map>


void DeadOptimizer::preprocess() {

}

void DeadOptimizer::optimizer() {
	map<SymbolItem*, int> usemap_in_block;
	BasicBlock* basic_block = NULL;
	list<Quaternary*> list;
	auto it = middle_code.end();
	while(it > middle_code.begin()) {
		it--;
		auto  result = (*it)->Result;
		auto t = quater_basic_block[*it];
		auto qtype = (*it)->type;
		// ����
		if (t != basic_block) {
			usemap_in_block.clear();
		}
		basic_block = t;

		// ��������� && ����Ϊ���� && Ϊ�����ֲ�����
		if (result != NULL && 
			(result->type == SymbolItemType::_variable || result ->type == SymbolItemType::temp_normal) && 
			(qtype == Add || qtype == Sub || qtype == Mult || qtype == Div || qtype == Assign ) && 
			result->block->pre != NULL) {
			// ���ӿ�β���÷���û�б�ʹ�ù� & �÷��Ų��ǻ�Ծ����
			if (usemap_in_block.find(result) == usemap_in_block.end() &&
				basic_block->active_out.find(result) == basic_block->active_out.end()) {
				// ����Ԫʽ�������룬ɾ��
				vector<Quaternary*> temp;
				temp.push_back(*it);
				string info = "[DeadOptimizer] Delete Quater " + PrintQuater(&temp)[0];
				cout << info << endl;
				continue;
			}
			// ���и÷���֮ǰ��ʹ�ü�¼���������
			usemap_in_block.erase(result);
		}
		// ������ʹ�����ע��
		auto OpA = (*it)->OpA;
		auto OpB = (*it)->OpB;
		if (OpA != NULL)
			usemap_in_block[OpA] = 1;
		if (OpB != NULL)
			usemap_in_block[OpB] = 1;
		list.push_front(*it);
	}

	for (auto it = list.begin(); it != list.end(); it++)
		optimized_code.push_back(*it);
}