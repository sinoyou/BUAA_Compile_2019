#include "SeekOptimizer.h"

void SeekOptimizer::preprocess() {

}

void SeekOptimizer::optimizer() {
	for (auto it = middle_code.begin(); it != middle_code.end(); it++) {
		if ((*it)->type == QuaterType::Assign) {
			auto result = (*it)->Result;
			auto quater = *it;
			auto basic_block = quater_basic_block[quater];
			// ����������ֵ��������Ծ��������
			if (basic_block->active_out.find(result) == basic_block->active_out.end()) {
				auto jt = it + 1;
				while (jt != middle_code.end()) {
					if (quater_basic_block[*jt] != basic_block)				// ������
						break;
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
}