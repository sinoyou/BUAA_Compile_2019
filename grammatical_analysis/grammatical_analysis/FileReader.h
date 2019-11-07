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
	// 构造函数
	FileReader(const char* path);

	// 成员函数: 句柄字符操作类
	void catToken();
	void clearToken();
	void getchar();
	void retract();
	string getToken() { return token; }
	string getText() {  return text; }

	// 成员函数：字符级判断函数
	/* 识别相关 */
	bool isSpace() { return text[ptr] == ' '; }							// 判断是否为空格
	bool isNewlineN() { return text[ptr] == '\n'; }						// 判断是否为换行符 \n
	bool isNewlineR() { return text[ptr] == '\r'; }						// 判断是否为换行符 \r
	bool isTab() { return text[ptr] == '\t'; }							// 判断是否Tab
	bool isLetter() { return (text[ptr] >= 'a' && text[ptr] <= 'z') 
		|| (text[ptr] >= 'A' && (text[ptr] <= 'Z')) 
		|| text[ptr] == '_'; }											// 字母
	bool isDigit() { return text[ptr] >= '0' && text[ptr] <= '9'; }		// 数字
	bool isNZeroDigit() { return text[ptr] >= '1' && text[ptr] <= '9'; }// 数字
	bool isComma() { return text[ptr] == ','; }							// 逗号
	bool isSemicn() { return text[ptr] == ';'; }						// 分号
	bool isEqu() { return text[ptr] == '='; }							// 等号
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
		|| text[ptr] == 33; }											// 字符串的组成定义
	bool isSinQuotation() { return text[ptr] == '\''; }					// '
	bool isDouQuotation() { return text[ptr] == '\"'; }					// "
	bool isEOT() { return text[ptr] == 0; }								// End Of Text
};
