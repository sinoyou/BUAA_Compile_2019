#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "Token.h"
#include "debug.h"

using namespace std;

// macro �궨��

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

// ֧�ֻ��ݵġ���̽�ԡ��ݹ飬��ʧ��ʱ����ݹ�ʽ���ء�
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
	void parse();						// ��������ʱû�з���
private:
	int ptoken;							// ����token�б������
	Token * token;						// ��ǰ����ptoken�µ�token

	vector<Token> & token_list;			// token���б�
	vector<int> ptoken_record;			// ptoken�ı��ݿռ�

	/* �ķ������������� */
	// ��ƾ�ķ��޷��������ߵĲ���
	vector<string> func_call_return_idenfr;
	vector<string> func_call_void_idenfr;
	
	/* Unit3 ���֧�� */
	vector<string> & output_list;		// ������б�
	void _save(string t);			// �����ַ�������
	void _save(Token* token);			// ��ʶ���ַ�������

	/* ֧�ֺ��� */
	void _error(const char* s);			// �����������
	Token _export_token(int index);		// ������������һ������
	void _update_token();				// ���µ���
	Token* _peek();						// ͵�����ʣ�������ptoken��token��
	Token* _peek(int x);				// ͵������ (ָ������ĵڼ���)
	void _next();						// ��ȡһ������

	void _backup();						// ѹ��һ��ָ��
	void _backdown();					// �ͷ�һ��ָ��
	void _recover();					// �ָ�һ��ָ�뱸��

	/* ���������ݹ��½����� */
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