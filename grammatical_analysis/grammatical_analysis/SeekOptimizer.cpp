#include "SeekOptimizer.h"
#include "Block.h"

void SeekOptimizer::preprocess() {

}

// ���㸳ֵ�������ܹ����������䷶Χ������������ֱ�ӷ�����㡣
vector<Quaternary*>::iterator copy_zone(vector<Quaternary*>::iterator start,
	vector<Quaternary*>::iterator end,
	map<Quaternary*, BasicBlock*> quater_block, 
	bool *check) {
	auto assigned = (*start)->Result;
	auto assigner = (*start)->OpA;
	auto it_assigned = start + 1;
	// ����assigned�������봫������
	while (it_assigned != end) {
		// �����
		if (quater_block[*it_assigned] != quater_block[*start])
			break;
		// ������ֵ��
		if ((*it_assigned)->Result == assigned)
			break;
		it_assigned += 1;
	}
	// ����assigner��������Ч���䣨������һ�α���ֵ��
	auto it_assigner = start + 1;
	while (it_assigner != end) {
		// �����
		if (quater_block[*it_assigner] != quater_block[*start])
			break;
		// ������ֵ��
		if ((*it_assigner)->Result == assigner)
			break;
		it_assigner += 1;
	}
	
	// �жϣ�assigned�����봫�����������assigner����֮�ڣ������ܽ��и�ֵ����
	if (it_assigned <= it_assigner) {
		*check = true;
		return it_assigned;
	}
	else {
		*check = false;
		return start + 1;
	}
}

void SeekOptimizer::optimizer() {
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->type == QuaterType::Assign) {
			auto result = (*it)->Result;
			auto quater = *it;
			auto basic_block = quater_basic_block[quater];
			// ����������ֵ��������Ծ�������� & �ֲ�
			if (basic_block->active_out.find(result) == basic_block->active_out.end()
				&& result->block->pre != NULL) {
				auto jt = it + 1;
				bool check = false;
				auto zone = copy_zone(it, middle_code.end(), quater_basic_block, &check);
				// copy_zone������Ч�����䣬�����Ч�Ͳ�ɾ����
				if (check) {
					while (jt != zone) {
						if (quater_basic_block[*jt] != basic_block)				// ������
							DEBUG_PRINT("[ERROR] Block should not be different here.");
						if ((*jt)->OpA == result)
							(*jt)->OpA = quater->OpA;
						if ((*jt)->OpB == result)
							(*jt)->OpB = quater->OpA;
						if ((*jt)->Result == result)							// �����µĸ�ֵ, ���ڸ�ֵ�Ժ���Ϊ�¸�ֵ������������ԭ����
							break;
						jt++;
					}
					vector<Quaternary*> temp;
					temp.push_back(*it);
					cout << "[Seek Optimizer] Delete Assign " + PrintQuater(&temp)[0] << endl;
				}
				else {
					optimized_code.push_back(*it);
				}
			}
			else {
				optimized_code.push_back(*it);
			}
		}
		else {
			optimized_code.push_back(*it);
		}
	}
}