#pragma once
#include "FileReader.h"
#include "Token.h"
#include <vector>
#include <tuple>
class LexParser
{
private:
	FileReader & reader;		// input tool for lexical parser
	SYMBOL symbol;				// current parsed type
	int current_line;			// current line in file
	vector<Token> token_list;	// list for parsed tokens
	vector<tuple<int, string>>& error_output_list;	// unit4 support

	int _getsym();		
	void _error(const char * s);
public:
	LexParser(FileReader& reader, vector<tuple<int, string>>& error_list);
	const vector<Token>& parse();				// 核心外部函数，生成词法解析后的list容器
};