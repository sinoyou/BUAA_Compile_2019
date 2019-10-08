#pragma once
#include <string>
#include <vector>
#include "Token.h"

using namespace std;

class GrammaticalParser
{
public:
	GrammaticalParser(vector<Token>& token_list,vector<string>& output);
	void parse();						// 解析器暂时没有返回
private:
	int ptoken;							// 遍历token列表的索引
	Token * token;						// 当前索引ptoken下的token
	vector<Token> & token_list;			// token的列表
	
	/* Unit3 输出支持 */
	vector<string> & output_list;		// 输出的列表
	void _save(const char* s);			// 中文字符串保存
	void _save(int head, int tail);		// 标识符字符串保存

	/* 支持函数 */
	void _error(const char* s);			// 错误输出处理
	Token _gettoken_index(int index);	// 根据索引获取一个单词
	void _gettoken();					// 获取一个单词
	void _retract();					// 回退一个单词

	/* 规则函数：递归下降调用 */
	int __add_operator();
	int __mult_operator();
	int __rel_operator();
	int __letter();
	int __number();
	int __non_zero_number();
	int __char();
	int __string();
	int __program();
	int __const_declar();
	int __const_def();
	int __unsigned_integer();
	int __integer();
	int __idenfr();
	int __declar_head();
	int __var_declar();
	int __var_def();
	int __function_return();
	int __function_void();
	int __compound_statement();
	int __parameter_list();
	int __main_function();
	int __expression();
	int __item();
	int __factor();
	int __statement();
	int __assign_statment();
	int __condition_statement();
	int __condition();
	int __loop_statement();
	int __step_length();
	int __function_call_return();
	int __function_call_void();
	int __value_parameter_list();
	int __statement_list();
	int __read_statement();
	int __write_statement();
	int __return_statement();
};

