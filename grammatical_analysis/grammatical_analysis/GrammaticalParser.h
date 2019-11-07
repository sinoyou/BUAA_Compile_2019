#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <tuple>
#include "Token.h"
#include "debug.h"
#include "ParseException.h"

using namespace std;

// 枚举变量
enum ErrorType {
	LexError,
	Redefine,
	Undefine,
	FuncParamNumUnmatch,
	FuncParamTypeUnmatch,
	ConditionError,
	VoidWithReturn,
	ReturnError,
	ArrayIndexWrong,
	ConstModify,
	MissSemicn,
	MissRightParent,
	MissRightBrack,
	MissWhile,
	ConstDefWrong
};

// 结构体 头和尾
struct PARSE_HEAD {
	int level;
};
struct PARSE_RETURN {
	int state;
	int a;
	int b;
};

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
	string s = string("ERROR WHEN PARSING ") + token->token;	\
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

#define RECUR_CHECK(func) func(PARSE_HEAD{head.level + 1});
#define SYMBOL_CHECK(symbol) symbol_check(symbol, __level__);
#define MULTI_SYMBOL_CHECK(symbols, len) multi_symbol_check(symbols, len, __level__);


class GrammaticalParser
{
public:
	GrammaticalParser(vector<Token>& token_list,vector<string>& output, vector<tuple<int, string>>& error_output);
	void parse();						// 解析器暂时没有返回
private:
	// Current 当前系列
	int ptoken;							// 遍历token列表的索引
	Token * token;						// 当前索引ptoken下的token

	vector<Token> & token_list;			// token的列表
	vector<int> ptoken_record;			// ptoken的备份空间

	/* 文法解析辅助工具 */
	// 单凭文法无法解析两者的差异
	vector<string> func_call_return_idenfr;
	vector<string> func_call_void_idenfr;
	
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
	PARSE_RETURN __add_operator(PARSE_HEAD head);
	PARSE_RETURN __mult_operator(PARSE_HEAD head);
	PARSE_RETURN __rel_operator(PARSE_HEAD head);
	PARSE_RETURN __letter(PARSE_HEAD head);
	PARSE_RETURN __number(PARSE_HEAD head);
	PARSE_RETURN __non_zero_number(PARSE_HEAD head);
	PARSE_RETURN __char(PARSE_HEAD head);
	PARSE_RETURN __string(PARSE_HEAD head);
	PARSE_RETURN __program(PARSE_HEAD head);
	PARSE_RETURN __const_description(PARSE_HEAD head);
	PARSE_RETURN __const_def(PARSE_HEAD head);
	PARSE_RETURN __unsigned_integer(PARSE_HEAD head);
	PARSE_RETURN __integer(PARSE_HEAD head);
	PARSE_RETURN __idenfr(PARSE_HEAD head);
	PARSE_RETURN __declar_head(PARSE_HEAD head);
	PARSE_RETURN __var_description(PARSE_HEAD head);
	PARSE_RETURN __var_def(PARSE_HEAD head);
	PARSE_RETURN __type_idenfr(PARSE_HEAD head);
	PARSE_RETURN __function_return(PARSE_HEAD head);
	PARSE_RETURN __function_void(PARSE_HEAD head);
	PARSE_RETURN __compound_statement(PARSE_HEAD head);
	PARSE_RETURN __parameter_list(PARSE_HEAD head);
	PARSE_RETURN __main_function(PARSE_HEAD head);
	PARSE_RETURN __expression(PARSE_HEAD head);
	PARSE_RETURN __item(PARSE_HEAD head);
	PARSE_RETURN __factor(PARSE_HEAD head);
	PARSE_RETURN __statement(PARSE_HEAD head);
	PARSE_RETURN __assign_statment(PARSE_HEAD head);
	PARSE_RETURN __condition_statement(PARSE_HEAD head);
	PARSE_RETURN __condition(PARSE_HEAD head);
	PARSE_RETURN __loop_statement(PARSE_HEAD head);
	PARSE_RETURN __step_length(PARSE_HEAD head);
	PARSE_RETURN __function_call_return(PARSE_HEAD head);
	PARSE_RETURN __function_call_void(PARSE_HEAD head);
	PARSE_RETURN __value_parameter_list(PARSE_HEAD head);
	PARSE_RETURN __statement_list(PARSE_HEAD head);
	PARSE_RETURN __read_statement(PARSE_HEAD head);
	PARSE_RETURN __write_statement(PARSE_HEAD head);
	PARSE_RETURN __return_statement(PARSE_HEAD head);
};