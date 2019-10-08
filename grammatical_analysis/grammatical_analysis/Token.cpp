#include "Token.h"
Token::Token(SYMBOL symbol, string token)
{
	this->symbol = symbol;
	this->token = token;
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