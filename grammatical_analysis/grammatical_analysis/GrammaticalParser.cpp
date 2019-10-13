#include "GrammaticalParser.h"

int cnt = 0;

#define DEBUG_PRINT(msg) fprintf(stderr, "%d line: %d, file : %s %s \n", cnt, __LINE__, __FILE__, msg); 

#define BACKUP \
{										\
	_backup();							\
	string s = string("[ENTER]");		\
	cnt ++;								\
	DEBUG_PRINT(s.c_str());				\
}										\

// FLAG_FAIL : 假设当前token是需要判断的token
#define FLAG_FAIL \
{										\
	string s = string("ERROR WHEN PARSING ") + token->token;	\
	DEBUG_PRINT(s.c_str());				\
	_recover();							\
	return -1;							\
}

// FLAG_PASS ： 需要读入一个新的token进行判断
#define FLAG_PASS						\
{										\
	string s = string("[LEAVE]");		\
	DEBUG_PRINT(s.c_str());				\
	cnt--;								\
	_backdown();						\
	return 0;							\
}

#define FLAG_SYMBOL_CHECK(symbol)  \
{										\
	_next();							\
	if (!token->equal(symbol))			\
		{FLAG_FAIL;}					\
	else								\
	 {_save(token);}					\
}

#define FLAG_MULTI_SYMBOL_CHECK(symbols,len) \
{										\
	_next();							\
	int cnt = 0;						\
	for(int i = 0; i < len; i++)		\
	{									\
		if(token->equal(symbols[i]))	\
		{								\
			cnt += 1;					\
			_save(token);				\
		}								\
	}									\
	if (cnt  == 0)						\
		{FLAG_FAIL;}					\
}

#define FLAG_RECUR(func) \
{										\
	int x = func();						\
	if(x != 0)							\
	{									\
		DEBUG_PRINT("ERROR WHEN RECUR");\
		FLAG_FAIL;						\
	}									\
}

#define FLAG_RECUR_TRY(func) \
{											\
	DEBUG_PRINT("### Begin of Try Zone");	\
	int x = func();							\
	if(x != 0)								\
	{										\
		DEBUG_PRINT("TRY FAILED WHEN RECUR");\
	}										\
	DEBUG_PRINT("### End of Try Zone");		\
}

/* 初始化函数 */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = -1;
}

/* 入口函数 */
void GrammaticalParser::parse() {
	__program();
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
 * 4. 超前偷窥：<程序>, <变量说明>, <复合语句>, <因子>, <语句>
 */

/**
 * ＜加法运算符＞ ::= +｜-
 * FIRST(+) = {PLUS}, FIRST(-) = {SUB}
 */
int GrammaticalParser::__add_operator() {
	BACKUP;
	SYMBOL candidate[] = { SYMBOL::PLUS,SYMBOL::MINU };
	FLAG_MULTI_SYMBOL_CHECK(candidate, 2);
	FLAG_PASS;
}

/**
 * ＜乘法运算符＞  ::= *｜/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
int GrammaticalParser::__mult_operator() {
	BACKUP;
	SYMBOL candidate[] = { SYMBOL::MULT,SYMBOL::DIV };
	FLAG_MULTI_SYMBOL_CHECK(candidate, 2);
	FLAG_PASS;
}

/**
 * ＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==
 * FIRST：...
*/
int GrammaticalParser::__rel_operator() {
	BACKUP;
	SYMBOL list[6] = {SYMBOL::LSS, SYMBOL::LEQ, SYMBOL::GRE, SYMBOL::GEQ, SYMBOL::NEQ, SYMBOL::EQL};
	FLAG_MULTI_SYMBOL_CHECK(list, 6);
	FLAG_PASS;
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
	FLAG_SYMBOL_CHECK(SYMBOL::CHARCON);
	FLAG_PASS;
}

/**
 * ＜字符串＞::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
 * FIRST: "
 * !: 词法分析时由于已经忽略了双引号"并判断了字符，因此该规则匹配不完全严格.
*/
int GrammaticalParser::__string() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::STRCON);
	FLAG_PASS;
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
int GrammaticalParser::__program()
{
	BACKUP;
	// <常量说明>
	if (_peek()->equal(SYMBOL::CONSTTK)) {
		FLAG_RECUR(__const_description);
	}

	// <变量说明>: 用反例判断，<变量说明>和<有返回值的函数定义>从后续第三个字符起出现差异
	if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__var_description);
	}

	// {＜有返回值函数定义＞ | ＜无返回值函数定义＞}
	// ＜无返回值函数定义＞需要排除void main
	while (
		(_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) ||
		(_peek()->equal(SYMBOL::VOIDTK) && !_peek(2)->equal(SYMBOL::MAINTK))
		)
	{
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			FLAG_RECUR(__function_return);
		}
		else {
			FLAG_RECUR(__function_void);
		}
	}

	// <主函数>
	FLAG_RECUR(__main_function);

	FLAG_PASS;
}

/**
 * ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
 * FISRT(<常量说明>) = {CONSTTK}
 * FOLLOW(<常量说明>) = {...}
 * 不存在FISRT(< const＜常量定义＞;>)和FOLLOW(<常量说明>)集合交叉情况。
*/
int GrammaticalParser::__const_description()
{
	BACKUP;

	FLAG_SYMBOL_CHECK(SYMBOL::CONSTTK);
	FLAG_RECUR(__const_def);
	FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);

	while(_peek()->equal(SYMBOL::CONSTTK)){
		FLAG_SYMBOL_CHECK(SYMBOL::CONSTTK);
		FLAG_RECUR(__const_def);
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);
	};

	FLAG_PASS;
}

/**
 * ＜常量定义＞::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<常量定义>) = {SEMICN}
 * 不存在FOLLOW(<常量定义>)与FIRST(,＜标识符＞＝＜整数＞| <字符>) 相交情况。
*/
int GrammaticalParser::__const_def()
{
	BACKUP;
	if (_peek()->equal(SYMBOL::INTTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::INTTK);
		FLAG_RECUR(__idenfr);					// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
		FLAG_RECUR(__integer);					// <整数>
		while (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__integer);
		}
	}
	else if (_peek()->equal(SYMBOL::CHARTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::CHARTK);
		FLAG_RECUR(__idenfr);					// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// 赋值符号
		FLAG_RECUR(__char);						// <符号数>
		while (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__char);
		}
	}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}

/**
 * ＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝| 0
 * FIRST: ...
*/
int GrammaticalParser::__unsigned_integer() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::INTCON);
	FLAG_PASS;
}

/**
 * ＜整数＞::= ［＋｜－］＜无符号整数＞
 * FIRST(1) = {PLUS}, FIRST(2) = {MINU}, FIRST(3) = {INTCON}
*/
int GrammaticalParser::__integer()
{
	BACKUP;
	if (_peek()->equal(SYMBOL::PLUS)) {
		FLAG_SYMBOL_CHECK(SYMBOL::PLUS);
	}
	else if (_peek()->equal(SYMBOL::MINU)) {
		FLAG_SYMBOL_CHECK(SYMBOL::MINU);
	}
	FLAG_RECUR(__unsigned_integer);
	FLAG_PASS;
}

/**
 * ＜标识符＞::=  ＜字母＞｛＜字母＞｜＜数字＞｝
 * FIRST: ...
*/
int GrammaticalParser::__idenfr() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::IDENFR);
	FLAG_PASS;
}

/**
 * ＜声明头部＞::=  int＜标识符＞ | char＜标识符＞
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
int GrammaticalParser::__declar_head() {
	BACKUP;
	if (_peek()->equal(SYMBOL::INTTK))
	{
		FLAG_SYMBOL_CHECK(SYMBOL::INTTK);
		FLAG_RECUR(__idenfr);
	}
	else if (_peek()->equal(SYMBOL::CHARTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::CHARTK);
		FLAG_RECUR(__idenfr);
	}
	else {
		FLAG_FAIL;
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
int GrammaticalParser::__var_description()
{
	BACKUP;
	SYMBOL fisrt_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
	FLAG_RECUR(__var_def);
	FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);

	// 用反例修正仅参考FIRST集合的不足
	while (_peek()->equal(fisrt_list,2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__var_def);
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);
	}
	FLAG_PASS;
}

/**
 * ＜变量定义＞::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )} 
 * FOLLOW(<变量定义>) = {SEMICN}, FISRT(, <标识符>...) 与 FIRST(<'[' <无符号函数> ']'>)没有交集.
 * FOLLOW(<变量定义>) = {SEMICN} 与 FIRST(<, <标识符>>) 没有交集
 * * 单变量和数组变量的声明差异[]取或不取来表示
*/
int GrammaticalParser::__var_def()
{
	BACKUP;
	FLAG_RECUR(__type_idenfr);
	FLAG_RECUR(__idenfr);
	// '[' <无符号整数> ']'
	if (_peek()->equal(SYMBOL::LBRACK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
		FLAG_RECUR(__unsigned_integer);
		FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
	}
	// {} 
	while (_peek()->equal(SYMBOL::COMMA)) {
		FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
		FLAG_RECUR(__idenfr);
		// '[' <无符号整数> ']'
		if (_peek()->equal(SYMBOL::LBRACK)) {
			FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
			FLAG_RECUR(__unsigned_integer);
			FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
		}
	}
	FLAG_PASS;
}


/**
 * ＜类型标识符＞::=  int | char
*/
int GrammaticalParser::__type_idenfr() {
	BACKUP;
	SYMBOL list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
	FLAG_MULTI_SYMBOL_CHECK(list, 2);
	FLAG_PASS;
}

/**
 * ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
*/
int GrammaticalParser::__function_return() {
	BACKUP;
	FLAG_RECUR(__declar_head);

	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__parameter_list);
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);

	FLAG_SYMBOL_CHECK(SYMBOL::LBRACE);
	FLAG_RECUR(__compound_statement);
	FLAG_SYMBOL_CHECK(SYMBOL::RBRACE);

	FLAG_PASS;
}

/**
 * ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
*/
int GrammaticalParser::__function_void()
{
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::VOIDTK);
	FLAG_RECUR(__idenfr);

	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__parameter_list);
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);

	FLAG_SYMBOL_CHECK(SYMBOL::LBRACE);
	FLAG_RECUR(__compound_statement);
	FLAG_SYMBOL_CHECK(SYMBOL::RBRACE);

	FLAG_PASS;
}

/**
 * ＜复合语句＞::=［＜常量说明＞］［＜变量说明＞］＜语句列＞
 * FISRT(<常量说明>) = {CONSTK}， FIRST(<变量说明>) = {FIRST(<变量定义>)} = {INTTK, CHARTK}
 * FOLLOW(...<[变量说明]>) = FISRT(<语句列>) = {INTTK, CONSTTK, ...}
 * !: [<变量说明>] 的选择性需要排除,_peek(3) = ( ?
*/
int GrammaticalParser::__compound_statement() {
	BACKUP;
	SYMBOL first_list[] = { SYMBOL::INTTK, SYMBOL::CONSTTK };
	// [<const des>]
	if (_peek()->equal(SYMBOL::CONSTTK)) {
		FLAG_RECUR(__const_description);
	}
	// [<var des>]
	if (_peek()->equal(first_list,2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__var_description);
	}
	FLAG_RECUR(__statement_list);
	FLAG_PASS;
}

/**
 * ＜参数表＞::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞} | ＜空＞
 * 不带回溯的文法分析方式：1.两种选择的FIRST集合不能有交集。 2.至多一个空。 3.导出空的选择，非空导出退出的序列中不能含有FOLLOW(<参数表>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(参数表) =  {RPARENT}
 * FOLLOW(<标识符>) = {RPARENT} 与 FIRST(<,<类型标识符><标识符>>) 无交集
*/
int GrammaticalParser::__parameter_list()
{
	BACKUP;
	// ~<空>
	if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
		FLAG_RECUR(__type_idenfr);
		FLAG_RECUR(__idenfr);
		while (_peek()->equal(SYMBOL::COMMA))
		{
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__type_idenfr);
			FLAG_RECUR(__idenfr);
		}
	}
	// <空>
	else {
		
	}
	FLAG_PASS;
}

/**
 * ＜主函数＞::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
 * FIRST(x): {VOIDTK}
*/
int GrammaticalParser::__main_function()
{
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::VOIDTK);				// void
	FLAG_SYMBOL_CHECK(SYMBOL::MAINTK);				// main
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);				// (
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);				// )
	FLAG_SYMBOL_CHECK(SYMBOL::LBRACE);				// {
	FLAG_RECUR(__compound_statement);				// <复合语句>
	FLAG_SYMBOL_CHECK(SYMBOL::RBRACE);				// }
	FLAG_PASS;
}

/**
 * ＜表达式＞::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>
 * FIRST(<表达式>): {PLUS, MINU, FIRST(项)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * 表达式将允许 ++5 - 1 的情况出现
 * 
 * FIRST(<加法运算符>) = {PLUS, MINU} 
 * FOLLOW(<表达式>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA} + {SEMICN}(<赋值语句新增>)
 * FIRST<加法运算符> 与 FOLLOW<表达式> 没有交集, 因此无回溯.
*/
int GrammaticalParser::__expression() {
	BACKUP;
	// match +/- if have
	if (_peek()->equal(SYMBOL::PLUS)) {
		FLAG_SYMBOL_CHECK(SYMBOL::PLUS);
	}
	else if (_peek()->equal(SYMBOL::MINU)) {
		FLAG_SYMBOL_CHECK(SYMBOL::MINU);
	}

	FLAG_RECUR(__item);								// <项>
	while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
		FLAG_RECUR(__add_operator);
		FLAG_RECUR(__item);
	}
	FLAG_PASS;
}


/**
 * ＜项＞::= ＜因子＞{＜乘法运算符＞＜因子＞}
 * FIRST(<项>) = FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}

 * FIRST(<乘法运算符>) = {MULT, DIV}
 * FOLLOW(<项>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST 与 FOLLOW 交集，故不存在回溯
*/
int GrammaticalParser::__item() {
	BACKUP;
	FLAG_RECUR(__factor);
	while (_peek()->equal(SYMBOL::MULT) || _peek()->equal(SYMBOL::DIV)) {
		FLAG_RECUR(__mult_operator);
		FLAG_RECUR(__factor);
	}
	FLAG_PASS;
}

/**
 * ＜因子＞::= ＜标识符＞ ｜ ＜标识符＞'['＜表达式＞']' | '('＜表达式＞')' ｜ ＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞  
 * FIRST(<因子>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * !: 因子的前两种选择不满足FIRST集合不相交的原则，改写为<标识符>[ '[' <表达式> ']' ]， 
 *    并且FIRST([...]) = {LBRACK} 与 FOLLOW{<因子>} = {MULT, DIV} + FOLLOW(<项>)没有交集
*/
int GrammaticalParser::__factor() {
	BACKUP;
	// <标识符> [ '[' <表达式> ']' ] 
	if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
		FLAG_SYMBOL_CHECK(SYMBOL::IDENFR);
		if (_peek()->equal(SYMBOL::LBRACK)) {
			FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
			FLAG_RECUR(__expression);
			FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
		}
	}
	// ＜有返回值函数调用语句＞  
	else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__function_call_return);
	}
	// '(' <表达式> ')'
	else if (_peek()->equal(SYMBOL::LPARENT)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
		FLAG_RECUR(__expression);
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	// <整数>
	else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
		FLAG_RECUR(__integer);
	}
	// <字符>
	else if (_peek()->equal(SYMBOL::CHARCON)) {
		FLAG_RECUR(__char);
	}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}

/**
 * ＜语句＞::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'| ＜有返回值函数调用语句＞; 
             |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜返回语句＞;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / CHARTK, INTTK / VOIDTK / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(＜有返回值函数调用语句＞)与FISRT(<赋值语句>) 有重合，需要用_peek(2) = (
*/
int GrammaticalParser::__statement()
{
	BACKUP;

	SYMBOL loop_first[] = { SYMBOL::WHILETK, SYMBOL::FORTK, SYMBOL::DOTK };

	if (_peek()->equal(SYMBOL::IFTK)) { FLAG_RECUR(__condition_statement); }
	else if (_peek()->equal(loop_first, 3)) { FLAG_RECUR(__loop_statement); }
	else if (_peek()->equal(SYMBOL::LBRACE)) { 
		FLAG_SYMBOL_CHECK(SYMBOL::LBRACE);
		FLAG_RECUR(__statement_list);
		FLAG_SYMBOL_CHECK(SYMBOL::RBRACE);
	}
	else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) 
	{ FLAG_RECUR(__function_call_return); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else if (_peek()->equal(SYMBOL::VOIDTK)) { FLAG_RECUR(__function_call_void); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) 
	{ FLAG_RECUR(__assign_statment); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else if (_peek()->equal(SYMBOL::SCANFTK)) { FLAG_RECUR(__read_statement); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else if (_peek()->equal(SYMBOL::PRINTFTK)) { FLAG_RECUR(__write_statement); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else if (_peek()->equal(SYMBOL::SEMICN)) { FLAG_SYMBOL_CHECK(SYMBOL::SEMICN); }
	else if (_peek()->equal(SYMBOL::RETURNTK)) { FLAG_RECUR(__return_statement); FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}


/**
 * ＜赋值语句＞::= ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
 * 规则改写 <赋值语句> ::= <标识符> ['[' <表达式> ']'] = <表达式>
 * FIRST(<赋值语句>) = {IDENFR}
*/
int GrammaticalParser::__assign_statment() {
	BACKUP;

	FLAG_SYMBOL_CHECK(SYMBOL::IDENFR);
	if (_peek()->equal(SYMBOL::LBRACK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
		FLAG_RECUR(__expression);
		FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
	}
	FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);					// ASSIGN: = ,  EQL: ==
	FLAG_RECUR(__expression);
	
	FLAG_PASS;
}

/**
 * ＜条件语句＞  ::= if '('＜条件＞')'＜语句＞［else＜语句＞］
 * FISRT(<条件语句>) = {IFTK}
 * 
 * FOLLOW(<条件语句>) = {ELSETK, SEMICN, WHILETK} + {RBRACE} // <语句> + <语句列> 的follow
 * FIRST(else <语句>) = {else}
 * FOLLOW 与 FIRST 在else上存在冲突，此时需要人为规定优先级次序，else粘粘至最近的if即可。
*/
int GrammaticalParser::__condition_statement() {
	BACKUP;

	FLAG_SYMBOL_CHECK(SYMBOL::IFTK);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__condition);
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);
	
	FLAG_RECUR(__statement);

	if (_peek()->equal(SYMBOL::ELSETK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::ELSETK);
		FLAG_RECUR(__statement);
	}

	FLAG_PASS;
}

/**
 * ＜条件＞ ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
 * FIRST(<条件>) = FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * 规则改写：<条件> ::= <表达式>  [ <关系运算符> <表达式> ]

 * FOLLOW<条件> = {SIMICN, RPARENT}, 与FIRST(<表达式>)无交集
 * FIRST(<关系运算符> <表达式>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
int GrammaticalParser::__condition() {
	BACKUP;

	FLAG_RECUR(__expression);

	if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
		_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
		FLAG_RECUR(__rel_operator);				// <关系运算符>
		FLAG_RECUR(__expression);				// <表达式>
	}

	FLAG_PASS;
}

/**
 * ＜循环语句＞::= while '('＜条件＞')'＜语句＞ | 
				  do＜语句＞while '('＜条件＞')' |
				  for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
 * FISRT(<循环语句>) = {WHILETK, DOTK, FORTK}
 * FIRST(1) != FIRST(2) != FIRST(3)
*/
int GrammaticalParser::__loop_statement()
{
	BACKUP;
	// while while '('＜条件＞')'＜语句＞
	if (_peek()->equal(SYMBOL::WHILETK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::WHILETK);					// while
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__condition);							// <条件>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// ）
		FLAG_RECUR(__statement);							// <语句>
	}
	// do while do＜语句＞while '('＜条件＞')'
	else if (_peek()->equal(SYMBOL::DOTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::DOTK);					// do
		FLAG_RECUR(__statement);							// <语句>
		FLAG_SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>

		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__condition);							// <条件>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// )
	}
	// for: for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
	else if (_peek()->equal(SYMBOL::FORTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::FORTK);					// for
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__idenfr);								// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
		FLAG_RECUR(__expression);							// <表达式>
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
		FLAG_RECUR(__condition);							// <条件>
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
		FLAG_RECUR(__idenfr);								// <标识符>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
		FLAG_RECUR(__idenfr);								// <标识符>
		if (_peek()->equal(SYMBOL::PLUS)) {					// (+ | -)
			FLAG_SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else {
			FLAG_SYMBOL_CHECK(SYMBOL::MINU);
		}
		FLAG_RECUR(__step_length);							// <步长>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// )
		FLAG_RECUR(__statement);							// <语句>
	}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}

/**
 * ＜步长＞::= ＜无符号整数＞  
*/
int GrammaticalParser::__step_length() {
	BACKUP;
	FLAG_RECUR(__unsigned_integer);
	FLAG_PASS;
}

/**
 * ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜有返回值函数调用语句＞) = {<标识符>}
*/
int GrammaticalParser::__function_call_return() {
	BACKUP;

	FLAG_RECUR(__idenfr);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__value_parameter_list);
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);

	FLAG_PASS;
}

/**
 * ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'
 * FISRT(＜无返回值函数调用语句＞) = {<标识符>}
*/
int GrammaticalParser::__function_call_void() {
	BACKUP;

	FLAG_RECUR(__idenfr);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__value_parameter_list);
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);

	FLAG_PASS;
}

/**
 * ＜值参数表＞::= ＜表达式＞{,＜表达式＞}｜＜空＞
 * FISRT() = FIRST(<表达式>)
 * FISRT(<表达式>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * FOLLOW(<值参数表>) = {RPARENT}
 * 
 * !: 也可以采用用FOLLOW判断空的情况，避免枚举FIRST遗漏
*/
int GrammaticalParser::__value_parameter_list()
{
	BACKUP;

	SYMBOL first_list[] = { SYMBOL::IDENFR, SYMBOL::LPARENT,
						 SYMBOL::PLUS, SYMBOL::MINU, SYMBOL::INTCON,
						 SYMBOL::CHARCON, SYMBOL::INTTK, SYMBOL::CHARTK };

	// <表达式>
	if (_peek()->equal(first_list, 8)) {
		FLAG_RECUR(__expression);
		while (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__expression);
		}
	}
	// <空> : ignore
	FLAG_PASS;
}

/**
 * ＜语句列＞::=｛＜语句＞｝
 * FIRST(<语句>) = 
 * {IFTK} + {WHILETK} + {DOTK} + {FORTK} + {LBRACE} + {INTTK, CHARTK} + {VOIDTK} + {IDENFR} + {SCANFTK} + {PRINTFTK}
 * + {SEMICN} + {RETURNTK}
 * FOLLOW(<语句列>) = {RBRACE}, 因此<空>选择与<语句>选择不存在回溯问题。
 *
 * !: 也可以采用判断follow集合的方式，避免枚举FISRT遗漏.
*/
int GrammaticalParser::__statement_list() {
	BACKUP;

	SYMBOL list[] = { 
		SYMBOL::IFTK,
		SYMBOL::WHILETK,
		SYMBOL::DOTK,
		SYMBOL::FORTK,
		SYMBOL::LBRACE,
		SYMBOL::INTTK, SYMBOL::CHARTK,
		SYMBOL::VOIDTK, 
		SYMBOL::IDENFR, 
		SYMBOL::SCANFTK, 
		SYMBOL::PRINTFTK, 
		SYMBOL::SEMICN,
		SYMBOL::RETURNTK };
	while (_peek()->equal(list, 13)) {
		FLAG_RECUR(__statement);
	}

	FLAG_PASS;
}

/**
 * ＜读语句＞::=  scanf '('＜标识符＞{,＜标识符＞}')'
 * FIRST(<读语句>) = {SCANFTK}
*/
int GrammaticalParser::__read_statement() {
	BACKUP;
	
	FLAG_SYMBOL_CHECK(SYMBOL::SCANFTK);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
	FLAG_RECUR(__idenfr);
	while (_peek()->equal(SYMBOL::COMMA)) {
		FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
		FLAG_RECUR(__idenfr);
	}
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);
	FLAG_PASS;
}

/**
 * ＜写语句＞ ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
 * FISRT(<读语句>) = {PRINTFTK}
 * FISRT(<字符串>) = {STRCON}, 
 * FISRT(<表达式>) = {PLUS, MINU, FIRST(项)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * 
 * !: 在判断字符串与表达式走向时，也可以采用if-else型，非字符串即表达式。
*/
int GrammaticalParser::__write_statement() {
	BACKUP;

	SYMBOL exp_first[] =
	{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
	  SYMBOL::INTCON, SYMBOL::CHARCON, SYMBOL::INTTK, SYMBOL::CHARTK };
	
	FLAG_SYMBOL_CHECK(SYMBOL::PRINTFTK);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);

	// printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'
	if (_peek()->equal(SYMBOL::STRCON)) {
		FLAG_RECUR(__string);
		if (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__expression);
		}
	}
	// printf '('＜表达式＞')'
	else if (_peek()->equal(exp_first, 8)) {
		FLAG_RECUR(__expression);
	}
	else {
		FLAG_FAIL;
	}
	FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);

	FLAG_PASS;
}

/**
 * ＜返回语句＞::=  return['('＜表达式＞')']   
 * FISRT('('＜表达式＞')') = {LPARENT};
 * FOLLOW{<返回语句>} = {SEMICN}，无交叉集合
*/
int GrammaticalParser::__return_statement() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::RETURNTK);
	if (_peek()->equal(LPARENT)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
		FLAG_RECUR(__expression);
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	FLAG_PASS;
}