#include "utils.h"
#include <cstdio>

string symbol_dict[50] = {"UNKNOWN","IDENFR","INTCON","CHARCON","STRCON","CONSTTK","INTTK",
	"CHARTK","VOIDTK","MAINTK","IFTK","ELSETK","DOTK","WHILETK","FORTK","SCANFTK","PRINTFTK",
	"RETURNTK","PLUS","MINU","MULT","DIV","LSS","LEQ","GRE","GEQ","EQL","NEQ","ASSIGN","SEMICN",
	"COMMA","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE"
};

/* 保留字分析函数 */
SYMBOL reserver(string s) {
	if (s == "const") return CONSTTK;
	else if (s == "int") return INTTK;
	else if (s == "char") return CHARTK;
	else if (s == "void") return VOIDTK;
	else if (s == "main") return MAINTK;
	else if (s == "if") return IFTK;
	else if (s == "else") return ELSETK;
	else if (s == "do") return DOTK;
	else if (s == "while") return WHILETK;
	else if (s == "for") return FORTK;
	else if (s == "scanf") return SCANFTK;
	else if (s == "printf") return PRINTFTK;
	else if (s == "return") return RETURNTK;
	else
		return UNKNOWN;
}

/* 根据词法分析结果进行输出 */
void print_token(FILE* f, SYMBOL symbol, string token)
{
	fprintf(f, "%s %s\n", symbol_dict[symbol].c_str(), token.c_str());
}