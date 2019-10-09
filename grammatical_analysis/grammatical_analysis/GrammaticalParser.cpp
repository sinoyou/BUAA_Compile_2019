#include "GrammaticalParser.h"

#define DEBUG_PRINT(msg) fprintf(stderr, "%s %s - line: %d, file : %s %s \n", __DATE__, __TIME__, __LINE__, __FILE__, msg); 

#define BACKUP _backup();

// FLAG_FAIL : 假设当前token是需要判断的token
#define FLAG_FAIL \
{										\
	DEBUG_PRINT("ERROR WHEN PARSING")	\
	_recover();							\
	return -1;							\
}

// FLAG_PASS ： 需要读入一个新的token进行判断
#define FLAG_PASS						\
{										\
	_next();							\
	return 0;							\
}

#define FLAG_SYMBOL_CHECK(symbol)  \
{										\
	if (!token->equal(symbol))			\
		{FLAG_FAIL;}					\
}

#define FLAG_RECUR(func) \
{										\
	_next();							\
	int x = func();						\
	if(x != 0)							\
	{									\
		DEBUG_PRINT("ERROR WHEN RECUR");\
		FLAG_FAIL;						\
	}									\
}

/* 支持函数 */
void _error(const char* s);			// 错误输出处理
Token _gettoken_index(int index);	// 根据索引获取一个单词
void _gettoken();					// 获取一个单词
void _retract();					// 回退一个单词

/* 初始化函数 */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = 0;
}

/* 支持函数 */
void GrammaticalParser::_error(const char * s) {
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
void GrammaticalParser::_next()
{
	ptoken++;
	_update_token();
}
void GrammaticalParser::_retract()
{
	ptoken--;
	_update_token();
}
void GrammaticalParser::_backup() {
	// ptoken备份
	ptoken_record.push_back(ptoken);
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
 */

/**
 * ＜加法运算符＞ ::= +｜-
 * FIRST(+) = {PLUS}, FIRST(-) = {SUB}
 */
int GrammaticalParser::__add_operator() {
	BACKUP;
	if (token->equal(SYMBOL::PLUS) || token->equal(SYMBOL::MINU)) {
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}

/**
 * ＜乘法运算符＞  ::= *｜/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
int GrammaticalParser::__mult_operator() {
	BACKUP;
	if (token->equal(SYMBOL::MULT) || token->equal(SYMBOL::DIV)) {
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}

/**
 * ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
 * FIRST：...
*/
int GrammaticalParser::__rel_operator() {
	BACKUP;
	SYMBOL list[6] = {SYMBOL::LSS, SYMBOL::LEQ, SYMBOL::GRE, SYMBOL::GEQ, SYMBOL::NEQ, SYMBOL::EQL};
	int x = 0;
	for (int i = 0; i < 6; i++)
		if (token->equal(list[i]))
			x = 1;
	if (x == 1) {
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}

/**
 * ＜字母＞::= ＿｜a｜．．．｜z｜A｜．．．｜Z
 * FIRST: ...
*/
int GrammaticalParser::__letter() {
	// 词法分析充足，该非终结符被忽略
	BACKUP;
	FLAG_FAIL;
}

/**
 * ＜数字＞::= ０｜＜非零数字＞
 * FIRST: ...
*/
int GrammaticalParser::__number() {
	// 词法分析充足，该非终结符被忽略
	BACKUP;
	FLAG_FAIL;
}

/**
 * ＜非零数字＞::= １｜．．．｜９
 * FIRST: ...
 */
int GrammaticalParser::__non_zero_number() {
	// 词法分析充足，该非终结符被忽略
	BACKUP;
	FLAG_FAIL;
}

/**
 * ＜字符＞::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'
 * FIRST: '
 * !: 词法分析时由于已经忽略了单引号'并判断了字符，因此该规则匹配不完全严格。
*/
int GrammaticalParser::__char() {
	BACKUP;
	if (token->equal(SYMBOL::CHARTK)) {
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}

/**
 * ＜字符串＞::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 * FIRST: "
 * !: 词法分析时由于已经忽略了双引号并判断了字符，因此该规则匹配不完全严格.
*/
int GrammaticalParser::__string() {
	BACKUP;
	if (token->equal(SYMBOL::STRCON)) {FLAG_PASS;}
	else {FLAG_FAIL;}
}

/**
 * ＜程序＞::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
 * 
*/
int GrammaticalParser::__program()
{

}

/**
 * ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
 * FISRT(x) = {CONSTTK}
*/
int GrammaticalParser::__const_des()
{
	BACKUP;
	int cnt = 0;
	while(token->equal(SYMBOL::CONSTTK)){
		cnt += 1;
		if (token->equal(SYMBOL::CONSTTK)) {
			_next();
			FLAG_RECUR(__const_def);
		}
		else {
			FLAG_FAIL;
		}
	};
	if (cnt > 0) { FLAG_PASS; }
	else { FLAG_PASS; }
}

/**
 * ＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
int GrammaticalParser::__const_def()
{
	BACKUP;
	if (token->equal(SYMBOL::INTTK)) {
		FLAG_RECUR(__idenfr);					// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
		FLAG_RECUR(__integer);					// <整数>
		while (token->equal(SYMBOL::COMMA)) {
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__integer);
		}
		FLAG_PASS;
	}
	else if (token->equal(SYMBOL::CHARTK)) {
		FLAG_RECUR(__idenfr);					// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
		FLAG_RECUR(__char);						// <符号数>
		while (token->equal(SYMBOL::COMMA)) {
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__char);
		}
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}