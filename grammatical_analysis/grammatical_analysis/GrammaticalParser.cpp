#include "GrammaticalParser.h"
/* ֧�ֺ��� */
void _error(const char* s);			// �����������
Token _gettoken_index(int index);	// ����������ȡһ������
void _gettoken();					// ��ȡһ������
void _retract();					// ����һ������

/* ��ʼ������ */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = 0;
}

/* ֧�ֺ��� */
void GrammaticalParser::_error(const char * s) {
	printf("Face an error when parsing grammar %s.\n", s);
}

Token GrammaticalParser::_gettoken_index(int index)
{
	return token_list[index];
}
void GrammaticalParser::_gettoken()
{
	ptoken++;
	token = &(token_list[ptoken]);
}
void GrammaticalParser::_retract()
{
	ptoken--;
	token = &(token_list[ptoken]);
}