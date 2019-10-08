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
};

