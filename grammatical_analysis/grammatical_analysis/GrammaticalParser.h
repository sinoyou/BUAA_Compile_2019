#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <tuple>
#include "Token.h"
#include "debug.h"
#include "ParseException.h"
#include "error.h"
#include "SymbolTable.h"
#include "SymbolFactory.h"

using namespace std;

// 结构体 头和尾：由于各递归下降无法进行统一，故已经弃用统一的结构体作为函数的参数和返回值。

// 宏定义
/* 所有带下划线的只能在宏定义中使用 */
#define FLAG_ENTER(name,level)									\
string __name_save__(name);										\
int __level__ = level;											\
{																\
	_backup();													\
	RECUR_PRINT(">>>",__level__, __name_save__);				\
}															

 #define FLAG_FAIL \
{										\
	string s = string("ERROR WHEN PARSING ") + _peek()->token;	\
	DEBUG_PRINT(s.c_str());				\
	_recover();							\
}

#define FLAG_PASS						\
{										\
	_backdown();						\
	RECUR_PRINT("<<<",__level__, __name_save__);	\
	if(is_rule_printed(__name_save__))	\
		_save(__name_save__);			\
}

#define SYMBOL_CHECK(symbol) symbol_check(symbol, __level__);
#define MULTI_SYMBOL_CHECK(symbols, len) multi_symbol_check(symbols, len, __level__);


class GrammaticalParser
{
public:
	GrammaticalParser(vector<Token>& token_list, vector<string>& output, vector<tuple<int, string>>& error_output);
	void parse();						// 解析器暂时没有返回
private:
	// Current 当前系列
	int ptoken;							// 遍历token列表的索引
	Token * token;						// 当前索引ptoken下的token
	SymbolTable symbol_table;			// 符号表

	vector<Token> & token_list;			// token的列表
	vector<int> ptoken_record;			// ptoken的备份空间

	/* 文法解析辅助工具 */
	// 单凭文法无法解析两者的差异
	// vector<string> func_call_return_idenfr;
	// vector<string> func_call_void_idenfr;
	
	/* Unit3 输出支持 */
	vector<string> & output_list;		// 输出的列表
	void _save(string t);				// 中文字符串保存
	void _save(Token* token);			// 标识符字符串保存

	/* Unit4 输出支持 */
	vector<tuple<int, string>>& error_output_list;		// error的列表
	void _register_error(int, ErrorType);				// 向error列表中注册error

	/* 支持函数 */
	void _error(const char* s);			// 错误输出处理
	Token _export_token(int index);		// 根据索引导出一个单词
	void _update_token();				// 更新单词
	Token* _peek();						// 偷窥单词（不更改ptoken和token）
	Token* _peek(int x);				// 偷窥单词 (指定后面的第几个)
	void _next();						// 获取一个单词

	void _backup();						// 压入一个指针
	void _backdown();					// 释放一个指针
	void _recover();					// 恢复一个指针备份

	void symbol_check(SYMBOL symbol,int level)	;							// 检查一个终结符
	void multi_symbol_check(SYMBOL symbols[],int len, int level) ; 			// 检查多个终结符

	/* 规则函数：递归下降调用 */
	void __add_operator(int level);
	void __mult_operator(int level);
	void __rel_operator(int level);
	void __letter(int level);
	void __number(int level);
	void __non_zero_number(int level);
	char __char(int level);
	void __string(int level);
	void __program(int level);
	void __const_description(int level);
	void __const_def(int level);
	int __unsigned_integer(int level);
	int __integer(int level);
	string __idenfr(int level, bool is_def);
	void __declar_head(int level, BasicType * return_type, string * func_name);
	void __var_description(int level);
	void __var_def(int level);
	BasicType __type_idenfr(int level);
	void __function_return(int level);
	void __function_void(int level);
	void __compound_statement(int level,bool* has_return);
	void __parameter_list(int level, vector<SymbolItem*>* para_list);
	void __main_function(int level);
	void __expression(int level, bool * is_char);
	void __item(int level, bool *is_char);
	void __factor(int level, bool *is_char);
	void __statement(int level, bool* has_return);
	void __assign_statment(int level);
	void __condition_statement(int level, bool *has_return);
	void __condition(int level);
	void __loop_statement(int level, bool *has_return);
	void __step_length(int level);
	void __function_call_return(int level);
	void __function_call_void(int level);
	void __value_parameter_list(int level, vector<SymbolItem*>* params);
	void __statement_list(int level, bool* has_return);
	void __read_statement(int level);
	void __write_statement(int level);
	void __return_statement(int level, bool* has_return);
};