#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
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
	int __add_operator();
	int __mult_operator();
	int __rel_operator();
	int __letter();
	int __number();
	int __non_zero_number();
	int __char();
	int __string();
	int __program();
	int __const_description();
	int __const_def();
	int __unsigned_integer();
	int __integer();
	int __idenfr();
	int __declar_head();
	int __var_description();
	int __var_def();
	int __type_idenfr();
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

