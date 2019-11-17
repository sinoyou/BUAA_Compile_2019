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

// �ṹ�� ͷ��β�����ڸ��ݹ��½��޷�����ͳһ�����Ѿ�����ͳһ�Ľṹ����Ϊ�����Ĳ����ͷ���ֵ��

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
	void parse();						// ��������ʱû�з���
private:
	// Current ��ǰϵ��
	int ptoken;							// ����token�б������
	Token * token;						// ��ǰ����ptoken�µ�token
	SymbolTable symbol_table;			// ���ű�

	vector<Token> & token_list;			// token���б�
	vector<int> ptoken_record;			// ptoken�ı��ݿռ�

	/* �ķ������������� */
	// ��ƾ�ķ��޷��������ߵĲ���
	// vector<string> func_call_return_idenfr;
	// vector<string> func_call_void_idenfr;
	
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