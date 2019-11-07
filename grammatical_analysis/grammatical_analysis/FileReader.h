#pragma once

#include<cstring>
#include<iostream>

using namespace std;

class FileReader {
private:
	string token;	// text[ptr]rent token 
	string text;	// reading buffer
	int ptr;		// pointer in reading buffer

public:
	// ���캯��
	FileReader(const char* path);

	// ��Ա����: ����ַ�������
	void catToken();
	void clearToken();
	void getchar();
	void retract();
	string getToken() { return token; }
	string getText() {  return text; }

	// ��Ա�������ַ����жϺ���
	/* ʶ����� */
	bool isSpace() { return text[ptr] == ' '; }							// �ж��Ƿ�Ϊ�ո�
	bool isNewlineN() { return text[ptr] == '\n'; }						// �ж��Ƿ�Ϊ���з� \n
	bool isNewlineR() { return text[ptr] == '\r'; }						// �ж��Ƿ�Ϊ���з� \r
	bool isTab() { return text[ptr] == '\t'; }							// �ж��Ƿ�Tab
	bool isLetter() { return (text[ptr] >= 'a' && text[ptr] <= 'z') 
		|| (text[ptr] >= 'A' && (text[ptr] <= 'Z')) 
		|| text[ptr] == '_'; }											// ��ĸ
	bool isDigit() { return text[ptr] >= '0' && text[ptr] <= '9'; }		// ����
	bool isNZeroDigit() { return text[ptr] >= '1' && text[ptr] <= '9'; }// ����
	bool isComma() { return text[ptr] == ','; }							// ����
	bool isSemicn() { return text[ptr] == ';'; }						// �ֺ�
	bool isEqu() { return text[ptr] == '='; }							// �Ⱥ�
	bool isPlus() { return text[ptr] == '+'; }							// +
	bool isMinu() { return text[ptr] == '-'; }							// -
	bool isDiv() { return text[ptr] == '/'; }							// /
	bool isMult() { return text[ptr] == '*'; }							// *
	bool isLss() { return text[ptr] == '<'; }							// <
	bool isGre() { return text[ptr] == '>'; }							// >
	bool isExclam() { return text[ptr] == '!'; }						// !
	bool isLparent() { return text[ptr] == '('; }						// (
	bool isRparent() { return text[ptr] == ')'; }						// )
	bool isLbrack() { return text[ptr] == '['; }						// [
	bool isRbrack() { return text[ptr] == ']'; }						// ]
	bool isLbrace() { return text[ptr] == '{'; }						// {
	bool isRbrace() { return text[ptr] == '}'; }						// }
	bool isChar() { return (text[ptr] >= 35 && text[ptr] <= 126) 
		|| text[ptr] == 32 
		|| text[ptr] == 33; }											// �ַ�������ɶ���
	bool isSinQuotation() { return text[ptr] == '\''; }					// '
	bool isDouQuotation() { return text[ptr] == '\"'; }					// "
	bool isEOT() { return text[ptr] == 0; }								// End Of Text
};
