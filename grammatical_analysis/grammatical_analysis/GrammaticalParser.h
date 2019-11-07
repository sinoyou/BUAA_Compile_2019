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

// ö�ٱ���
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

// �ṹ�� ͷ��β
struct PARSE_HEAD {
	int level;
};
struct PARSE_RETURN {
	int state;
	int a;
	int b;
};

// �궨��
/* ���д��»��ߵ�ֻ���ں궨����ʹ�� */
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
	void parse();						// ��������ʱû�з���
private:
	// Current ��ǰϵ��
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
	void _save(string t);				// �����ַ�������
	void _save(Token* token);			// ��ʶ���ַ�������

	/* Unit4 ���֧�� */
	vector<tuple<int, string>>& error_output_list;		// error���б�
	void _register_error(int, ErrorType);				// ��error�б���ע��error

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

	void symbol_check(SYMBOL symbol,int level)	;							// ���һ���ս��
	void multi_symbol_check(SYMBOL symbols[],int len, int level) ; 			// ������ս��

	/* ���������ݹ��½����� */
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