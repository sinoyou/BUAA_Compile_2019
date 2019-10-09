#include "GrammaticalParser.h"

#define DEBUG_PRINT(msg) fprintf(stderr, "%s %s - line: %d, file : %s %s \n", __DATE__, __TIME__, __LINE__, __FILE__, msg); 

#define BACKUP _backup();

// FLAG_FAIL : ���赱ǰtoken����Ҫ�жϵ�token
#define FLAG_FAIL \
{										\
	DEBUG_PRINT("ERROR WHEN PARSING")	\
	_recover();							\
	return -1;							\
}

// FLAG_PASS �� ��Ҫ����һ���µ�token�����ж�
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

/* ֧�ֺ��� */
void _error(const char* s);			// �����������
Token _gettoken_index(int index);	// ����������ȡһ������
void _gettoken();					// ��ȡһ������
void _retract();					// ����һ������

/* ��ʼ������ */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = 0;
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
	// ptoken����
	ptoken_record.push_back(ptoken);
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
 */

/**
 * ���ӷ�������� ::= +��-
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
 * ���˷��������  ::= *��/
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
 * ����ϵ�������  ::=  <��<=��>��>=��!=��==
 * FIRST��...
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
	if (token->equal(SYMBOL::CHARTK)) {
		FLAG_PASS;
	}
	else {
		FLAG_FAIL;
	}
}

/**
 * ���ַ�����::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
 * FIRST: "
 * !: �ʷ�����ʱ�����Ѿ�������˫���Ų��ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�.
*/
int GrammaticalParser::__string() {
	BACKUP;
	if (token->equal(SYMBOL::STRCON)) {FLAG_PASS;}
	else {FLAG_FAIL;}
}

/**
 * ������::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
 * 
*/
int GrammaticalParser::__program()
{

}

/**
 * ������˵���� ::=  const���������壾;{ const���������壾;}
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
 * ���������壾::=int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
int GrammaticalParser::__const_def()
{
	BACKUP;
	if (token->equal(SYMBOL::INTTK)) {
		FLAG_RECUR(__idenfr);					// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// ��ֵ����
		FLAG_RECUR(__integer);					// <����>
		while (token->equal(SYMBOL::COMMA)) {
			FLAG_RECUR(__idenfr);
			FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);
			FLAG_RECUR(__integer);
		}
		FLAG_PASS;
	}
	else if (token->equal(SYMBOL::CHARTK)) {
		FLAG_RECUR(__idenfr);					// <��ʶ��>
		FLAG_SYMBOL_CHECK(SYMBOL::ASSIGN);		// ��ֵ����
		FLAG_RECUR(__char);						// <������>
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