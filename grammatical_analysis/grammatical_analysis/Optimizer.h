#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include<vector>
#include "Quaternary.h"
#include "debug.h"

// 优化器-抽象函数
class Optimizer
{
public:
	// 构造函数
	Optimizer(string name, vector<Quaternary*> middle) :optimizer_name(name), middle_code(middle) {}

	// 导出函数
	vector<Quaternary*> get_optimized_quaters();
	void dumps();

protected:
	string optimizer_name;						// 优化器名称
	vector<Quaternary*> middle_code;			// 输入中间码
	vector<Quaternary*> optimized_code;			// 输出中间码
};

#endif // !__OPTIMIZER_H__


