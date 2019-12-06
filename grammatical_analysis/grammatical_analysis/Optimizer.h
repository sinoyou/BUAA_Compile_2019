#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include<vector>
#include "Quaternary.h"
#include "debug.h"


class Optimizer
{
public:
	vector<Quaternary*> middle_code;			// �����м���
	string optimizer_name;						// �Ż�������
	vector<Quaternary*> optimized_code;			// ����м���

	// ���캯��
	Optimizer(string name, vector<Quaternary*> middle) :optimizer_name(name), middle_code(middle) {}

	// ��������
	vector<Quaternary*> get_optimized_quaters();
	void dumps();

private:
	// Ԥ������
	void preprocess();
	// �Ż�����
	void optimizer();
};

#endif // !__OPTIMIZER_H__


