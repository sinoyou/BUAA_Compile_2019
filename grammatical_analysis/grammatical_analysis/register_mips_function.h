#ifndef __REG_MIPS_FUNCTION_H__
#define __REG_MIPS_FUNCTION_H__

#include<vector>
#include "SymbolItem.h"
#include<map>
#include "Quaternary.h"
#include "register_mips_code_utils.h"
#include "BasicBlock.h"
#include "RegisterPool.h"

class RegisterPool;
class RegMipsFunction
{
private:
	vector<string> sub_object_code;
	SymbolItem* func_head;
	vector<Quaternary*> quater_list;
	map<SymbolItem*, int> offset_map;
	map<SymbolItem*, string>* global_data;
	map<Quaternary*, BasicBlock*> quater_basicblock;
	RegisterPool* register_pool;

	// 隐式定义参数的保存
	int ret_offset;
	int ra_offset;

public:
	// MipsFunction 初始化函数，将建立函数中显式定义量和隐式定义量相对于$sp指针的偏移量
	RegMipsFunction(SymbolItem* func_head, 
		vector<Quaternary*> func_quater, 
		map<SymbolItem*, string>* global_data,
		map<Quaternary*, BasicBlock*> quater_basicblock);
	// 获取函数头
	SymbolItem* get_funchead() { return func_head; };

	// 获取所需要的栈空间大小 - 显式定义 + 隐式定义
	int stack_size();
	// 判断域位置，并返回对应的地址或地址标签
	void get_addr(SymbolItem* item, bool* local, int* offset, string* data_label);
	// 导出此函数部分的目标代码
	vector<string> dump();
};

#endif // !__MIPS_FUNCTION_H__


