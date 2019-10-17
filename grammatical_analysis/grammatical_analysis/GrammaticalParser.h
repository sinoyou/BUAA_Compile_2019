#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "Token.h"
#include "debug.h"

using namespace std;

// macro 宏定义

#define FLAG_FUNC_HEAD int __level__
	
#define FLAG_ENTER(name)										\
string __name_save__(name);										\
{																\
	_backup();													\
	RECUR_PRINT(">>>",__level__, __name_save__);				\
}															

#define FLAG_FAIL \
{										\
	string s = string("ERROR WHEN PARSING ") + token->token;	\
	DEBUG_PRINT(s.c_str());				\
	_recover();							\
	return -1;							\
}

#define FLAG_PASS						\
{										\
	_backdown();						\
	RECUR_PRINT("<<<",__level__, __name_save__);	\
	if(is_rule_printed(__name_save__))	\
		_save(__name_save__);			\
	return 0;							\
}

#define FLAG_SYMBOL_CHECK(tar_symbol)   \
{										\
	_next();							\
	if (!token->equal(tar_symbol))		\
		{FLAG_FAIL;}					\
	else								\
	 {									\
		_save(token);					\
		PARSE_PRINT(__level__, token->token, symbol_dict[token->symbol]);	\
	}									\
}

#define FLAG_MULTI_SYMBOL_CHECK(symbols,len) \
{										\
	_next();							\
	int cnt = 0;						\
	for(int i = 0; i < len; i++)		\
	{									\
		if(token->equal(symbols[i]))	\
		{								\
			cnt += 1;					\
			_save(token);				\
			PARSE_PRINT(__level__, token->token, symbol_dict[token->symbol]);	\
		}								\
	}									\
	if (cnt  == 0)						\
		{FLAG_FAIL;}					\
}

#define FLAG_RECUR(func) \
{										\
	int x = func(__level__ + 1);		\
	if(x != 0)							\
	{									\
		DEBUG_PRINT("ERROR WHEN RECUR");\
		FLAG_FAIL;						\
	}									\
}

// 支持回溯的“试探性”递归，当失败时不会递归式返回。
#define FLAG_RECUR_TRY(func) \
{											\
	DEBUG_PRINT("### Begin of Try Zone");	\
	int x = func();							\
	if(x != 0)								\
	{										\
		DEBUG_PRINT("TRY FAILED WHEN RECUR");\
	}										\
	DEBUG_PRINT("### End of Try Zone");		\
}

class GrammaticalParser
{
public:
	GrammaticalParser(vector<Token>& token_list,vector<string>& output);
	void parse();						// 解析器暂时没有返回
private:
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
	void _save(string t);			// 中文字符串保存
	void _save(Token* token);			// 标识符字符串保存

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

	/* 规则函数：递归下降调用 */
	int __add_operator(FLAG_FUNC_HEAD);
	int __mult_operator(FLAG_FUNC_HEAD);
	int __rel_operator(FLAG_FUNC_HEAD);
	int __letter(FLAG_FUNC_HEAD);
	int __number(FLAG_FUNC_HEAD);
	int __non_zero_number(FLAG_FUNC_HEAD);
	int __char(FLAG_FUNC_HEAD);
	int __string(FLAG_FUNC_HEAD);
	int __program(FLAG_FUNC_HEAD);
	int __const_description(FLAG_FUNC_HEAD);
	int __const_def(FLAG_FUNC_HEAD);
	int __unsigned_integer(FLAG_FUNC_HEAD);
	int __integer(FLAG_FUNC_HEAD);
	int __idenfr(FLAG_FUNC_HEAD);
	int __declar_head(FLAG_FUNC_HEAD);
	int __var_description(FLAG_FUNC_HEAD);
	int __var_def(FLAG_FUNC_HEAD);
	int __type_idenfr(FLAG_FUNC_HEAD);
	int __function_return(FLAG_FUNC_HEAD);
	int __function_void(FLAG_FUNC_HEAD);
	int __compound_statement(FLAG_FUNC_HEAD);
	int __parameter_list(FLAG_FUNC_HEAD);
	int __main_function(FLAG_FUNC_HEAD);
	int __expression(FLAG_FUNC_HEAD);
	int __item(FLAG_FUNC_HEAD);
	int __factor(FLAG_FUNC_HEAD);
	int __statement(FLAG_FUNC_HEAD);
	int __assign_statment(FLAG_FUNC_HEAD);
	int __condition_statement(FLAG_FUNC_HEAD);
	int __condition(FLAG_FUNC_HEAD);
	int __loop_statement(FLAG_FUNC_HEAD);
	int __step_length(FLAG_FUNC_HEAD);
	int __function_call_return(FLAG_FUNC_HEAD);
	int __function_call_void(FLAG_FUNC_HEAD);
	int __value_parameter_list(FLAG_FUNC_HEAD);
	int __statement_list(FLAG_FUNC_HEAD);
	int __read_statement(FLAG_FUNC_HEAD);
	int __write_statement(FLAG_FUNC_HEAD);
	int __return_statement(FLAG_FUNC_HEAD);
};