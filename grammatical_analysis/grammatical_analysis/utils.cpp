#include "utils.h"
#include <cstdio>

string symbol_dict[50] = {"UNKNOWN","IDENFR","INTCON","CHARCON","STRCON","CONSTTK","INTTK",
	"CHARTK","VOIDTK","MAINTK","IFTK","ELSETK","DOTK","WHILETK","FORTK","SCANFTK","PRINTFTK",
	"RETURNTK","PLUS","MINU","MULT","DIV","LSS","LEQ","GRE","GEQ","EQL","NEQ","ASSIGN","SEMICN",
	"COMMA","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE"
};

string print_rule_dict[30] = {
	"<字符串>","<程序>","<常量说明>","<常量定义>","<无符号整数>","<整数>","<声明头部>",
	"<变量说明>","<变量定义>","<有返回值函数定义>","<无返回值函数定义>","<复合语句>","<参数表>","<主函数>",
	"<表达式>","<项>","<因子>","<语句>","<赋值语句>","<条件语句>","<条件>",
	"<循环语句>","<步长>","<有返回值函数调用语句>","<无返回值函数调用语句>","<值参数表>","<语句列>","<读语句>",
	"<写语句>","<返回语句>"
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

/* 查看该中文规则是否应该输出 */
bool is_rule_printed(string s) {
	for (int i = 0; i < 30; i++) {
		if (print_rule_dict[i] == s)
			return true;
	}
	return false;
}