#pragma once
#include <string>
#include <vector>
#include "Token.h"

using namespace std;

class GrammaticalParser
{
public:
	GrammaticalParser(vector<Token>& token_list,vector<string>& output);
	void parse();						// ��������ʱû�з���
private:
	int ptoken;							// ����token�б������
	Token * token;						// ��ǰ����ptoken�µ�token
	vector<Token> & token_list;			// token���б�
	
	/* Unit3 ���֧�� */
	vector<string> & output_list;		// ������б�
	void _save(const char* s);			// �����ַ�������
	void _save(int head, int tail);		// ��ʶ���ַ�������

	/* ֧�ֺ��� */
	void _error(const char* s);			// �����������
	Token _gettoken_index(int index);	// ����������ȡһ������
	void _gettoken();					// ��ȡһ������
	void _retract();					// ����һ������

	/* ���������ݹ��½����� */
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

