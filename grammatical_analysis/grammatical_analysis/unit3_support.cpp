#include "GrammaticalParser.h"
#include "utils.h"
void GrammaticalParser::_save(string t)
{
	output_list.push_back(t);
}

void GrammaticalParser::_save(Token * token)
{
	string t = symbol_dict[token->symbol] + " " + token->token;
	output_list.push_back(t);
}