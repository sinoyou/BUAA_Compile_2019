#include <tuple>
#include "GrammaticalParser.h"
#include "error.h"

void GrammaticalParser::_register_error(int line, ErrorType type) {
	char type_char = 'a' + type;
	string type_str;
	type_str.push_back(type_char);
	error_output_list.push_back(tuple<int, string>(line,type_str));
}