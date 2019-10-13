#pragma once
#include "symbol.h"
void print_token(FILE* f, SYMBOL symbol, string token);
SYMBOL reserver(string token);
bool is_rule_printed(string s);