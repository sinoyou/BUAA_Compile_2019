#include "GrammaticalParser.h"
#include "utils.h"
#include "debug.h"

/* 初始化函数 */
GrammaticalParser::GrammaticalParser(
	vector<Token>& token_list, 
	vector<string>& output,
	vector<tuple<int, string>>& error_output)
	:token_list(token_list), output_list(output), error_output_list(error_output) {
	token = NULL;
	ptoken = -1;
}

/* 入口函数 */
void GrammaticalParser::parse() {
	__program(PARSE_HEAD{1});
}

/* 支持函数 */
void GrammaticalParser::_error(const char* s) {
	printf("Face an error when parsing grammar %s.\n", s);
}

Token GrammaticalParser::_export_token(int index)
{
	return token_list[index];
}

void GrammaticalParser::_update_token() {
	token = &(token_list[ptoken]);
}

Token* GrammaticalParser::_peek() {
	return &(token_list[ptoken + 1]);
}

Token* GrammaticalParser::_peek(int x) {
	return &(token_list[ptoken + x]);
}

void GrammaticalParser::_next()
{
	ptoken++;
	_update_token();
}

void GrammaticalParser::_backup() {
	// ptoken备份
	ptoken_record.push_back(ptoken);
}

void GrammaticalParser::_backdown() {
	// ptoken 备份在顺利完成后的删除
	ptoken_record.pop_back();
}

void GrammaticalParser::_recover() {
	// ptoken与token恢复;
	ptoken = ptoken_record.back();
	ptoken_record.pop_back();
	_update_token();
}

/**************************** 解析函数定义 ******************************/
/*
 * 1. 部分过简单的数字与字母类型识别被忽略，已被词法分析解决。
 * 2. <字符>匹配不是严格的。
 * 3. 在对每条规则分析时，部分计算了FOLLOW集合，但FOLLOW集合并不应该纳入该规则的策略选择中，只是为了验证规则不产生回溯情况。
 * 4. 特例-超前偷窥：<程序>, <变量说明>, <因子>, <语句>
 * 5. 特例-不同规则，相同结构：需建立符号表判断<有返回函数调用语句>和<无返回函数调用语句>
 */

 /**
  * ＜加法运算符＞ ::= +｜-
  * FIRST(+) = {PLUS}, FIRST(-) = {SUB}
  */
PARSE_RETURN GrammaticalParser::__add_operator(PARSE_HEAD head) {
	FLAG_ENTER("<加法运算符>",head.level);
	try {
		SYMBOL candidate[] = { SYMBOL::PLUS,SYMBOL::MINU };
		MULTI_SYMBOL_CHECK(candidate, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜乘法运算符＞  ::= *｜/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
PARSE_RETURN GrammaticalParser::__mult_operator(PARSE_HEAD head) {
	FLAG_ENTER("<乘法运算符>", head.level);
	try {
		SYMBOL candidate[] = { SYMBOL::MULT,SYMBOL::DIV };
		MULTI_SYMBOL_CHECK(candidate, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
 * FIRST：...
*/
PARSE_RETURN GrammaticalParser::__rel_operator(PARSE_HEAD head) {
	FLAG_ENTER("<关系运算符>", head.level);
	try {
		SYMBOL list[6] = { SYMBOL::LSS, SYMBOL::LEQ, SYMBOL::GRE, SYMBOL::GEQ, SYMBOL::NEQ, SYMBOL::EQL };
		MULTI_SYMBOL_CHECK(list, 6);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜字母＞::= ＿｜a｜．．．｜z｜A｜．．．｜Z
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__letter(PARSE_HEAD head) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<字母>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Char>"));
}

/**
 * ＜数字＞::= ０｜＜非零数字＞
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__number(PARSE_HEAD head) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<数字>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Number>"));
}

/**
 * ＜非零数字＞::= １｜．．．｜９
 * FIRST: ...
 */
PARSE_RETURN GrammaticalParser::__non_zero_number(PARSE_HEAD head) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<非零数字>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Non-Zero Number>"));
}

/**
 * ＜字符＞::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
 * FIRST: '
 * !: 词法分析时由于已经忽略了单引号'并判断了字符，因此该规则匹配不完全严格。
*/
PARSE_RETURN GrammaticalParser::__char(PARSE_HEAD head) {
	FLAG_ENTER("<字符>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::CHARCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜字符串＞::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 * FIRST: "
 * !: 词法分析时由于已经忽略了双引号"并判断了字符，因此该规则匹配不完全严格.
*/
PARSE_RETURN GrammaticalParser::__string(PARSE_HEAD head) {
	FLAG_ENTER("<字符串>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::STRCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜程序＞::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
 * FISRT(<常量说明>) = {CONSTTK}
 * FISRT(<变量说明>) = {INTTK, CHARTK}
 * FISRT(<有返回的函数定义>) = {INTTK, CHARTK}
 * FISRT(<无返回的函数定义>) = {VOIDTK}
 * FISRT(<主函数>) = {VOIDTK}
 *
 * !:{INTTK, CHARTK} 出现了FISRT集合的冲突，需要更多的偷窥 - 针对<变量说明>和<有函数值函数定义>
 * !:{VOIDTK} 出现了FIRST集合的冲突，需要更多的偷窥
 */
PARSE_RETURN GrammaticalParser::__program(PARSE_HEAD head)
{
	FLAG_ENTER("<程序>", head.level);
	try {
		// <常量说明>
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			RECUR_CHECK(__const_description);
		}

		// <变量说明>: 用反例判断，<变量说明>和<有返回值的函数定义>从后续第三个字符起出现差异
		if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			RECUR_CHECK(__var_description);
		}

		// {＜有返回值函数定义＞ | ＜无返回值函数定义＞}
		// ＜无返回值函数定义＞需要排除void main
		while (
			(_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) ||
			(_peek()->equal(SYMBOL::VOIDTK) && !_peek(2)->equal(SYMBOL::MAINTK))
			)
		{
			if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
				RECUR_CHECK(__function_return);
			}
			else {
				RECUR_CHECK(__function_void);
			}
		}
		// <主函数>
		RECUR_CHECK(__main_function);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
 * FISRT(<常量说明>) = {CONSTTK}
 * FOLLOW(<常量说明>) = {...}
 * 不存在FISRT(< const＜常量定义＞;>)和FOLLOW(<常量说明>)集合交叉情况。
*/
PARSE_RETURN GrammaticalParser::__const_description(PARSE_HEAD head)
{
	FLAG_ENTER("<常量说明>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::CONSTTK);
		RECUR_CHECK(__const_def);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		while (_peek()->equal(SYMBOL::CONSTTK)) {
			SYMBOL_CHECK(SYMBOL::CONSTTK);
			RECUR_CHECK(__const_def);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		};
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<常量定义>) = {SEMICN}
 * 不存在FOLLOW(<常量定义>)与FIRST(,＜标识符＞＝＜整数＞| <字符>) 相交情况。
*/
PARSE_RETURN GrammaticalParser::__const_def(PARSE_HEAD head)
{
	FLAG_ENTER("<常量定义>", head.level);
	try {
		if (_peek()->equal(SYMBOL::INTTK)) {
			SYMBOL_CHECK(SYMBOL::INTTK);
			RECUR_CHECK(__idenfr);					// <标识符>
			SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
			RECUR_CHECK(__integer);					// <整数>
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__idenfr);
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				RECUR_CHECK(__integer);
			}
		}
		else if (_peek()->equal(SYMBOL::CHARTK)) {
			SYMBOL_CHECK(SYMBOL::CHARTK);
			RECUR_CHECK(__idenfr);					// <标识符>
			SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
			RECUR_CHECK(__char);						// <符号数>
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__idenfr);
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				RECUR_CHECK(__char);
			}
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, token->token);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝| 0
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__unsigned_integer(PARSE_HEAD head) {
	FLAG_ENTER("<无符号整数>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::INTCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜整数＞::= ［＋｜－］＜无符号整数＞
 * FIRST(1) = {PLUS}, FIRST(2) = {MINU}, FIRST(3) = {INTCON}
*/
PARSE_RETURN GrammaticalParser::__integer(PARSE_HEAD head)
{
	FLAG_ENTER("<整数>", head.level);
	try {
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
		}
		RECUR_CHECK(__unsigned_integer);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜标识符＞::=  ＜字母＞｛＜字母＞｜＜数字＞｝
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__idenfr(PARSE_HEAD head) {
	FLAG_ENTER("<标识符>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::IDENFR);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜声明头部＞::=  int＜标识符＞ | char＜标识符＞
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
PARSE_RETURN GrammaticalParser::__declar_head(PARSE_HEAD head) {
	FLAG_ENTER("<声明头部>", head.level);
	try {
		if (_peek()->equal(SYMBOL::INTTK))
		{
			SYMBOL_CHECK(SYMBOL::INTTK);
			RECUR_CHECK(__idenfr);
		}
		else if (_peek()->equal(SYMBOL::CHARTK)) {
			SYMBOL_CHECK(SYMBOL::CHARTK);
			RECUR_CHECK(__idenfr);
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<Declaration Head>"));
		}
	}
	catch(ParseException & e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜变量说明＞::= ＜变量定义＞;{＜变量定义＞;}
 * FIRST(<变量说明>) = FISRT(<变量定义>) = {INTTK, CHARTK}
 * FOLLOW(<变量说明>) = FISRT(<有返回值函数定义>) + FISRT(<无返回值函数定义>) + FIRST(<主函数>) + FIRST(<语句列>)
 * {INTTK, CHARTK} // _peek(3) = ( ?
 * {VOIDTK}		   // _peek() = VOIDTK
 * {VOIDTK}        // _peek() = VOIDTK
 * {IFTK, WHILETK, DOTK, FORTK, LBRACE, INTTK, CHARTK,    // _peek(3) = (?
 *  VOIDTK, IDENFR, SCANFTK, PRINTFTK, SEMICN, RETURNTK}
 */
PARSE_RETURN GrammaticalParser::__var_description(PARSE_HEAD head)
{
	FLAG_ENTER("<变量说明>", head.level);
	try {
		SYMBOL fisrt_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		RECUR_CHECK(__var_def);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		// 用反例修正仅参考FIRST集合的不足
		while (_peek()->equal(fisrt_list, 2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			RECUR_CHECK(__var_def);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜变量定义＞::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )}
 * FOLLOW(<变量定义>) = {SEMICN}, FISRT(, <标识符>...) 与 FIRST(<'[' <无符号函数> ']'>)没有交集.
 * FOLLOW(<变量定义>) = {SEMICN} 与 FIRST(<, <标识符>>) 没有交集
 * * 单变量和数组变量的声明差异[]取或不取来表示
*/
PARSE_RETURN GrammaticalParser::__var_def(PARSE_HEAD head)
{
	FLAG_ENTER("<变量定义>", head.level);
	try {
		RECUR_CHECK(__type_idenfr);
		RECUR_CHECK(__idenfr);
		// '[' <无符号整数> ']'
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			RECUR_CHECK(__unsigned_integer);
			SYMBOL_CHECK(SYMBOL::RBRACK);
		}
		// {} 
		while (_peek()->equal(SYMBOL::COMMA)) {
			SYMBOL_CHECK(SYMBOL::COMMA);
			RECUR_CHECK(__idenfr);
			// '[' <无符号整数> ']'
			if (_peek()->equal(SYMBOL::LBRACK)) {
				SYMBOL_CHECK(SYMBOL::LBRACK);
				RECUR_CHECK(__unsigned_integer);
				SYMBOL_CHECK(SYMBOL::RBRACK);
			}
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}


/**
 * ＜类型标识符＞::=  int | char
*/
PARSE_RETURN GrammaticalParser::__type_idenfr(PARSE_HEAD head) {
	FLAG_ENTER("<类型标识符>", head.level);
	try {
		SYMBOL list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		MULTI_SYMBOL_CHECK(list, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
*/
PARSE_RETURN GrammaticalParser::__function_return(PARSE_HEAD head) {
	FLAG_ENTER("<有返回值函数定义>", head.level);
	try {
		RECUR_CHECK(__declar_head);
		Token save = *token;

		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__parameter_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
		SYMBOL_CHECK(SYMBOL::LBRACE);
		RECUR_CHECK(__compound_statement);
		SYMBOL_CHECK(SYMBOL::RBRACE);

		func_call_return_idenfr.push_back(save.token);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
*/
PARSE_RETURN GrammaticalParser::__function_void(PARSE_HEAD head)
{
	FLAG_ENTER("<无返回值函数定义>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);
		RECUR_CHECK(__idenfr);
		Token save = *token;

		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__parameter_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		SYMBOL_CHECK(SYMBOL::LBRACE);
		RECUR_CHECK(__compound_statement);
		SYMBOL_CHECK(SYMBOL::RBRACE);

		func_call_void_idenfr.push_back(save.token);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜复合语句＞::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
 * FISRT(<常量说明>) = {CONSTK}， FIRST(<变量说明>) = {FIRST(<变量定义>)} = {INTTK, CHARTK}
 * FOLLOW(...<[变量说明]>) = FISRT(<语句列>) 与 FISRT(<变量说明>) 没有交集
*/
PARSE_RETURN GrammaticalParser::__compound_statement(PARSE_HEAD head) {
	FLAG_ENTER("<复合语句>", head.level);
	try {
		SYMBOL first_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		// [<const des>]
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			RECUR_CHECK(__const_description);
		}
		// [<var des>]
		if (_peek()->equal(first_list, 2)) {
			RECUR_CHECK(__var_description);
		}
		RECUR_CHECK(__statement_list);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜参数表＞::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞} | ＜空＞
 * 不带回溯的文法分析方式：1.两种选择的FIRST集合不能有交集。 2.至多一个空。 3.导出空的选择，非空导出退出的序列中不能含有FOLLOW(<参数表>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(参数表) =  {RPARENT}
 * FOLLOW(<标识符>) = {RPARENT} 与 FIRST(<,<类型标识符><标识符>>) 无交集
*/
PARSE_RETURN GrammaticalParser::__parameter_list(PARSE_HEAD head)
{
	FLAG_ENTER("<参数表>", head.level);
	try {
		// ~<空>
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			RECUR_CHECK(__type_idenfr);
			RECUR_CHECK(__idenfr);
			while (_peek()->equal(SYMBOL::COMMA))
			{
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__type_idenfr);
				RECUR_CHECK(__idenfr);
			}
		}
		// <空>
		else {

		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜主函数＞::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 * FIRST(x): {VOIDTK}
*/
PARSE_RETURN GrammaticalParser::__main_function(PARSE_HEAD head)
{
	FLAG_ENTER("<主函数>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);				// void
		SYMBOL_CHECK(SYMBOL::MAINTK);				// main
		SYMBOL_CHECK(SYMBOL::LPARENT);				// (
		SYMBOL_CHECK(SYMBOL::RPARENT);				// )
		SYMBOL_CHECK(SYMBOL::LBRACE);				// {
		RECUR_CHECK(__compound_statement);			// <复合语句>
		SYMBOL_CHECK(SYMBOL::RBRACE);				// }
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
 * FIRST(<表达式>): {PLUS, MINU, FIRST(项)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * 表达式将允许 ++5 - 1 的情况出现
 *
 * FIRST(<加法运算符>) = {PLUS, MINU}
 * FOLLOW(<表达式>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA} + {SEMICN}(<赋值语句新增>)
 * FIRST<加法运算符> 与 FOLLOW<表达式> 没有交集, 因此无回溯.
*/
PARSE_RETURN GrammaticalParser::__expression(PARSE_HEAD head) {
	FLAG_ENTER("<表达式>", head.level);
	try {
		// match +/- if have
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
		}

		RECUR_CHECK(__item);								// <项>
		while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
			RECUR_CHECK(__add_operator);
			RECUR_CHECK(__item);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}


/**
 * ＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
 * FIRST(<项>) = FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}

 * FIRST(<乘法运算符>) = {MULT, DIV}
 * FOLLOW(<项>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST 与 FOLLOW 交集，故不存在回溯
*/
PARSE_RETURN GrammaticalParser::__item(PARSE_HEAD head) {
	FLAG_ENTER("<项>", head.level);
	try {
		RECUR_CHECK(__factor);
		while (_peek()->equal(SYMBOL::MULT) || _peek()->equal(SYMBOL::DIV)) {
			RECUR_CHECK(__mult_operator);
			RECUR_CHECK(__factor);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜因子＞::= ＜标识符＞ ｜ ＜标识符＞'['＜表达式＞']' | '('＜表达式＞')' ｜ ＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞
 * FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * !: 因子的前两种选择不满足FIRST集合不相交的原则，改写为<标识符>[ '[' <表达式> ']' ]，
 *    并且FIRST([...]) = {LBRACK} 与 FOLLOW{<因子>} = {MULT, DIV} + FOLLOW(<项>)没有交集
*/
PARSE_RETURN GrammaticalParser::__factor(PARSE_HEAD head) {
	FLAG_ENTER("<因子>", head.level);
	try {
		// <标识符> [ '[' <表达式> ']' ] 
		if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
			SYMBOL_CHECK(SYMBOL::IDENFR);
			if (_peek()->equal(SYMBOL::LBRACK)) {
				SYMBOL_CHECK(SYMBOL::LBRACK);
				RECUR_CHECK(__expression);
				SYMBOL_CHECK(SYMBOL::RBRACK);
			}
		}
		// ＜有返回值函数调用语句＞  
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) {
			RECUR_CHECK(__function_call_return);
		}
		// '(' <表达式> ')'
		else if (_peek()->equal(SYMBOL::LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			RECUR_CHECK(__expression);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
		// <整数>
		else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
			RECUR_CHECK(__integer);
		}
		// <字符>
		else if (_peek()->equal(SYMBOL::CHARCON)) {
			RECUR_CHECK(__char);
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<factor>"));
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜语句＞::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'| ＜有返回值函数调用语句＞;
			 |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜返回语句＞;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / IDENFR / IDENFR / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(＜有返回值函数调用语句＞)与FISRT(<赋值语句>) 有重合，需要用_peek(2) = (
*/
PARSE_RETURN GrammaticalParser::__statement(PARSE_HEAD head)
{
	FLAG_ENTER("<语句>", head.level);
	try {
		SYMBOL loop_first[] = { SYMBOL::WHILETK, SYMBOL::FORTK, SYMBOL::DOTK };

		if (_peek()->equal(SYMBOL::IFTK)) { RECUR_CHECK(__condition_statement); }
		else if (_peek()->equal(loop_first, 3)) { RECUR_CHECK(__loop_statement); }
		else if (_peek()->equal(SYMBOL::LBRACE)) {
			SYMBOL_CHECK(SYMBOL::LBRACE);
			RECUR_CHECK(__statement_list)
			SYMBOL_CHECK(SYMBOL::RBRACE);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT))
		{
			// 有返回值函数调用与无返回值函数调用形式完全一致，需要提前使用符号表特判。
			vector<string>::iterator itr_return = func_call_return_idenfr.begin();
			vector<string>::iterator itr_void = func_call_void_idenfr.begin();
			int x = 0;
			while (itr_return != func_call_return_idenfr.end()) {
				if (*itr_return == _peek()->token) {
					x++;
					break;
				}
				itr_return++;
			}
			while (itr_void != func_call_void_idenfr.end()) {
				if (*itr_void == _peek()->token) {
					x--;
					break;
				}
				itr_void++;
			}
			if (x == 1) { RECUR_CHECK(__function_call_return) }
			else if (x == -1) { RECUR_CHECK(__function_call_void); }
			else { FLAG_FAIL; }	// 在类型为函数名的标识符中没有找到对应的。
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT))
		{
			RECUR_CHECK(__assign_statment);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::SCANFTK)) { RECUR_CHECK(__read_statement); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::PRINTFTK)) { RECUR_CHECK(__write_statement); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::SEMICN)) { SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::RETURNTK)) { RECUR_CHECK(__return_statement); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<statement>"));
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}


/**
 * ＜赋值语句＞::= ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
 * 规则改写 <赋值语句> ::= <标识符> ['[' <表达式> ']'] = <表达式>
 * FIRST(<赋值语句>) = {IDENFR}
*/
PARSE_RETURN GrammaticalParser::__assign_statment(PARSE_HEAD head) {
	FLAG_ENTER("<赋值语句>", head.level);
	
	try {
		SYMBOL_CHECK(SYMBOL::IDENFR);
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			RECUR_CHECK(__expression);
			SYMBOL_CHECK(SYMBOL::RBRACK);
		}
		SYMBOL_CHECK(SYMBOL::ASSIGN);					// ASSIGN: = ,  EQL: ==
		RECUR_CHECK(__expression);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
 * FISRT(<条件语句>) = {IFTK}
 *
 * FOLLOW(<条件语句>) = {RBRACE} // <语句> + <语句列> 的follow
 * FIRST(else <语句>) = {else}
 * 两个if+一个else的情况下文法层级解析存在歧义，此时需要人为规定优先级次序，进行贪婪匹配，else粘粘至最近的if即可。
*/
PARSE_RETURN GrammaticalParser::__condition_statement(PARSE_HEAD head) {
	FLAG_ENTER("<条件语句>", head.level);

	try {
		SYMBOL_CHECK(SYMBOL::IFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__condition);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		RECUR_CHECK(__statement);

		if (_peek()->equal(SYMBOL::ELSETK)) {
			SYMBOL_CHECK(SYMBOL::ELSETK);
			RECUR_CHECK(__statement);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜条件＞ ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
 * FIRST(<条件>) = FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * 规则改写：<条件> ::= <表达式>  [ <关系运算符> <表达式> ]

 * FOLLOW<条件> = {SIMICN, RPARENT}, 与FIRST(<关系运算符> <表达式>)无交集
 * FIRST(<关系运算符> <表达式>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
PARSE_RETURN GrammaticalParser::__condition(PARSE_HEAD head) {
	FLAG_ENTER("<条件>", head.level);

	try {
		RECUR_CHECK(__expression);

		if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
			_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
			RECUR_CHECK(__rel_operator);				// <关系运算符>
			RECUR_CHECK(__expression);				// <表达式>
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜循环语句＞::= while '('＜条件＞')'＜语句＞ |
				  do＜语句＞while '('＜条件＞')' |
				  for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
 * FISRT(<循环语句>) = {WHILETK, DOTK, FORTK}
 * FIRST(1) != FIRST(2) != FIRST(3)
*/
PARSE_RETURN GrammaticalParser::__loop_statement(PARSE_HEAD head)
{
	FLAG_ENTER("<循环语句>", head.level);
	try {
		// while while '('＜条件＞')'＜语句＞
		if (_peek()->equal(SYMBOL::WHILETK)) {
			SYMBOL_CHECK(SYMBOL::WHILETK);					// while
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__condition);							// <条件>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// ）
			RECUR_CHECK(__statement);							// <语句>
		}
		// do while do＜语句＞while '('＜条件＞')'
		else if (_peek()->equal(SYMBOL::DOTK)) {
			SYMBOL_CHECK(SYMBOL::DOTK);					// do
			RECUR_CHECK(__statement);							// <语句>
			SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>

			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__condition);							// <条件>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
		}
		// for: for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
		else if (_peek()->equal(SYMBOL::FORTK)) {
			SYMBOL_CHECK(SYMBOL::FORTK);					// for
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__idenfr);								// <标识符>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			RECUR_CHECK(__expression);							// <表达式>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			RECUR_CHECK(__condition);							// <条件>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			RECUR_CHECK(__idenfr);								// <标识符>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			RECUR_CHECK(__idenfr);								// <标识符>
			if (_peek()->equal(SYMBOL::PLUS)) {					// (+ | -)
				SYMBOL_CHECK(SYMBOL::PLUS);
			}
			else {
				SYMBOL_CHECK(SYMBOL::MINU);
			}
			RECUR_CHECK(__step_length);							// <步长>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
			RECUR_CHECK(__statement);							// <语句>
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("loop statement"));
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜步长＞::= ＜无符号整数＞
*/
PARSE_RETURN GrammaticalParser::__step_length(PARSE_HEAD head) {
	FLAG_ENTER("<步长>", head.level);
	try {
		RECUR_CHECK(__unsigned_integer);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜有返回值函数调用语句＞) = {<标识符>}
*/
PARSE_RETURN GrammaticalParser::__function_call_return(PARSE_HEAD head) {
	FLAG_ENTER("<有返回值函数调用语句>", head.level);

	try {
		RECUR_CHECK(__idenfr);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__value_parameter_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜无返回值函数调用语句＞) = {<标识符>}
*/
PARSE_RETURN GrammaticalParser::__function_call_void(PARSE_HEAD head) {
	FLAG_ENTER("<无返回值函数调用语句>", head.level);

	try {
		RECUR_CHECK(__idenfr);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__value_parameter_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞
 * FISRT() = FIRST(<表达式>)
 * FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * FOLLOW(<值参数表>) = {RPARENT}
 *
 * !: 也可以采用用FOLLOW判断空的情况，避免枚举FIRST遗漏
*/
PARSE_RETURN GrammaticalParser::__value_parameter_list(PARSE_HEAD head)
{
	FLAG_ENTER("<值参数表>", head.level);

	try {
		SYMBOL first_list[] = { SYMBOL::IDENFR, SYMBOL::LPARENT,
					 SYMBOL::PLUS, SYMBOL::MINU, SYMBOL::INTCON,
					 SYMBOL::CHARCON };

		// <表达式>
		if (_peek()->equal(first_list, 6)) {
			RECUR_CHECK(__expression);
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__expression);
			}
		}
		else {
			// <空> : ignore
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜语句列＞::=｛＜语句＞｝
 * FIRST(<语句>) =
 * {IFTK} + {WHILETK} + {DOTK} + {FORTK} + {LBRACE} + {IDENFR*3} + {IDENFR} + {SCANFTK} + {PRINTFTK}
 * + {SEMICN} + {RETURNTK}
 * FOLLOW(<语句列>) = {RBRACE}, 因此<空>选择与<语句>选择不存在回溯问题。
 *
 * !: 也可以采用判断follow集合的方式，避免枚举FISRT遗漏.
*/
PARSE_RETURN GrammaticalParser::__statement_list(PARSE_HEAD head) {
	FLAG_ENTER("<语句列>", head.level);

	try {
		SYMBOL list[] = {
			SYMBOL::IFTK,
			SYMBOL::WHILETK,
			SYMBOL::DOTK,
			SYMBOL::FORTK,
			SYMBOL::LBRACE,
			SYMBOL::IDENFR,
			SYMBOL::SCANFTK,
			SYMBOL::PRINTFTK,
			SYMBOL::SEMICN,
			SYMBOL::RETURNTK };
		while (_peek()->equal(list, 10)) {
			RECUR_CHECK(__statement);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜读语句＞::=  scanf '('＜标识符＞{,＜标识符＞}')'
 * FIRST(<读语句>) = {SCANFTK}
*/
PARSE_RETURN GrammaticalParser::__read_statement(PARSE_HEAD head) {
	FLAG_ENTER("<读语句>", head.level);

	try {
		SYMBOL_CHECK(SYMBOL::SCANFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__idenfr);
		while (_peek()->equal(SYMBOL::COMMA)) {
			SYMBOL_CHECK(SYMBOL::COMMA);
			RECUR_CHECK(__idenfr);
		}
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜写语句＞ ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
 * FISRT(<读语句>) = {PRINTFTK}
 * FISRT(<字符串>) = {STRCON},
 * FISRT(<表达式>) = {PLUS, MINU, FIRST(项)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 *
 * !: 在判断字符串与表达式走向时，也可以采用if-else型，非字符串即表达式。
*/
PARSE_RETURN GrammaticalParser::__write_statement(PARSE_HEAD head) {
	FLAG_ENTER("<写语句>", head.level);

	try {
		SYMBOL exp_first[] =
		{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
		  SYMBOL::INTCON, SYMBOL::CHARCON };

		SYMBOL_CHECK(SYMBOL::PRINTFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);

		// printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'
		if (_peek()->equal(SYMBOL::STRCON)) {
			RECUR_CHECK(__string);
			if (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__expression);
			}
		}
		// printf '('＜表达式＞')'
		else if (_peek()->equal(exp_first, 6)) {
			RECUR_CHECK(__expression);
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<write statement>"));
		}
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ＜返回语句＞::=  return['('＜表达式＞')']
 * FISRT('('＜表达式＞')') = {LPARENT};
 * FOLLOW{<返回语句>} = {SEMICN}，无交叉集合
*/
PARSE_RETURN GrammaticalParser::__return_statement(PARSE_HEAD head) {
	FLAG_ENTER("<返回语句>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::RETURNTK);
		if (_peek()->equal(LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			RECUR_CHECK(__expression);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}