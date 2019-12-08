#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include<vector>
#include "Quaternary.h"
#include "debug.h"

// �Ż���-������
class Optimizer
{
public:
	// ���캯��
	Optimizer(string name, vector<Quaternary*> middle) :optimizer_name(name), middle_code(middle) {}

	// ��������
	vector<Quaternary*> get_optimized_quaters();
	void dumps();

protected:
	string optimizer_name;						// �Ż�������
	vector<Quaternary*> middle_code;			// �����м���
	vector<Quaternary*> optimized_code;			// ����м���
};

#endif // !__OPTIMIZER_H__


