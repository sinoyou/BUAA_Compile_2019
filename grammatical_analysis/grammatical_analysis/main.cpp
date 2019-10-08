// token_analysis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include <cstdio>
#include <list>

// Global Variable and Pointer
FileReader* reader;
FILE* f;

int main()
{
	// initial output
	f = fopen("output.txt", "w");
	// initial reader class
	FileReader t("testfile.txt");
	reader = &t;

	LexParser lexParser = LexParser(reader);
	list<Token>* token_list_ptr = lexParser.parse();

	list<Token>::iterator itr = token_list_ptr->begin();
	while (itr!= token_list_ptr->end()) {
		print_token(f,itr->symbol, itr->token);
		itr++;
	}
	fclose(f);
	return 0;
}
//
///* 错误处理 */
//void error(const char* s) {
//	printf("We Face an Error When Parsing %s.\n", s);
//}
//
///* 词法分析函数 */
//int getsym() {
//	reader->clearToken();
//	/* 清除无关空白符 */
//	while (reader->isSpace() || reader->isNewline() || reader->isTab()) {
//		reader->getchar();
//	}
//
//	/* 枚举优先级：保留字 - 标识符 - 常整数 - 分界符 - 字符 - 字符串 */
//	/* 保留字与标识符 */
//	if (reader->isLetter()) {
//		while (reader->isLetter() || reader->isDigit()) {
//			reader->catToken();
//			reader->getchar();
//		}
//		reader->retract();							
//		SYMBOL resultvalue = reserver(reader->getToken());
//		if (resultvalue == 0)
//			symbol = IDENFR;
//		else
//			symbol = resultvalue;
//	}
//	/* 常整数 */
//	// Question : 是否允许前置零？
//	else if (reader->isDigit()) {
//		// non-zero : must start with a non-zero number.
//		if (reader->isNZeroDigit()) {
//			while (reader->isDigit()) {
//				reader->catToken();
//				reader->getchar();
//			}
//			reader->retract();
//			symbol = INTCON;
//		}
//		// zero : must only 1 '0'.
//		else {
//			reader->catToken();
//			reader->getchar();
//			if (!reader->isDigit()) {
//				reader->retract();
//				symbol = INTCON;
//			}
//			else
//				error("INTCON");
//		}
//	}
//	/* 双分界符 */
//	else if (reader->isLss()) { // < | <=
//		reader->catToken();
//		reader->getchar();
//		if (reader->isEqu()) {
//			reader->catToken();
//			symbol = LEQ;
//		}
//		else {
//			reader->retract();
//			symbol = LSS;
//		}
//	}
//	else if (reader->isGre()) { // > | >=
//		reader->catToken();
//		reader->getchar();
//		if (reader->isEqu())
//		{
//			reader->catToken();
//			symbol = GEQ;
//		}
//		else {
//			reader->retract();
//			symbol = GRE;
//		}
//	}
//	else if (reader->isEqu()) { // = | ==
//		reader->catToken();
//		reader->getchar();
//		if (reader->isEqu())
//		{
//			reader->catToken();
//			symbol = EQL;
//		}
//		else
//		{
//			reader->retract();
//			symbol = ASSIGN;
//		}
//	}
//	else if (reader->isExclam()) { // !=
//		reader->catToken();
//		reader->getchar();
//		if (reader->isEqu())
//		{
//			reader->catToken();
//			symbol = NEQ;
//		}
//		else {
//			// todo
//			error("!=");
//		}
//	}
//	/* 单分界符号 */
//	else if (reader->isPlus()) reader->catToken(),symbol = PLUS;
//	else if (reader->isMinu()) reader->catToken(),symbol = MINU;
//	else if (reader->isMult()) reader->catToken(),symbol = MULT;
//	else if (reader->isDiv())  reader->catToken(),symbol = DIV;
//	else if (reader->isSemicn()) reader->catToken(),symbol = SEMICN;
//	else if (reader->isComma()) reader->catToken(),symbol = COMMA;
//	else if (reader->isLparent()) reader->catToken(),symbol = LPARENT;
//	else if (reader->isRparent()) reader->catToken(),symbol = RPARENT;
//	else if (reader->isLbrack()) reader->catToken(),symbol = LBRACK;
//	else if (reader->isRbrack()) reader->catToken(),symbol = RBRACK;
//	else if (reader->isLbrace()) reader->catToken(),symbol = LBRACE;
//	else if (reader->isRbrace()) reader->catToken(),symbol = RBRACE;
//	/* 字符 */
//	else if (reader->isSinQuotation()) {
//		reader->getchar();
//		// Question : 可见字符即可还是需要按照文法定义进行?
//		if (reader->isDigit() || reader->isLetter() || reader->isPlus() || reader->isMinu() || reader->isMult() || reader->isDiv()) {
//			reader->catToken();
//			reader->getchar();
//			if (reader->isSinQuotation())
//				symbol = CHARCON;
//			else
//				error("CHARCON");
//		}
//	}
//	/* 字符串 */
//	else if (reader->isDouQuotation()) {
//		// 转置符 暂时不考虑
//		reader->getchar();
//		while (reader->isChar() && !reader->isDouQuotation()) {
//			reader->catToken();
//			reader->getchar();
//		}
//		if (reader->isDouQuotation()) {
//			symbol = STRCON;
//		}
//		else {
//			error("STRCON");
//		}
//	}
//	else if (reader->isEOT()) {
//		return -1;
//	}
//	else
//		error("UNKNOWN");
//	return 0;
//}