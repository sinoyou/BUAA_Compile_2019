#pragma once
#include "FileReader.h"
#include "Token.h"
#include <list>
class LexParser
{
private:
	FileReader* reader;		// input tool for lexical parser
	SYMBOL symbol;			// current parsed type
	list<Token> token_list;	// list for parsed tokens
	int _getsym();		
	void _error(const char * s);
public:
	LexParser(FileReader* reader);
	list<Token>* parse();				// �����ⲿ���������ɴʷ��������list����
};