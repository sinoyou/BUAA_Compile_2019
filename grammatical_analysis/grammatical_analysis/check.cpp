#include "GrammaticalParser.h"
#include "debug.h"

// 检查一个终结符
void GrammaticalParser::symbol_check(SYMBOL symbol, int level) {
	if (!_peek()->equal(symbol)) {
		if (!_peek()->equal(SYMBOL::UNKNOWN)) {
			// unit4-error-k
			// unit4-error-l
			// unit4-error-m
			if (symbol == SYMBOL::SEMICN) {
				_register_error(token->line, ErrorType::MissSemicn);
			}
			else if (symbol == SYMBOL::RPARENT)
			{
				_register_error(token->line, ErrorType::MissRightParent);
			}
			else if (symbol == SYMBOL::RBRACK) {
				_register_error(token->line, ErrorType::MissRightBrack);
			}
			else
				throw ParseException(ParseExceptionType::WrongToken, token->token);
		}
		else {
			_next();
		}
	}
	else {
		_next();
		_save(token);					
		PARSE_PRINT(level, token->token, symbol_dict[token->symbol]);	
	}
}

// 检查多个终结符
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