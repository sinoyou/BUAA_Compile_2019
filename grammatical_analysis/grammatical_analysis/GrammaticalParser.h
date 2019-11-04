#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "Token.h"
#include "debug.h"
#include "error.h"

using namespace std;

// �궨����ṹ��
struct PARSE_HEAD {
	int level;
};

/* ���д��»��ߵ�ֻ���ں궨����ʹ�� */
#define FLAG_ENTER(name,level)									\
string __name_save__(name);										\
int __level__ = level;											\
{																\
	_backup();													\
	RECUR_PRINT(">>>",__level__, __name_save__);				\
}															

// #define FLAG_FAIL \
{										\
	string s = string("ERROR WHEN PARSING ") + token->token;	\
	DEBUG_PRINT(s.c_str());				\
	_recover();							\
	return -1;							\
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
	return 0;							\
}

// #define FLAG_SYMBOL_CHECK(tar_symbol)   \
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

// #define FLAG_MULTI_SYMBOL_CHECK(symbols,len) \
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

#define RECUR_CHECK(func) func(PARSE_HEAD{head.level + 1});

// #define FLAG_RECUR(func) \
{										\
	int x = func(__level__ + 1);		\
	if(x != 0)							\
	{									\
		DEBUG_PRINT("ERROR WHEN RECUR");\
		FLAG_FAIL;						\
	}									\
}

// ֧�ֻ��ݵġ���̽�ԡ��ݹ飬��ʧ��ʱ����ݹ�ʽ���ء�
// #define FLAG_RECUR_TRY(func) \
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

	void SYMBOL_CHECK(SYMBOL symbol) throw();							// ���һ���ս��
	void MULTI_SYMBOL_CHECK(SYMBOL symbols[],int len) throw(); 			// ������ս��

	/* ���������ݹ��½����� */
	int __add_operator(PARSE_HEAD head);
	int __mult_operator(PARSE_HEAD head);
	int __rel_operator(PARSE_HEAD head);
	int __letter(PARSE_HEAD head);
	int __number(PARSE_HEAD head);
	int __non_zero_number(PARSE_HEAD head);
	int __char(PARSE_HEAD head);
	int __string(PARSE_HEAD head);
	int __program(PARSE_HEAD head);
	int __const_description(PARSE_HEAD head);
	int __const_def(PARSE_HEAD head);
	int __unsigned_integer(PARSE_HEAD head);
	int __integer(PARSE_HEAD head);
	int __idenfr(PARSE_HEAD head);
	int __declar_head(PARSE_HEAD head);
	int __var_description(PARSE_HEAD head);
	int __var_def(PARSE_HEAD head);
	int __type_idenfr(PARSE_HEAD head);
	int __function_return(PARSE_HEAD head);
	int __function_void(PARSE_HEAD head);
	int __compound_statement(PARSE_HEAD head);
	int __parameter_list(PARSE_HEAD head);
	int __main_function(PARSE_HEAD head);
	int __expression(PARSE_HEAD head);
	int __item(PARSE_HEAD head);
	int __factor(PARSE_HEAD head);
	int __statement(PARSE_HEAD head);
	int __assign_statment(PARSE_HEAD head);
	int __condition_statement(PARSE_HEAD head);
	int __condition(PARSE_HEAD head);
	int __loop_statement(PARSE_HEAD head);
	int __step_length(PARSE_HEAD head);
	int __function_call_return(PARSE_HEAD head);
	int __function_call_void(PARSE_HEAD head);
	int __value_parameter_list(PARSE_HEAD head);
	int __statement_list(PARSE_HEAD head);
	int __read_statement(PARSE_HEAD head);
	int __write_statement(PARSE_HEAD head);
	int __return_statement(PARSE_HEAD head);
};