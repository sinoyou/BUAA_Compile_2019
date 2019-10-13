#include "GrammaticalParser.h"
#include "utils.h"
void GrammaticalParser::_save(string t)
{
	output_list.push_back(t);
}

void GrammaticalParser::_save(Token * token)
{
	//int h = ptoken_record.back() + 1;
	//int t = ptoken;
	//for (int i = h; i <= t; i++) {
	//	Token token = _export_token(i);
	//	string t = symbol_dict[token.symbol] + " " + token.token;
	//	output_list.push_back(t);
	//}
	string t = symbol_dict[token->symbol] + " " + token->token;
	output_list.push_back(t);
}