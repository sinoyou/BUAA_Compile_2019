﻿#include "GrammaticalParser.h"
#include "utils.h"
#include "debug.h"
#include <list>

/* 初始化函数 */
GrammaticalParser::GrammaticalParser(
	vector<Token>& token_list, 
	vector<string>& output,
	vector<tuple<int, string>>& error_output)
	:token_list(token_list), output_list(output), error_output_list(error_output) {
	token = NULL;
	ptoken = -1;
	symbol_table = SymbolTable();
}

/* 入口函数 */
void GrammaticalParser::parse() {
	__program(1);
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
void GrammaticalParser::__add_operator(int level) {
	FLAG_ENTER("<加法运算符>",level);
	try {
		SYMBOL candidate[] = { SYMBOL::PLUS,SYMBOL::MINU };
		MULTI_SYMBOL_CHECK(candidate, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜乘法运算符＞  ::= *｜/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
void GrammaticalParser::__mult_operator(int level) {
	FLAG_ENTER("<乘法运算符>", level);
	try {
		SYMBOL candidate[] = { SYMBOL::MULT,SYMBOL::DIV };
		MULTI_SYMBOL_CHECK(candidate, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
 * FIRST：...
*/
void GrammaticalParser::__rel_operator(int level) {
	FLAG_ENTER("<关系运算符>", level);
	try {
		SYMBOL list[6] = { SYMBOL::LSS, SYMBOL::LEQ, SYMBOL::GRE, SYMBOL::GEQ, SYMBOL::NEQ, SYMBOL::EQL };
		MULTI_SYMBOL_CHECK(list, 6);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜字母＞::= ＿｜a｜．．．｜z｜A｜．．．｜Z
 * FIRST: ...
*/
void GrammaticalParser::__letter(int level) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<字母>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Char>"));
}

/**
 * ＜数字＞::= ０｜＜非零数字＞
 * FIRST: ...
*/
void GrammaticalParser::__number(int level) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<数字>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Number>"));
}

/**
 * ＜非零数字＞::= １｜．．．｜９
 * FIRST: ...
 */
void GrammaticalParser::__non_zero_number(int level) {
	// 词法分析充足，该非终结符被忽略
	FLAG_ENTER("<非零数字>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Non-Zero Number>"));
}

/**
 * ＜字符＞::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
 * FIRST: '
 * !: 词法分析时由于已经忽略了单引号'并判断了字符，因此该规则匹配不完全严格。
*/
char GrammaticalParser::__char(int level) {
	FLAG_ENTER("<字符>", level);
	try {
		SYMBOL_CHECK(SYMBOL::CHARCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return token->token[0];
}

/**
 * ＜字符串＞::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 * FIRST: "
 * !: 词法分析时由于已经忽略了双引号"并判断了字符，因此该规则匹配不完全严格.
*/
string GrammaticalParser::__string(int level) {
	FLAG_ENTER("<字符串>", level);
	try {
		SYMBOL_CHECK(SYMBOL::STRCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return token->token;
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
void GrammaticalParser::__program(int level)
{
	FLAG_ENTER("<程序>", level);
	symbol_table.add_one_block();												// 进入当前块
	Block* block = symbol_table.get_present_block();
	SymbolItem * global = SymbolFactory::create_function(block,
		"#",
		BasicType::_void, vector<SymbolItem*>());
	try {
		// <常量说明>
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			__const_description(level + 1);
		}

		// <变量说明>: 用反例判断，<变量说明>和<有返回值的函数定义>从后续第三个字符起出现差异
		if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			__var_description(level + 1);
		}

		// {＜有返回值函数定义＞ | ＜无返回值函数定义＞}
		// ＜无返回值函数定义＞需要排除void main
		while (
			(_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) ||
			(_peek()->equal(SYMBOL::VOIDTK) && !_peek(2)->equal(SYMBOL::MAINTK))
			)
		{
			if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
				__function_return(level + 1);
			}
			else {
				__function_void(level + 1);
			}
		}
		// <主函数>
		__main_function(level + 1);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	symbol_table.exit_present_block();												// 离开当前块
}

/**
 * ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
 * FISRT(<常量说明>) = {CONSTTK}
 * FOLLOW(<常量说明>) = {...}
 * 不存在FISRT(< const＜常量定义＞;>)和FOLLOW(<常量说明>)集合交叉情况。
*/
void GrammaticalParser::__const_description(int level)
{
	FLAG_ENTER("<常量说明>", level);
	try {
		SYMBOL_CHECK(SYMBOL::CONSTTK);
		__const_def(level + 1);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		while (_peek()->equal(SYMBOL::CONSTTK)) {
			SYMBOL_CHECK(SYMBOL::CONSTTK);
			__const_def(level + 1);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		};
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<常量定义>) = {SEMICN}
 * 不存在FOLLOW(<常量定义>)与FIRST(,＜标识符＞＝＜整数＞| <字符>) 相交情况。
*/
void GrammaticalParser::__const_def(int level)
{
	FLAG_ENTER("<常量定义>", level);
	Block* block = symbol_table.get_present_block();

	try {
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			BasicType type;
			string name;
			int const_value;
			if (_peek()->equal(SYMBOL::INTTK)) {
				SYMBOL_CHECK(SYMBOL::INTTK);
				type = BasicType::_int;
			}
			else {
				SYMBOL_CHECK(SYMBOL::CHARTK);
				type = BasicType::_char;
			}

			int cnt = 0;
			do {
				if (cnt ++ > 0)
					SYMBOL_CHECK(SYMBOL::COMMA);
				name = __idenfr(level + 1, true);
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				// unit4-error-o
				try {
					if (type == BasicType::_int)
						const_value = __integer(level + 1);
					else
						const_value = __char(level + 1);
					// 掺杂其他的东西
					if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
						while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
							_next();
						_register_error(token->line, ErrorType::ConstDefWrong);
					}
					// 正确无误后再输入表中
					SymbolItem* temp = 
						SymbolFactory::create_const(block, name, type, const_value);
					GetConstDeclarQuater(block, temp);
				}
				catch (ParseException& e) {
					_register_error(_peek()->line, ErrorType::ConstDefWrong);
					_next();
				}
			} while (_peek()->equal(SYMBOL::COMMA));
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
	
}

/**
 * ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝| 0
 * FIRST: ...
*/
int GrammaticalParser::__unsigned_integer(int level) {
	FLAG_ENTER("<无符号整数>", level);
	try {
		SYMBOL_CHECK(SYMBOL::INTCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return token->transnum();
}

/**
 * ＜整数＞::= ［＋｜－］＜无符号整数＞
 * FIRST(1) = {PLUS}, FIRST(2) = {MINU}, FIRST(3) = {INTCON}
*/
int GrammaticalParser::__integer(int level)
{
	FLAG_ENTER("<整数>", level);
	int number;
	int neg = 1;
	try {
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
			neg = -1;
		}
		number = __unsigned_integer(level + 1);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return number * neg;
}

/**
 * ＜标识符＞::=  ＜字母＞｛＜字母＞｜＜数字＞｝
 * FIRST: ...
*/
string GrammaticalParser::__idenfr(int level, bool is_def) {
	FLAG_ENTER("<标识符>", level);
	try {
		SYMBOL_CHECK(SYMBOL::IDENFR);
		// unit4error - b 
		// unit4error - c 
		void* p = find_indefr(symbol_table.get_present_block(), token->token);
		void* p1 = find_indefr_current(symbol_table.get_present_block(), token->token);
		if (is_def && p1 != NULL) {
			_register_error(token->line, ErrorType::Redefine);
		}
		if (!is_def && p == NULL) {
			_register_error(token->line, ErrorType::Undefine);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return token->token;
}

/**
 * ＜声明头部＞::=  int＜标识符＞ | char＜标识符＞
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
void GrammaticalParser::__declar_head(int level, BasicType* return_type, string* func_name) {
	FLAG_ENTER("<声明头部>", level);
	try {
		if (_peek()->equal(SYMBOL::INTTK))
		{
			SYMBOL_CHECK(SYMBOL::INTTK);
			*return_type = BasicType::_int;
			*func_name = __idenfr(level + 1, true);
		}
		else if (_peek()->equal(SYMBOL::CHARTK)) {
			SYMBOL_CHECK(SYMBOL::CHARTK);
			*return_type = BasicType::_char;
			*func_name = __idenfr(level + 1, true);
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
void GrammaticalParser::__var_description(int level)
{
	FLAG_ENTER("<变量说明>", level);
	try {
		SYMBOL fisrt_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		__var_def(level + 1);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		// 用反例修正仅参考FIRST集合的不足
		while (_peek()->equal(fisrt_list, 2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			__var_def(level + 1);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜变量定义＞::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )}
 * FOLLOW(<变量定义>) = {SEMICN}, FISRT(, <标识符>...) 与 FIRST(<'[' <无符号函数> ']'>)没有交集.
 * FOLLOW(<变量定义>) = {SEMICN} 与 FIRST(<, <标识符>>) 没有交集
 * * 单变量和数组变量的声明差异[]取或不取来表示
*/
void GrammaticalParser::__var_def(int level)
{
	FLAG_ENTER("<变量定义>", level);
	Block* block = symbol_table.get_present_block();
	try {
		BasicType type = __type_idenfr(level + 1);
		int cnt = 0;
		// {} 
		do {
			if(cnt ++ > 0)
				SYMBOL_CHECK(SYMBOL::COMMA);
			string idenfr_name = __idenfr(level + 1, true);
			bool is_array = false;
			// '[' <无符号整数> ']'
			int size = 1;
			if (_peek()->equal(SYMBOL::LBRACK)) {
				SYMBOL_CHECK(SYMBOL::LBRACK);
				size = __unsigned_integer(level + 1);
				SYMBOL_CHECK(SYMBOL::RBRACK);
				is_array = true;
			}
			SymbolItem* temp = 
				SymbolFactory::create_variable(block, idenfr_name, type, size, is_array);
			GetVarDeclarQuater(block, temp);
		} while (_peek()->equal(SYMBOL::COMMA));
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}


/**
 * ＜类型标识符＞::=  int | char
*/
BasicType GrammaticalParser::__type_idenfr(int level) {
	FLAG_ENTER("<类型标识符>", level);
	try {
		SYMBOL list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		MULTI_SYMBOL_CHECK(list, 2);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	if (token->equal(SYMBOL::INTTK)) {
		return BasicType::_int;
	}
	else {
		return BasicType::_char;
	}
}

/**
 * ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
*/
void GrammaticalParser::__function_return(int level) {
	FLAG_ENTER("<有返回值函数定义>", level);
	symbol_table.add_one_block();												// 进入当前块
	Block* block = symbol_table.get_present_block();
	try {
		BasicType return_type;
		string func_name;
		vector<SymbolItem*> para_list;

		__declar_head(level + 1, &return_type, &func_name);

		SYMBOL_CHECK(SYMBOL::LPARENT);
		__parameter_list(level + 1, &para_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
		// 函数头声明完毕，可以更新函数的SymbolItem部件了.
		SymbolItem* func_head = 
			SymbolFactory::create_function(symbol_table.get_present_block(), func_name, return_type, para_list);
		GetFuncDeclarQuater(block, func_head);
		for (auto it = para_list.begin(); it != para_list.end(); it++) {
			GetFuncParaDeclarQuater(block, *it);
		}
		
		SYMBOL_CHECK(SYMBOL::LBRACE);
		bool has_return = false;
		__compound_statement(level + 1, &has_return);

		// RECUR_CHECK(__compound_statement, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::RBRACE);
		// unit4-error h
		if (!has_return) {
			_register_error(token->line, ErrorType::ReturnError);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;

	symbol_table.exit_present_block();											// 离开当前块
}

/**
 * ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
*/
void GrammaticalParser::__function_void(int level) {
	FLAG_ENTER("<无返回值函数定义>", level);

	symbol_table.add_one_block();												// 进入当前块
	Block* block = symbol_table.get_present_block();
	try {
		BasicType return_type = BasicType::_void;
		string func_name;
		vector<SymbolItem*> para_list;

		SYMBOL_CHECK(SYMBOL::VOIDTK);
		func_name = __idenfr(level + 1, true);

		SYMBOL_CHECK(SYMBOL::LPARENT);
		__parameter_list(level + 1, &para_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
		// 函数头声明完毕，可以更新函数的SymbolItem部件了.
		SymbolItem * func_head = 
			SymbolFactory::create_function(symbol_table.get_present_block(), func_name, return_type, para_list);
		GetFuncDeclarQuater(block, func_head);
		for (auto it = para_list.begin(); it != para_list.end(); it++) {
			GetFuncParaDeclarQuater(block, *it);
		}

		SYMBOL_CHECK(SYMBOL::LBRACE);
		bool has_return = false;
		__compound_statement(level + 1, &has_return);
		GetFuncRetQuater(block, NULL);							// 无返回值函数强行增加一个return

		// RECUR_CHECK(__compound_statement, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::RBRACE);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;

	symbol_table.exit_present_block();											// 离开当前块
}

/**
 * ＜复合语句＞::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
 * FISRT(<常量说明>) = {CONSTK}， FIRST(<变量说明>) = {FIRST(<变量定义>)} = {INTTK, CHARTK}
 * FOLLOW(...<[变量说明]>) = FISRT(<语句列>) 与 FISRT(<变量说明>) 没有交集
*/
void GrammaticalParser::__compound_statement(int level, bool *has_return) {
	FLAG_ENTER("<复合语句>", level);
	try {
		SYMBOL first_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		// [<const des>]
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			__const_description(level + 1);
		}
		// [<var des>]
		if (_peek()->equal(first_list, 2)) {
			__var_description(level + 1);
		}
		__statement_list(level + 1, has_return);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜参数表＞::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞} | ＜空＞
 * 不带回溯的文法分析方式：1.两种选择的FIRST集合不能有交集。 2.至多一个空。 3.导出空的选择，非空导出退出的序列中不能含有FOLLOW(<参数表>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(参数表) =  {RPARENT}
 * FOLLOW(<标识符>) = {RPARENT} 与 FIRST(<,<类型标识符><标识符>>) 无交集
*/
void GrammaticalParser::__parameter_list(int level, vector<SymbolItem*>* para_list)
{
	FLAG_ENTER("<参数表>", level);

	try {
		// ~<空>
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			int cnt = 0;
			do
			{
				if(cnt ++ > 0)
					SYMBOL_CHECK(SYMBOL::COMMA);
				BasicType type = __type_idenfr(level + 1);
				string name = __idenfr(level + 1, true);
				// 插入类型表示
				SymbolItem* para = SymbolFactory::create_variable(symbol_table.get_present_block(),name, type, 1, false);
				para_list->push_back(para);
			} while (_peek()->equal(SYMBOL::COMMA));
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
	
}

/**
 * ＜主函数＞::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 * FIRST(x): {VOIDTK}
*/
void GrammaticalParser::__main_function(int level)
{
	FLAG_ENTER("<主函数>", level);
	symbol_table.add_one_block();												// 进入当前块
	SymbolItem* main = SymbolFactory::create_function(symbol_table.get_present_block(),
		"main", BasicType::_void, vector<SymbolItem*>());
	Block* block = symbol_table.get_present_block();
	GetFuncDeclarQuater(block, main);
	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);				// void
		SYMBOL_CHECK(SYMBOL::MAINTK);				// main
		SYMBOL_CHECK(SYMBOL::LPARENT);				// (
		SYMBOL_CHECK(SYMBOL::RPARENT);				// )
		SYMBOL_CHECK(SYMBOL::LBRACE);				// {
		bool has_return = false;
		__compound_statement(level + 1, &has_return);
		GetFuncRetQuater(block, NULL);
		SYMBOL_CHECK(SYMBOL::RBRACE);				// }
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	symbol_table.exit_present_block();
	
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
SymbolItem* GrammaticalParser::__expression(int level, bool* is_char) {
	FLAG_ENTER("<表达式>", level);
	int cnt = 0;
	bool neg = false;
	SymbolItem* first_item;
	Block* block = symbol_table.get_present_block();
	try {
		// match +/- if have
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
			neg = true;
		}

		bool temp = false;
		first_item = __item(level + 1, &temp);
		// 取负操作
		if (neg) {
			// 取反操作 -'c' 涉及运算操作，因此需要转换为整数
			SymbolItem* temp_neg = SymbolFactory::create_temp(block, BasicType::_int);
			GetSubQuater(block, NULL, first_item, temp_neg);
			first_item = temp_neg;
		}
		cnt++;
		while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
			Token* op = _peek();
			__add_operator(level + 1);
			SymbolItem* second_item = __item(level + 1, NULL);
			cnt++;

			// 根据操作符的差异，生成不同的中间式 - begin
			SymbolItem* temp_item = SymbolFactory::create_temp(block, BasicType::_int);
			if (op->equal(SYMBOL::PLUS))
				GetAddQuater(block, first_item, second_item, temp_item);
			else
				GetSubQuater(block, first_item, second_item, temp_item);
			first_item = temp_item;
			// end
		}
		// 表达式只有一个项且这个项是char，那么表达式就是char型
		if (cnt == 1 && temp && is_char != NULL)
			* is_char = true;
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return first_item;
}


/**
 * ＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
 * FIRST(<项>) = FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}

 * FIRST(<乘法运算符>) = {MULT, DIV}
 * FOLLOW(<项>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST 与 FOLLOW 交集，故不存在回溯
*/
SymbolItem* GrammaticalParser::__item(int level, bool* is_char) {
	FLAG_ENTER("<项>", level);
	int cnt = 0;
	Block* block = symbol_table.get_present_block();
	SymbolItem* first_factor;
	try {
		bool temp = false;
		first_factor = __factor(level + 1, &temp);
		cnt++;
		while (_peek()->equal(SYMBOL::MULT) || _peek()->equal(SYMBOL::DIV)) {
			Token* op = _peek();
			__mult_operator(level + 1);
			SymbolItem* secord_factor = __factor(level + 1, NULL);
			cnt++;

			// 根据操作符，生成相应中间代码
			SymbolItem* temp = SymbolFactory::create_temp(block, BasicType::_int);
			if (op->equal(SYMBOL::MULT))
				GetMultQuater(block, first_factor, secord_factor, temp);
			else
				GetDivQuater(block, first_factor, secord_factor, temp);
			first_factor = temp;
		}
		// 若果只有一个因子并且因子为char，则为char
		if (cnt == 1 && temp && is_char) {
			*is_char = true;
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return first_factor;
}

/**
 * ＜因子＞::= ＜标识符＞ ｜ ＜标识符＞'['＜表达式＞']' | '('＜表达式＞')' ｜ ＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞
 * FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * !: 因子的前两种选择不满足FIRST集合不相交的原则，改写为<标识符>[ '[' <表达式> ']' ]，
 *    并且FIRST([...]) = {LBRACK} 与 FOLLOW{<因子>} = {MULT, DIV} + FOLLOW(<项>)没有交集
*/
SymbolItem* GrammaticalParser::__factor(int level, bool *is_char) {
	FLAG_ENTER("<因子>", level);
	Block* block = symbol_table.get_present_block();
	SymbolItem* factor;
	try {
		// <标识符> [ '[' <表达式> ']' ] 
		if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
			string name = __idenfr(level + 1, false);
			// 在符号表中查找变量类型
			SymbolItem* p = find_const_var(symbol_table.get_present_block(), name, true);
			if (p != NULL) {
				if (p->basic_type == BasicType::_char && is_char != NULL) {
					*is_char = true;
				}
			}
			// 数组类型
			if (_peek()->equal(SYMBOL::LBRACK)) {
				bool char_detector = false;
				SYMBOL_CHECK(SYMBOL::LBRACK);
				SymbolItem* index = __expression(level + 1, &char_detector);
				// unit4-error-i
				if (char_detector) {
					_register_error(token->line, ErrorType::ArrayIndexWrong);
				}
				SYMBOL_CHECK(SYMBOL::RBRACK);
				factor = SymbolFactory::create_temp(block, p->basic_type);
				GetArrayQueryQuater(block, p, index, factor);
			}
			// 非数组类型
			else {
				factor = p;
			}
		}
		// ＜有返回值函数调用语句＞  
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) {
			string name = _peek()->token;
			// 在符号表中查找函数类型
			BasicType type = BasicType::_int;
			SymbolItem* p = find_func(symbol_table.get_present_block(), name, true);
			if (p != NULL) {
				if (p->basic_type == BasicType::_char && is_char != NULL) {
					*is_char = true;
					type = BasicType::_char;
				}
			}
			factor = __function_call_return(level + 1);
		}
		// '(' <表达式> ')'
		else if (_peek()->equal(SYMBOL::LPARENT)) {
			// 由于表达式周围存在()括号，因此char必须强行转换为int.
			SYMBOL_CHECK(SYMBOL::LPARENT);
			SymbolItem* temp = __expression(level + 1, NULL);
			factor = SymbolFactory::create_temp(block, BasicType::_int);
			GetAssignQuater(block, temp, factor);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
		// <整数>
		else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
			int value = __integer(level + 1);
			factor = SymbolFactory::create_temp_const(block, BasicType::_int, value);
		}
		// <字符>
		else if (_peek()->equal(SYMBOL::CHARCON)) {
			char c = __char(level + 1);
			if (is_char) {
				*is_char = true;
			}
			factor = SymbolFactory::create_temp_const(block, BasicType::_char, c);
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
	return factor;
}

/**
 * ＜语句＞::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'| ＜有返回值函数调用语句＞;
			 |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜返回语句＞;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / IDENFR / IDENFR / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(＜有返回值函数调用语句＞)与FISRT(<赋值语句>) 有重合，需要用_peek(2) = (
*/
void GrammaticalParser::__statement(int level, bool * has_return)
{
	FLAG_ENTER("<语句>", level);
	Block* block = symbol_table.get_present_block();
	try {
		SYMBOL loop_first[] = { SYMBOL::WHILETK, SYMBOL::FORTK, SYMBOL::DOTK };

		if (_peek()->equal(SYMBOL::IFTK)) { __condition_statement(level + 1, has_return); }
		else if (_peek()->equal(loop_first, 3)) {
			__loop_statement(level + 1, has_return);
		}
		else if (_peek()->equal(SYMBOL::LBRACE)) {
			SYMBOL_CHECK(SYMBOL::LBRACE);
			__statement_list(level + 1, has_return);
			SYMBOL_CHECK(SYMBOL::RBRACE);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT))
		{
			string peek_func_name = _peek()->token;
			SymbolItem* found = find_func(symbol_table.get_present_block(), peek_func_name, true);
			// 有返回值函数调用与无返回值函数调用形式完全一致，需要提前使用符号表特判。
			if (found != NULL && found->basic_type != BasicType::_void) { __function_call_return(level + 1); }
			else if (found != NULL && found->basic_type == BasicType::_void) { __function_call_void(level + 1); }
			else { 
				_register_error(_peek()->line, ErrorType::Undefine);
				while (!_peek()->equal(SYMBOL::SEMICN))
					_next();
			}	// 在类型为函数名的标识符中没有找到对应的。
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT))
		{
			__assign_statment(level + 1);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::SCANFTK)) { __read_statement(level + 1); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::PRINTFTK)) { __write_statement(level + 1); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::SEMICN)) { SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::RETURNTK)) { 
			// RECUR_CHECK(__return_statement, RECUR_DEFAULT);
			__return_statement(level + 1, has_return);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<statement>"));
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}


/**
 * ＜赋值语句＞::= ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
 * 规则改写 <赋值语句> ::= <标识符> ['[' <表达式> ']'] = <表达式>
 * FIRST(<赋值语句>) = {IDENFR}
*/
void GrammaticalParser::__assign_statment(int level) {
	FLAG_ENTER("<赋值语句>", level);
	Block* block = symbol_table.get_present_block();
	
	try {
		string name = __idenfr(level + 1 ,false);
		SymbolItem* p = find_const_var(symbol_table.get_present_block(), name, true);
		if (p!=NULL) {
			if (p->type == SymbolItemType::_const) {
				_register_error(token->line, ErrorType::ConstModify);
			}
		}
		SymbolItem* index = NULL;
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			bool char_detector = false;
			index = __expression(level + 1, &char_detector);
			// unit4-error-i
			if (char_detector) {
				_register_error(token->line, ErrorType::ArrayIndexWrong);
			}
			SYMBOL_CHECK(SYMBOL::RBRACK);
		}
		SYMBOL_CHECK(SYMBOL::ASSIGN);					// ASSIGN: = ,  EQL: ==
		SymbolItem* exp = __expression(level + 1, NULL);
		if (index)
			GetAssignArrayQuater(block, exp, index, p);
		else
			GetAssignQuater(block, exp, p);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
 * FISRT(<条件语句>) = {IFTK}
 *
 * FOLLOW(<条件语句>) = {RBRACE} // <语句> + <语句列> 的follow
 * FIRST(else <语句>) = {else}
 * 两个if+一个else的情况下文法层级解析存在歧义，此时需要人为规定优先级次序，进行贪婪匹配，else粘粘至最近的if即可。
*/
void GrammaticalParser::__condition_statement(int level, bool* has_return) {
	FLAG_ENTER("<条件语句>", level);
	Block* block = symbol_table.get_present_block();
	SymbolItem* else_label = SymbolFactory::create_label(block, "else");
	SymbolItem* if_end_label = SymbolFactory::create_label(block, "endif");

	try {
		SYMBOL_CHECK(SYMBOL::IFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		SymbolItem* condition = __condition(level + 1);
		GetBzQuater(block, else_label, condition);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		__statement(level + 1, has_return);

		GetGotoQuater(block, if_end_label);
		GetSetLabelQuater(block, else_label);

		if (_peek()->equal(SYMBOL::ELSETK)) {
			SYMBOL_CHECK(SYMBOL::ELSETK);
			__statement(level + 1, has_return);
		}

		GetSetLabelQuater(block, if_end_label);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜条件＞ ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
 * FIRST(<条件>) = FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * 规则改写：<条件> ::= <表达式>  [ <关系运算符> <表达式> ]

 * FOLLOW<条件> = {SIMICN, RPARENT}, 与FIRST(<关系运算符> <表达式>)无交集
 * FIRST(<关系运算符> <表达式>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
SymbolItem* GrammaticalParser::__condition(int level) {
	FLAG_ENTER("<条件>", level);
	Block* block = symbol_table.get_present_block();
	SymbolItem* condition;
	try {
		bool char_detector = false;
		SymbolItem * exp1 = __expression(level + 1, &char_detector);
		SymbolItem* exp2 = NULL;

		Token* t = NULL;
		if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
			_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
			t = _peek();
			__rel_operator(level + 1);														// <关系运算符>
			exp2 = __expression(level + 1, &char_detector);					// <表达式>
		}
		// uni4-error-f
		if (char_detector) {
			_register_error(token->line, ErrorType::ConditionError);
		}

		// 生成中间代码
		if (t == NULL) {
			condition = exp1;
		}
		else {
			condition = SymbolFactory::create_temp(block, BasicType::_int);
			if (t->equal(SYMBOL::GRE))
				GetGtCmpQuater(block, exp1, exp2, condition);
			else if (t->equal(SYMBOL::GEQ))
				GetGeqCmpQuater(block, exp1, exp2, condition);
			else if (t->equal(SYMBOL::LSS))
				GetLtCmpQuater(block, exp1, exp2, condition);
			else if (t->equal(SYMBOL::LEQ))
				GetLeqCmpQuater(block, exp1, exp2, condition);
			else if (t->equal(SYMBOL::NEQ))
				GetNeqCmpQuater(block, exp1, exp2, condition);
			else if (t->equal(SYMBOL::EQL))
				GetEqlCmpQuater(block, exp1, exp2, condition);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return condition;
}

/**
 * ＜循环语句＞::= while '('＜条件＞')'＜语句＞ |
				  do＜语句＞while '('＜条件＞')' |
				  for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
 * FISRT(<循环语句>) = {WHILETK, DOTK, FORTK}
 * FIRST(1) != FIRST(2) != FIRST(3)
*/
void GrammaticalParser::__loop_statement(int level, bool* has_return)
{
	FLAG_ENTER("<循环语句>", level);
	Block* block = symbol_table.get_present_block();
	try {
		// while while '('＜条件＞')'＜语句＞
		if (_peek()->equal(SYMBOL::WHILETK)) {
			block->register_statement(StatementType::while_stat);
			SymbolItem* head = SymbolFactory::create_label(block, "while_head");
			SymbolItem* tail = SymbolFactory::create_label(block, "while_end");
			SYMBOL_CHECK(SYMBOL::WHILETK);											// while
			SYMBOL_CHECK(SYMBOL::LPARENT);											// (
			GetSetLabelQuater(block, head);
			SymbolItem* condition = __condition(level + 1);							// <条件>
			GetBzQuater(block, tail, condition);
			SYMBOL_CHECK(SYMBOL::RPARENT);											// ）
			__statement(level +1 , has_return);
			GetGotoQuater(block, head);
			GetSetLabelQuater(block, tail);
		}
		// do while do＜语句＞while '('＜条件＞')'
		else if (_peek()->equal(SYMBOL::DOTK)) {
			block->register_statement(StatementType::dowhile_stat);
			SymbolItem* head = SymbolFactory::create_label(block, "dowhile_head");
			SymbolItem* tail = SymbolFactory::create_label(block, "dowhile_end");
			GetSetLabelQuater(block, head);
			SYMBOL_CHECK(SYMBOL::DOTK);							// do
			__statement(level + 1, has_return);
			try {
				SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>
			}
			catch (ParseException& e) {
				_register_error(token->line, ErrorType::MissWhile);
			}

			SYMBOL_CHECK(SYMBOL::LPARENT);								// (
			SymbolItem* condition  = __condition(level + 1);			// <条件>
			SYMBOL_CHECK(SYMBOL::RPARENT);								// )
			GetBnzQuater(block, head, condition);
			GetSetLabelQuater(block, tail);
		}
		// for: for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
		else if (_peek()->equal(SYMBOL::FORTK)) {
			block->register_statement(StatementType::for_stat);
			SymbolItem* initial = SymbolFactory::create_label(block, "for_initial");
			SymbolItem* judge = SymbolFactory::create_label(block, "for_judge");
			SymbolItem* update = SymbolFactory::create_label(block, "for_update");
			SymbolItem* body = SymbolFactory::create_label(block, "for_body");
			SymbolItem* tail = SymbolFactory::create_label(block, "for_tail");
			
			// 初始化部分
			SYMBOL_CHECK(SYMBOL::FORTK);					// for
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			GetSetLabelQuater(block, initial);
			string name = __idenfr(level + 1, false);		// <标识符>
			SymbolItem* var = find_const_var(block, name, true);
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			SymbolItem* exp = __expression(level + 1, NULL);// <表达式>
			GetAssignQuater(block, exp, var);
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			
			// 判断部分
			GetSetLabelQuater(block, judge);
			SymbolItem * condition = __condition(level + 1);		// <条件>
			SYMBOL_CHECK(SYMBOL::SEMICN);							// ;
			GetBzQuater(block, tail, condition);
			
			// 更新部分 - （源代码解析）
			name = __idenfr(level + 1, false);								// <标识符>
			SymbolItem* step_counter = find_const_var(block, name, true);
			SYMBOL_CHECK(SYMBOL::ASSIGN);									// =
			name = __idenfr(level + 1, false);								// <标识符>
			SymbolItem* step_updater = find_const_var(block, name, true);
			SYMBOL update_op;
			if (_peek()->equal(SYMBOL::PLUS)) {								// (+ | -)
				SYMBOL_CHECK(SYMBOL::PLUS);
				update_op = SYMBOL::PLUS;
			}
			else {
				SYMBOL_CHECK(SYMBOL::MINU);
				update_op = SYMBOL::MINU;
			}
			int value = __step_length(level + 1);							// <步长>
			SymbolItem* step_len = SymbolFactory::create_temp_const(block, BasicType::_int, value);
			SYMBOL_CHECK(SYMBOL::RPARENT);									// )
			
			// 主干部分
			GetSetLabelQuater(block, body);
			__statement(level + 1, has_return);

			// 更新部分 - （中间码）
			GetSetLabelQuater(block, update);
			if (update_op == SYMBOL::PLUS)
				GetAddQuater(block, step_updater, step_len, step_counter);
			else 
				GetSubQuater(block, step_updater, step_len, step_counter);
			GetGotoQuater(block, judge);

			// 循环尾部
			GetSetLabelQuater(block, tail);
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
	
}

/**
 * ＜步长＞::= ＜无符号整数＞
*/
int GrammaticalParser::__step_length(int level) {
	FLAG_ENTER("<步长>", level);
	int value;
	try {
		value =  __unsigned_integer(level + 1);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return value;
}

/**
 * ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜有返回值函数调用语句＞) = {<标识符>}
*/
SymbolItem* GrammaticalParser::__function_call_return(int level) {
	FLAG_ENTER("<有返回值函数调用语句>", level);
	Block* block = symbol_table.get_present_block();
	SymbolItem* ret=NULL;
	try {
		string name = __idenfr(level + 1, false);
		SymbolItem* p = find_func(symbol_table.get_present_block(), name, true);
		// 如果没有的话，直接忽略这句话
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			// 定义vector容器，用于存储所有表示函数参数的temp，必须要统一push。
			vector<SymbolItem*> paras_store;
			__value_parameter_list(level+1 , &(p->paramsList), &paras_store);
			for (auto it = paras_store.begin(); it != paras_store.end(); it++)
				GetFuncParaPushQuater(block, *it);
			SYMBOL_CHECK(SYMBOL::RPARENT);
			GetFuncCallQuater(block, p);
			ret = SymbolFactory::create_temp(block, p->basic_type);
			GetAssignRetQuater(block, ret);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return ret;
}

/**
 * ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜无返回值函数调用语句＞) = {<标识符>}
*/
void GrammaticalParser::__function_call_void(int level) {
	FLAG_ENTER("<无返回值函数调用语句>", level);

	Block* block = symbol_table.get_present_block();

	try {
		string name = __idenfr(level + 1, false);
		SymbolItem* p = find_func(symbol_table.get_present_block(), name, true);
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			// 定义vector容器，用于存储所有表示函数参数的temp，必须要统一push。
			vector<SymbolItem*> paras_store;
			__value_parameter_list(level + 1, &(p->paramsList), &paras_store);
			for (auto it = paras_store.begin(); it != paras_store.end(); it++)
				GetFuncParaPushQuater(block, *it);
			SYMBOL_CHECK(SYMBOL::RPARENT);
			GetFuncCallQuater(block, p);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞
 * FISRT() = FIRST(<表达式>)
 * FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * FOLLOW(<值参数表>) = {RPARENT}
 *
 * !: 也可以采用用FOLLOW判断空的情况，避免枚举FIRST遗漏
*/
void GrammaticalParser::__value_parameter_list(int level, vector<SymbolItem*>* params, vector<SymbolItem*>* params_exp)
{
	FLAG_ENTER("<值参数表>", level);

	Block* block = symbol_table.get_present_block();

	try {
		SYMBOL first_list[] = { SYMBOL::IDENFR, SYMBOL::LPARENT,
					 SYMBOL::PLUS, SYMBOL::MINU, SYMBOL::INTCON,
					 SYMBOL::CHARCON };

		// <表达式>
		int cnt = 0;
		bool char_detector = false;
		vector<string> temp;
		if (_peek()->equal(first_list, 6)) {
			do
			{
				if(cnt ++ > 0)
					SYMBOL_CHECK(SYMBOL::COMMA);
				char_detector = false;
				SymbolItem* exp = __expression(level + 1, &char_detector);
				params_exp->push_back(exp);
				// GetFuncParaPushQuater(block, exp);
				temp.push_back((char_detector ? "char" : "int"));
			} while (_peek()->equal(SYMBOL::COMMA));
		}
		else {
			// <空> : ignore
		}
		// unit4-error-d
		if (cnt != params->size()) {
			_register_error(token->line, ErrorType::FuncParamNumUnmatch);
		}
		// unit4-error-e
		auto it1 = temp.begin();
		auto it2 = params->begin();
		for ( ; it1 != temp.end() && it2 != params->end(); it1++, it2++) {
			string std = (*it2)->basic_type == BasicType::_char ? "char" : "int";
			if (std != *it1) {
				_register_error(token->line, ErrorType::FuncParamTypeUnmatch);
				break;
			}
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
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
void GrammaticalParser::__statement_list(int level, bool * has_return) {
	FLAG_ENTER("<语句列>", level);

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
			// RECUR_CHECK(__statement, RECUR_DEFAULT);
			__statement(level + 1, has_return);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ＜读语句＞::=  scanf '('＜标识符＞{,＜标识符＞}')'
 * FIRST(<读语句>) = {SCANFTK}
*/
void GrammaticalParser::__read_statement(int level) {
	FLAG_ENTER("<读语句>", level);
	Block* block = symbol_table.get_present_block();

	try {
		SYMBOL_CHECK(SYMBOL::SCANFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		int cnt = 0;
		do
		{
			if(cnt ++ > 0)
				SYMBOL_CHECK(SYMBOL::COMMA);
			string name = __idenfr(level + 1, false);
			SymbolItem* temp = find_const_var(block, name, true);
			GetScanQuater(block, temp);
		} while (_peek()->equal(SYMBOL::COMMA));
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
}

/**
 * ＜写语句＞ ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
 * FISRT(<读语句>) = {PRINTFTK}
 * FISRT(<字符串>) = {STRCON},
 * FISRT(<表达式>) = {PLUS, MINU, FIRST(项)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 *
 * !: 在判断字符串与表达式走向时，也可以采用if-else型，非字符串即表达式。
*/
void GrammaticalParser::__write_statement(int level) {
	FLAG_ENTER("<写语句>", level);
	Block* block = symbol_table.get_present_block();
	SymbolItem* newline = SymbolFactory::create_temp_const(block, BasicType::_char, '\n');
	try {
		SYMBOL exp_first[] =
		{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
		  SYMBOL::INTCON, SYMBOL::CHARCON };

		SYMBOL_CHECK(SYMBOL::PRINTFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);

		// printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'
		if (_peek()->equal(SYMBOL::STRCON)) {
			string strcon = __string(level + 1);
			SymbolItem* strcon_temp = SymbolFactory::create_temp_string(block, strcon);
			GetPrintQuater(block, strcon_temp);
			if (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				SymbolItem* exp = __expression(level + 1, NULL);
				GetPrintQuater(block, exp);
			}
		}
		// printf '('＜表达式＞')'
		else if (_peek()->equal(exp_first, 6)) {
			SymbolItem * exp = __expression(level + 1, NULL);
			GetPrintQuater(block, exp);
		}
		else {
			throw ParseException(ParseExceptionType::UnknownBranch, string("<write statement>"));
		}
		SYMBOL_CHECK(SYMBOL::RPARENT);
		GetPrintQuater(block, newline);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ＜返回语句＞::=  return['('＜表达式＞')']
 * FISRT('('＜表达式＞')') = {LPARENT};
 * FOLLOW{<返回语句>} = {SEMICN}，无交叉集合
*/
void GrammaticalParser::__return_statement(int level, bool * has_return) {
	FLAG_ENTER("<返回语句>", level);

	try {
		SYMBOL_CHECK(SYMBOL::RETURNTK);
		Block* block = symbol_table.get_present_block();
		*has_return = true;
		if (_peek()->equal(LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			bool char_detector = false;
			SymbolItem* exp = __expression(level + 1, &char_detector);
			// unit4-error-h
			if (block->func_head->basic_type == BasicType::_int && char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			else if (block->func_head->basic_type == BasicType::_char && !char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			else if (block->func_head->basic_type == BasicType::_void) {
				_register_error(token->line, ErrorType::VoidWithReturn);
			}
			SYMBOL_CHECK(SYMBOL::RPARENT);
			GetFuncRetQuater(block, exp);
		}
		else {
			if (block->func_head->basic_type == BasicType::_int || block->func_head->basic_type == BasicType::_char)
			{
				_register_error(token->line, ErrorType::ReturnError);
			}
			GetFuncRetQuater(block, NULL);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
}