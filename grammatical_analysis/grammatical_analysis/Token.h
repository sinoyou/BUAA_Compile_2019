#pragma once
#include "symbol.h"
#include <string>

class Token
{
public:
	SYMBOL symbol;
	string token;
	int line;
	Token(SYMBOL symbol, string token, int line);
	
	int transnum();
	bool equal(SYMBOL symbol);
	bool equal(SYMBOL* symbols, int len);
};

