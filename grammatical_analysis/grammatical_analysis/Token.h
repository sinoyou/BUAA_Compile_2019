#pragma once
#include "symbol.h"
#include <string>

class Token
{
public:
	SYMBOL symbol;
	string token;
	Token(SYMBOL symbol, string token);
	
	int transnum();
	bool equal(SYMBOL symbol);
	bool equal(SYMBOL* symbols, int len);
};

