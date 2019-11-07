#include "Token.h"
Token::Token(SYMBOL symbol, string token, int line)
{
	this->symbol = symbol;
	this->token = token;
	this->line = line;
}

bool Token::equal(SYMBOL symbol)
{
	return this->symbol == symbol;
}

bool Token::equal(SYMBOL* symbols, int len) {
	for (int i = 0; i < len; i++) {
		if (this -> symbol == symbols[i])
			return true;
	}
	return false;
}

int Token::transnum() {
	int sum = 0;
	unsigned int i = 0;
	while (i < token.length()) {
		if (token[i] >= '0' && token[i] <= '9') {
			sum = sum * 10 + token[i] - '0';
			i++;
		}
		else {
			printf("[Error] Unable to parse number %c in token %s.\n", token[i], token.c_str());
		}
	}
	return sum;
}