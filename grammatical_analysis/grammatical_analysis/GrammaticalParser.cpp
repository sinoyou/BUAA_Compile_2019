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

// FLAG_FAIL : ���赱ǰtoken����Ҫ�жϵ�token
#define FLAG_FAIL \
{										\
	string s = string("ERROR WHEN PARSING ") + token->token;	\
	DEBUG_PRINT(s.c_str());				\
	_recover();							\
	return -1;							\
}

// FLAG_PASS �� ��Ҫ����һ���µ�token�����ж�
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

/* ��ʼ������ */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = -1;
}

/* ��ں��� */
void GrammaticalParser::parse() {
	__program();
}

/* ֧�ֺ��� */
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
	// ptoken����
	ptoken_record.push_back(ptoken);
}

void GrammaticalParser::_backdown() {
	// ptoken ������˳����ɺ��ɾ��
	ptoken_record.pop_back();
}

void GrammaticalParser::_recover() {
	// ptoken��token�ָ�;
	ptoken = ptoken_record.back();
	ptoken_record.pop_back();
	_update_token();
}

/**************************** ������������ ******************************/
/*															
 * 1. ���ֹ��򵥵���������ĸ����ʶ�𱻺��ԣ��ѱ��ʷ����������					
 * 2. <�ַ�>ƥ�䲻���ϸ�ġ�
 * 3. �ڶ�ÿ���������ʱ�����ּ�����FOLLOW���ϣ���FOLLOW���ϲ���Ӧ������ù���Ĳ���ѡ���У�ֻ��Ϊ����֤���򲻲������������
 * 4. ��ǰ͵����<����>, <����˵��>, <�������>, <����>, <���>
 */

/**
 * ���ӷ�������� ::= +��-
 * FIRST(+) = {PLUS}, FIRST(-) = {SUB}
 */
int GrammaticalParser::__add_operator() {
	BACKUP;
	SYMBOL candidate[] = { SYMBOL::PLUS,SYMBOL::MINU };
	FLAG_MULTI_SYMBOL_CHECK(candidate, 2);
	FLAG_PASS;
}

/**
 * ���˷��������  ::= *��/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
int GrammaticalParser::__mult_operator() {
	BACKUP;
	SYMBOL candidate[] = { SYMBOL::MULT,SYMBOL::DIV };
	FLAG_MULTI_SYMBOL_CHECK(candidate, 2);
	FLAG_PASS;
}

/**
 * ����ϵ�������  ::=  <��<=��>��>=��!=��==
 * FIRST��...
*/
int GrammaticalParser::__rel_operator() {
	BACKUP;
	SYMBOL list[6] = {SYMBOL::LSS, SYMBOL::LEQ, SYMBOL::GRE, SYMBOL::GEQ, SYMBOL::NEQ, SYMBOL::EQL};
	FLAG_MULTI_SYMBOL_CHECK(list, 6);
	FLAG_PASS;
}

/**
 * ����ĸ��::= �ߣ�a����������z��A����������Z
 * FIRST: ...
*/
int GrammaticalParser::__letter() {
	// �ʷ��������㣬�÷��ս��������
	BACKUP;
	FLAG_FAIL;
}

/**
 * �����֣�::= �������������֣�
 * FIRST: ...
*/
int GrammaticalParser::__number() {
	// �ʷ��������㣬�÷��ս��������
	BACKUP;
	FLAG_FAIL;
}

/**
 * ���������֣�::= ��������������
 * FIRST: ...
 */
int GrammaticalParser::__non_zero_number() {
	// �ʷ��������㣬�÷��ս��������
	BACKUP;
	FLAG_FAIL;
}

/**
 * ���ַ���::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'
 * FIRST: '
 * !: �ʷ�����ʱ�����Ѿ������˵�����'���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�
*/
int GrammaticalParser::__char() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::CHARCON);
	FLAG_PASS;
}

/**
 * ���ַ�����::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
 * FIRST: "
 * !: �ʷ�����ʱ�����Ѿ�������˫����"���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�.
*/
int GrammaticalParser::__string() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::STRCON);
	FLAG_PASS;
}

/**
 * ������::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
 * FISRT(<����˵��>) = {CONSTTK}
 * FISRT(<����˵��>) = {INTTK, CHARTK}
 * FISRT(<�з��صĺ�������>) = {INTTK, CHARTK}
 * FISRT(<�޷��صĺ�������>) = {VOIDTK}
 * FISRT(<������>) = {VOIDTK}
 * 
 * !:{INTTK, CHARTK} ������FISRT���ϵĳ�ͻ����Ҫ�����͵�� - ���<����˵��>��<�к���ֵ��������>
 * !:{VOIDTK} ������FIRST���ϵĳ�ͻ����Ҫ�����͵��
 */
int GrammaticalParser::__program()
{
	BACKUP;
	// <����˵��>
	if (_peek()->equal(SYMBOL::CONSTTK)) {
		FLAG_RECUR(__const_description);
	}

	// <����˵��>: �÷����жϣ�<����˵��>��<�з���ֵ�ĺ�������>�Ӻ����������ַ�����ֲ���
	if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__var_description);
	}

	// {���з���ֵ�������壾 | ���޷���ֵ�������壾}
	// ���޷���ֵ�������壾��Ҫ�ų�void main
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

	// <������>
	FLAG_RECUR(__main_function);

	FLAG_PASS;
}

/**
 * ������˵���� ::=  const���������壾;{ const���������壾;}
 * FISRT(<����˵��>) = {CONSTTK}
 * FOLLOW(<����˵��>) = {...}
 * ������FISRT(< const���������壾;>)��FOLLOW(<����˵��>)���Ͻ��������
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
 * ���������壾::=int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<��������>) = {SEMICN}
 * ������FOLLOW(<��������>)��FIRST(,����ʶ��������������| <�ַ�>) �ཻ�����
*/
int GrammaticalParser::__const_def()
{
	BACKUP;
	if (_peek()->equal(SYMBOL::INTTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::INTTK);
		FLAG_RECUR(__idenfr);					// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// ��ֵ����
		FLAG_RECUR(__integer);					// <����>
		while (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__integer);
		}
	}
	else if (_peek()->equal(SYMBOL::CHARTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::CHARTK);
		FLAG_RECUR(__idenfr);					// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// ��ֵ����
		FLAG_RECUR(__char);						// <������>
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
 * ���޷���������  ::= ���������֣��������֣���| 0
 * FIRST: ...
*/
int GrammaticalParser::__unsigned_integer() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::INTCON);
	FLAG_PASS;
}

/**
 * ��������::= �ۣ������ݣ��޷���������
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
 * ����ʶ����::=  ����ĸ��������ĸ���������֣���
 * FIRST: ...
*/
int GrammaticalParser::__idenfr() {
	BACKUP;
	FLAG_SYMBOL_CHECK(SYMBOL::IDENFR);
	FLAG_PASS;
}

/**
 * ������ͷ����::=  int����ʶ���� | char����ʶ����
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
 * ������˵����::= ���������壾;{���������壾;}
 * FIRST(<����˵��>) = FISRT(<��������>) = {INTTK, CHARTK}
 * FOLLOW(<����˵��>) = FISRT(<�з���ֵ��������>) + FISRT(<�޷���ֵ��������>) + FIRST(<������>) + FIRST(<�����>)
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

	// �÷����������ο�FIRST���ϵĲ���
	while (_peek()->equal(fisrt_list,2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__var_def);
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);
	}
	FLAG_PASS;
}

/**
 * ���������壾::= �����ͱ�ʶ����(����ʶ����|����ʶ����'['���޷���������']'){,(����ʶ����|����ʶ����'['���޷���������']' )} 
 * FOLLOW(<��������>) = {SEMICN}, FISRT(, <��ʶ��>...) �� FIRST(<'[' <�޷��ź���> ']'>)û�н���.
 * FOLLOW(<��������>) = {SEMICN} �� FIRST(<, <��ʶ��>>) û�н���
 * * �������������������������[]ȡ��ȡ����ʾ
*/
int GrammaticalParser::__var_def()
{
	BACKUP;
	FLAG_RECUR(__type_idenfr);
	FLAG_RECUR(__idenfr);
	// '[' <�޷�������> ']'
	if (_peek()->equal(SYMBOL::LBRACK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
		FLAG_RECUR(__unsigned_integer);
		FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
	}
	// {} 
	while (_peek()->equal(SYMBOL::COMMA)) {
		FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
		FLAG_RECUR(__idenfr);
		// '[' <�޷�������> ']'
		if (_peek()->equal(SYMBOL::LBRACK)) {
			FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
			FLAG_RECUR(__unsigned_integer);
			FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
		}
	}
	FLAG_PASS;
}


/**
 * �����ͱ�ʶ����::=  int | char
*/
int GrammaticalParser::__type_idenfr() {
	BACKUP;
	SYMBOL list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
	FLAG_MULTI_SYMBOL_CHECK(list, 2);
	FLAG_PASS;
}

/**
 * ���з���ֵ�������壾  ::=  ������ͷ����'('��������')' '{'��������䣾'}'
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
 * ���޷���ֵ�������壾  ::= void����ʶ����'('��������')''{'��������䣾'}'
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
 * ��������䣾::=�ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
 * FISRT(<����˵��>) = {CONSTK}�� FIRST(<����˵��>) = {FIRST(<��������>)} = {INTTK, CHARTK}
 * FOLLOW(...<[����˵��]>) = FISRT(<�����>) = {INTTK, CONSTTK, ...}
 * !: [<����˵��>] ��ѡ������Ҫ�ų�,_peek(3) = ( ?
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
 * ��������::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����} | ���գ�
 * �������ݵ��ķ�������ʽ��1.����ѡ���FIRST���ϲ����н����� 2.����һ���ա� 3.�����յ�ѡ�񣬷ǿյ����˳��������в��ܺ���FOLLOW(<������>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(������) =  {RPARENT}
 * FOLLOW(<��ʶ��>) = {RPARENT} �� FIRST(<,<���ͱ�ʶ��><��ʶ��>>) �޽���
*/
int GrammaticalParser::__parameter_list()
{
	BACKUP;
	// ~<��>
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
	// <��>
	else {
		
	}
	FLAG_PASS;
}

/**
 * ����������::= void main��(����)�� ��{����������䣾��}��
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
	FLAG_RECUR(__compound_statement);				// <�������>
	FLAG_SYMBOL_CHECK(SYMBOL::RBRACE);				// }
	FLAG_PASS;
}

/**
 * �����ʽ��::= �ۣ������ݣ��{���ӷ�����������}   //[+|-]ֻ�����ڵ�һ��<��>
 * FIRST(<���ʽ>): {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * ���ʽ������ ++5 - 1 ���������
 * 
 * FIRST(<�ӷ������>) = {PLUS, MINU} 
 * FOLLOW(<���ʽ>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA} + {SEMICN}(<��ֵ�������>)
 * FIRST<�ӷ������> �� FOLLOW<���ʽ> û�н���, ����޻���.
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

	FLAG_RECUR(__item);								// <��>
	while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
		FLAG_RECUR(__add_operator);
		FLAG_RECUR(__item);
	}
	FLAG_PASS;
}


/**
 * ���::= �����ӣ�{���˷�������������ӣ�}
 * FIRST(<��>) = FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}

 * FIRST(<�˷������>) = {MULT, DIV}
 * FOLLOW(<��>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST �� FOLLOW �������ʲ����ڻ���
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
 * �����ӣ�::= ����ʶ���� �� ����ʶ����'['�����ʽ��']' | '('�����ʽ��')' �� �������� | ���ַ��������з���ֵ����������䣾  
 * FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * !: ���ӵ�ǰ����ѡ������FIRST���ϲ��ཻ��ԭ�򣬸�дΪ<��ʶ��>[ '[' <���ʽ> ']' ]�� 
 *    ����FIRST([...]) = {LBRACK} �� FOLLOW{<����>} = {MULT, DIV} + FOLLOW(<��>)û�н���
*/
int GrammaticalParser::__factor() {
	BACKUP;
	// <��ʶ��> [ '[' <���ʽ> ']' ] 
	if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
		FLAG_SYMBOL_CHECK(SYMBOL::IDENFR);
		if (_peek()->equal(SYMBOL::LBRACK)) {
			FLAG_SYMBOL_CHECK(SYMBOL::LBRACK);
			FLAG_RECUR(__expression);
			FLAG_SYMBOL_CHECK(SYMBOL::RBRACK);
		}
	}
	// ���з���ֵ����������䣾  
	else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) {
		FLAG_RECUR(__function_call_return);
	}
	// '(' <���ʽ> ')'
	else if (_peek()->equal(SYMBOL::LPARENT)) {
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);
		FLAG_RECUR(__expression);
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	// <����>
	else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
		FLAG_RECUR(__integer);
	}
	// <�ַ�>
	else if (_peek()->equal(SYMBOL::CHARCON)) {
		FLAG_RECUR(__char);
	}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}

/**
 * ����䣾::= ��������䣾����ѭ����䣾| '{'������У�'}'| ���з���ֵ����������䣾; 
             |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|��������䣾;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / CHARTK, INTTK / VOIDTK / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(���з���ֵ����������䣾)��FISRT(<��ֵ���>) ���غϣ���Ҫ��_peek(2) = (
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
 * ����ֵ��䣾::= ����ʶ�����������ʽ��|����ʶ����'['�����ʽ��']'=�����ʽ��
 * �����д <��ֵ���> ::= <��ʶ��> ['[' <���ʽ> ']'] = <���ʽ>
 * FIRST(<��ֵ���>) = {IDENFR}
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
 * ��������䣾  ::= if '('��������')'����䣾��else����䣾��
 * FISRT(<�������>) = {IFTK}
 * 
 * FOLLOW(<�������>) = {ELSETK, SEMICN, WHILETK} + {RBRACE} // <���> + <�����> ��follow
 * FIRST(else <���>) = {else}
 * FOLLOW �� FIRST ��else�ϴ��ڳ�ͻ����ʱ��Ҫ��Ϊ�涨���ȼ�����elseճճ�������if���ɡ�
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
 * �������� ::=  �����ʽ������ϵ������������ʽ���������ʽ��
 * FIRST(<����>) = FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * �����д��<����> ::= <���ʽ>  [ <��ϵ�����> <���ʽ> ]

 * FOLLOW<����> = {SIMICN, RPARENT}, ��FIRST(<���ʽ>)�޽���
 * FIRST(<��ϵ�����> <���ʽ>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
int GrammaticalParser::__condition() {
	BACKUP;

	FLAG_RECUR(__expression);

	if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
		_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
		FLAG_RECUR(__rel_operator);				// <��ϵ�����>
		FLAG_RECUR(__expression);				// <���ʽ>
	}

	FLAG_PASS;
}

/**
 * ��ѭ����䣾::= while '('��������')'����䣾 | 
				  do����䣾while '('��������')' |
				  for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
 * FISRT(<ѭ�����>) = {WHILETK, DOTK, FORTK}
 * FIRST(1) != FIRST(2) != FIRST(3)
*/
int GrammaticalParser::__loop_statement()
{
	BACKUP;
	// while while '('��������')'����䣾
	if (_peek()->equal(SYMBOL::WHILETK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::WHILETK);					// while
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__condition);							// <����>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// ��
		FLAG_RECUR(__statement);							// <���>
	}
	// do while do����䣾while '('��������')'
	else if (_peek()->equal(SYMBOL::DOTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::DOTK);					// do
		FLAG_RECUR(__statement);							// <���>
		FLAG_SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>

		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__condition);							// <����>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// )
	}
	// for: for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
	else if (_peek()->equal(SYMBOL::FORTK)) {
		FLAG_SYMBOL_CHECK(SYMBOL::FORTK);					// for
		FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);					// (
		FLAG_RECUR(__idenfr);								// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
		FLAG_RECUR(__expression);							// <���ʽ>
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
		FLAG_RECUR(__condition);							// <����>
		FLAG_SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
		FLAG_RECUR(__idenfr);								// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
		FLAG_RECUR(__idenfr);								// <��ʶ��>
		if (_peek()->equal(SYMBOL::PLUS)) {					// (+ | -)
			FLAG_SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else {
			FLAG_SYMBOL_CHECK(SYMBOL::MINU);
		}
		FLAG_RECUR(__step_length);							// <����>
		FLAG_SYMBOL_CHECK(SYMBOL::RPARENT);					// )
		FLAG_RECUR(__statement);							// <���>
	}
	else {
		FLAG_FAIL;
	}
	FLAG_PASS;
}

/**
 * ��������::= ���޷���������  
*/
int GrammaticalParser::__step_length() {
	BACKUP;
	FLAG_RECUR(__unsigned_integer);
	FLAG_PASS;
}

/**
 * ���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���з���ֵ����������䣾) = {<��ʶ��>}
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
 * ���޷���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���޷���ֵ����������䣾) = {<��ʶ��>}
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
 * ��ֵ������::= �����ʽ��{,�����ʽ��}�����գ�
 * FISRT() = FIRST(<���ʽ>)
 * FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * FOLLOW(<ֵ������>) = {RPARENT}
 * 
 * !: Ҳ���Բ�����FOLLOW�жϿյ����������ö��FIRST��©
*/
int GrammaticalParser::__value_parameter_list()
{
	BACKUP;

	SYMBOL first_list[] = { SYMBOL::IDENFR, SYMBOL::LPARENT,
						 SYMBOL::PLUS, SYMBOL::MINU, SYMBOL::INTCON,
						 SYMBOL::CHARCON, SYMBOL::INTTK, SYMBOL::CHARTK };

	// <���ʽ>
	if (_peek()->equal(first_list, 8)) {
		FLAG_RECUR(__expression);
		while (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__expression);
		}
	}
	// <��> : ignore
	FLAG_PASS;
}

/**
 * ������У�::=������䣾��
 * FIRST(<���>) = 
 * {IFTK} + {WHILETK} + {DOTK} + {FORTK} + {LBRACE} + {INTTK, CHARTK} + {VOIDTK} + {IDENFR} + {SCANFTK} + {PRINTFTK}
 * + {SEMICN} + {RETURNTK}
 * FOLLOW(<�����>) = {RBRACE}, ���<��>ѡ����<���>ѡ�񲻴��ڻ������⡣
 *
 * !: Ҳ���Բ����ж�follow���ϵķ�ʽ������ö��FISRT��©.
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
 * ������䣾::=  scanf '('����ʶ����{,����ʶ����}')'
 * FIRST(<�����>) = {SCANFTK}
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
 * ��д��䣾 ::= printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'| printf '('�����ʽ��')'
 * FISRT(<�����>) = {PRINTFTK}
 * FISRT(<�ַ���>) = {STRCON}, 
 * FISRT(<���ʽ>) = {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON} + {INTTK, CHARTK}
 * 
 * !: ���ж��ַ�������ʽ����ʱ��Ҳ���Բ���if-else�ͣ����ַ��������ʽ��
*/
int GrammaticalParser::__write_statement() {
	BACKUP;

	SYMBOL exp_first[] =
	{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
	  SYMBOL::INTCON, SYMBOL::CHARCON, SYMBOL::INTTK, SYMBOL::CHARTK };
	
	FLAG_SYMBOL_CHECK(SYMBOL::PRINTFTK);
	FLAG_SYMBOL_CHECK(SYMBOL::LPARENT);

	// printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'
	if (_peek()->equal(SYMBOL::STRCON)) {
		FLAG_RECUR(__string);
		if (_peek()->equal(SYMBOL::COMMA)) {
			FLAG_SYMBOL_CHECK(SYMBOL::COMMA);
			FLAG_RECUR(__expression);
		}
	}
	// printf '('�����ʽ��')'
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
 * ��������䣾::=  return['('�����ʽ��')']   
 * FISRT('('�����ʽ��')') = {LPARENT};
 * FOLLOW{<�������>} = {SEMICN}���޽��漯��
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