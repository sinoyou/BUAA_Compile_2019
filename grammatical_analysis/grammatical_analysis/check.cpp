#include "GrammaticalParser.h"
#include "debug.h"

// ���һ���ս��
void GrammaticalParser::symbol_check(SYMBOL symbol, int level) {
	_next();
	if (!token->equal(symbol)) {
		throw ParseException(ParseExceptionType::WrongToken, token->token);
	}
	else {
		_save(token);					
		PARSE_PRINT(level, token->token, symbol_dict[token->symbol]);	
	}
}

// ������ս��
void GrammaticalParser::multi_symbol_check(SYMBOL symbols[], int len, int level) {
	_next();
	int cnt = 0;
	for (int i = 0; i < len; i++) {
		if (token->equal(symbols[i])) {
			cnt += 1;
			_save(token);
			PARSE_PRINT(level, token->token, token->token);
		}
	}
	if (cnt == 0) {
		throw ParseException(ParseExceptionType::WrongToken, string(symbol_dict[token->symbol]));
	}
}