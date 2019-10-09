#include "GrammaticalParser.h"
#include "utils.h"
void GrammaticalParser::_save(const char* s)
{
	string t = string(s);
	output_list.push_back(t);
}

void GrammaticalParser::_save(int h, int t)
{
	for (int i = h; i <= t; i++) {
		Token token = _export_token(i);
		string t = symbol_dict[token.symbol] + " " + token.token;
		output_list.push_back(t);
	}
}