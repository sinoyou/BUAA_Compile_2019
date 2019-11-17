#include "GrammaticalParser.h"
#include "utils.h"
#include "debug.h"

/* ��ʼ������ */
GrammaticalParser::GrammaticalParser(
	vector<Token>& token_list, 
	vector<string>& output,
	vector<tuple<int, string>>& error_output)
	:token_list(token_list), output_list(output), error_output_list(error_output) {
	token = NULL;
	ptoken = -1;
	symbol_table = SymbolTable();
}

/* ��ں��� */
void GrammaticalParser::parse() {
	__program(1);
}

/* ֧�ֺ��� */
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
 * 4. ����-��ǰ͵����<����>, <����˵��>, <����>, <���>
 * 5. ����-��ͬ������ͬ�ṹ���轨�����ű��ж�<�з��غ����������>��<�޷��غ����������>
 */

 /**
  * ���ӷ�������� ::= +��-
  * FIRST(+) = {PLUS}, FIRST(-) = {SUB}
  */
void GrammaticalParser::__add_operator(int level) {
	FLAG_ENTER("<�ӷ������>",level);
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
 * ���˷��������  ::= *��/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
void GrammaticalParser::__mult_operator(int level) {
	FLAG_ENTER("<�˷������>", level);
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
 * ����ϵ�������  ::=  <��<=��>��>=��!=��==
 * FIRST��...
*/
void GrammaticalParser::__rel_operator(int level) {
	FLAG_ENTER("<��ϵ�����>", level);
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
 * ����ĸ��::= �ߣ�a����������z��A����������Z
 * FIRST: ...
*/
void GrammaticalParser::__letter(int level) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<��ĸ>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Char>"));
}

/**
 * �����֣�::= �������������֣�
 * FIRST: ...
*/
void GrammaticalParser::__number(int level) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<����>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Number>"));
}

/**
 * ���������֣�::= ��������������
 * FIRST: ...
 */
void GrammaticalParser::__non_zero_number(int level) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<��������>", level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Non-Zero Number>"));
}

/**
 * ���ַ���::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'
 * FIRST: '
 * !: �ʷ�����ʱ�����Ѿ������˵�����'���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�
*/
char GrammaticalParser::__char(int level) {
	FLAG_ENTER("<�ַ�>", level);
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
 * ���ַ�����::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
 * FIRST: "
 * !: �ʷ�����ʱ�����Ѿ�������˫����"���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�.
*/
void GrammaticalParser::__string(int level) {
	FLAG_ENTER("<�ַ���>", level);
	try {
		SYMBOL_CHECK(SYMBOL::STRCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
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
void GrammaticalParser::__program(int level)
{
	FLAG_ENTER("<����>", level);
	symbol_table.add_one_block();												// ���뵱ǰ��
	SymbolFactory::create_function(symbol_table.get_present_block(),
		"__global__",
		BasicType::_void, vector<SymbolItem*>());
	try {
		// <����˵��>
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			__const_description(level + 1);
		}

		// <����˵��>: �÷����жϣ�<����˵��>��<�з���ֵ�ĺ�������>�Ӻ����������ַ�����ֲ���
		if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			__var_description(level + 1);
		}

		// {���з���ֵ�������壾 | ���޷���ֵ�������壾}
		// ���޷���ֵ�������壾��Ҫ�ų�void main
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
		// <������>
		__main_function(level + 1);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	symbol_table.exit_present_block();												// �뿪��ǰ��
}

/**
 * ������˵���� ::=  const���������壾;{ const���������壾;}
 * FISRT(<����˵��>) = {CONSTTK}
 * FOLLOW(<����˵��>) = {...}
 * ������FISRT(< const���������壾;>)��FOLLOW(<����˵��>)���Ͻ��������
*/
void GrammaticalParser::__const_description(int level)
{
	FLAG_ENTER("<����˵��>", level);
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
 * ���������壾::=int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<��������>) = {SEMICN}
 * ������FOLLOW(<��������>)��FIRST(,����ʶ��������������| <�ַ�>) �ཻ�����
*/
void GrammaticalParser::__const_def(int level)
{
	FLAG_ENTER("<��������>", level);
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
				if (cnt++ > 0)
					SYMBOL_CHECK(SYMBOL::COMMA);
				name = __idenfr(level + 1, true);
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				// unit4-error-o
				try {
					if (type == BasicType::_int)
						const_value = __integer(level + 1);
					else
						const_value = __char(level + 1);
					// ���������Ķ���
					if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
						while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
							_next();
						_register_error(token->line, ErrorType::ConstDefWrong);
					}
					// ��ȷ��������������
					SymbolFactory::create_const(symbol_table.get_present_block(), name, type, const_value);
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
 * ���޷���������  ::= ���������֣��������֣���| 0
 * FIRST: ...
*/
int GrammaticalParser::__unsigned_integer(int level) {
	FLAG_ENTER("<�޷�������>", level);
	Token* t = _peek();
	try {
		SYMBOL_CHECK(SYMBOL::INTCON);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return t->transnum();
}

/**
 * ��������::= �ۣ������ݣ��޷���������
 * FIRST(1) = {PLUS}, FIRST(2) = {MINU}, FIRST(3) = {INTCON}
*/
int GrammaticalParser::__integer(int level)
{
	FLAG_ENTER("<����>", level);
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
 * ����ʶ����::=  ����ĸ��������ĸ���������֣���
 * FIRST: ...
*/
string GrammaticalParser::__idenfr(int level, bool is_def) {
	FLAG_ENTER("<��ʶ��>", level);
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
 * ������ͷ����::=  int����ʶ���� | char����ʶ����
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
void GrammaticalParser::__declar_head(int level, BasicType* return_type, string* func_name) {
	FLAG_ENTER("<����ͷ��>", level);
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
 * ������˵����::= ���������壾;{���������壾;}
 * FIRST(<����˵��>) = FISRT(<��������>) = {INTTK, CHARTK}
 * FOLLOW(<����˵��>) = FISRT(<�з���ֵ��������>) + FISRT(<�޷���ֵ��������>) + FIRST(<������>) + FIRST(<�����>)
 * {INTTK, CHARTK} // _peek(3) = ( ?
 * {VOIDTK}		   // _peek() = VOIDTK
 * {VOIDTK}        // _peek() = VOIDTK
 * {IFTK, WHILETK, DOTK, FORTK, LBRACE, INTTK, CHARTK,    // _peek(3) = (?
 *  VOIDTK, IDENFR, SCANFTK, PRINTFTK, SEMICN, RETURNTK}
 */
void GrammaticalParser::__var_description(int level)
{
	FLAG_ENTER("<����˵��>", level);
	try {
		SYMBOL fisrt_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		__var_def(level + 1);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		// �÷����������ο�FIRST���ϵĲ���
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
 * ���������壾::= �����ͱ�ʶ����(����ʶ����|����ʶ����'['���޷���������']'){,(����ʶ����|����ʶ����'['���޷���������']' )}
 * FOLLOW(<��������>) = {SEMICN}, FISRT(, <��ʶ��>...) �� FIRST(<'[' <�޷��ź���> ']'>)û�н���.
 * FOLLOW(<��������>) = {SEMICN} �� FIRST(<, <��ʶ��>>) û�н���
 * * �������������������������[]ȡ��ȡ����ʾ
*/
void GrammaticalParser::__var_def(int level)
{
	FLAG_ENTER("<��������>", level);
	try {
		__type_idenfr(level + 1);
		BasicType type = (token->symbol == SYMBOL::CHARTK) ? BasicType::_char : BasicType::_int;
		
		int cnt = 0;
		// {} 
		do {
			if(cnt ++ > 0)
				SYMBOL_CHECK(SYMBOL::COMMA);
			string idenfr_name = __idenfr(level + 1, true);
			bool is_array = false;
			// '[' <�޷�������> ']'
			if (_peek()->equal(SYMBOL::LBRACK)) {
				SYMBOL_CHECK(SYMBOL::LBRACK);
				__unsigned_integer(level + 1);
				SYMBOL_CHECK(SYMBOL::RBRACK);
				is_array = true;
			}
			SymbolFactory::create_variable(symbol_table.get_present_block(), idenfr_name, type, is_array);
		} while (_peek()->equal(SYMBOL::COMMA));
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}


/**
 * �����ͱ�ʶ����::=  int | char
*/
BasicType GrammaticalParser::__type_idenfr(int level) {
	FLAG_ENTER("<���ͱ�ʶ��>", level);
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
 * ���з���ֵ�������壾  ::=  ������ͷ����'('��������')' '{'��������䣾'}'
*/
void GrammaticalParser::__function_return(int level) {
	FLAG_ENTER("<�з���ֵ��������>", level);

	symbol_table.add_one_block();												// ���뵱ǰ��

	try {
		BasicType return_type;
		string func_name;
		vector<SymbolItem*> para_list;

		__declar_head(level + 1, &return_type, &func_name);

		SYMBOL_CHECK(SYMBOL::LPARENT);
		__parameter_list(level + 1, &para_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
		// ����ͷ������ϣ����Ը��º�����SymbolItem������.
		SymbolFactory::create_function(symbol_table.get_present_block(), func_name, return_type, para_list);
		
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

	symbol_table.exit_present_block();											// �뿪��ǰ��
}

/**
 * ���޷���ֵ�������壾  ::= void����ʶ����'('��������')''{'��������䣾'}'
*/
void GrammaticalParser::__function_void(int level) {
	FLAG_ENTER("<�޷���ֵ��������>", level);

	symbol_table.add_one_block();												// ���뵱ǰ��

	try {
		BasicType return_type = BasicType::_void;
		string func_name;
		vector<SymbolItem*> para_list;

		SYMBOL_CHECK(SYMBOL::VOIDTK);
		func_name = __idenfr(level + 1, true);

		SYMBOL_CHECK(SYMBOL::LPARENT);
		__parameter_list(level + 1, &para_list);
		SYMBOL_CHECK(SYMBOL::RPARENT);
		// ����ͷ������ϣ����Ը��º�����SymbolItem������.
		SymbolFactory::create_function(symbol_table.get_present_block(), func_name, return_type, para_list);


		SYMBOL_CHECK(SYMBOL::LBRACE);
		bool has_return = false;
		__compound_statement(level + 1, &has_return);

		// RECUR_CHECK(__compound_statement, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::RBRACE);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;

	symbol_table.exit_present_block();											// �뿪��ǰ��
}

/**
 * ��������䣾::=�ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
 * FISRT(<����˵��>) = {CONSTK}�� FIRST(<����˵��>) = {FIRST(<��������>)} = {INTTK, CHARTK}
 * FOLLOW(...<[����˵��]>) = FISRT(<�����>) �� FISRT(<����˵��>) û�н���
*/
void GrammaticalParser::__compound_statement(int level, bool *has_return) {
	FLAG_ENTER("<�������>", level);
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
 * ��������::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����} | ���գ�
 * �������ݵ��ķ�������ʽ��1.����ѡ���FIRST���ϲ����н����� 2.����һ���ա� 3.�����յ�ѡ�񣬷ǿյ����˳��������в��ܺ���FOLLOW(<������>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(������) =  {RPARENT}
 * FOLLOW(<��ʶ��>) = {RPARENT} �� FIRST(<,<���ͱ�ʶ��><��ʶ��>>) �޽���
*/
void GrammaticalParser::__parameter_list(int level, vector<SymbolItem*>* para_list)
{
	FLAG_ENTER("<������>", level);

	try {
		// ~<��>
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			int cnt = 0;
			do
			{
				if(cnt ++ > 0)
					SYMBOL_CHECK(SYMBOL::COMMA);
				BasicType type = __type_idenfr(level + 1);
				string name = __idenfr(level + 1, true);
				// �������ͱ�ʾ
				SymbolItem* para = SymbolFactory::create_variable(symbol_table.get_present_block(),name, type, false);
				para_list->push_back(para);
			} while (_peek()->equal(SYMBOL::COMMA));
		}
		// <��>
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
 * ����������::= void main��(����)�� ��{����������䣾��}��
 * FIRST(x): {VOIDTK}
*/
void GrammaticalParser::__main_function(int level)
{
	FLAG_ENTER("<������>", level);
	symbol_table.add_one_block();												// ���뵱ǰ��
	SymbolFactory::create_function(symbol_table.get_present_block(), "main", BasicType::_void, vector<SymbolItem*>());
	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);				// void
		SYMBOL_CHECK(SYMBOL::MAINTK);				// main
		SYMBOL_CHECK(SYMBOL::LPARENT);				// (
		SYMBOL_CHECK(SYMBOL::RPARENT);				// )
		SYMBOL_CHECK(SYMBOL::LBRACE);				// {
		bool has_return = false;
		__compound_statement(level + 1, &has_return);
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
 * �����ʽ��::= �ۣ������ݣ��{���ӷ�����������}   //[+|-]ֻ�����ڵ�һ��<��>
 * FIRST(<���ʽ>): {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * ���ʽ������ ++5 - 1 ���������
 *
 * FIRST(<�ӷ������>) = {PLUS, MINU}
 * FOLLOW(<���ʽ>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA} + {SEMICN}(<��ֵ�������>)
 * FIRST<�ӷ������> �� FOLLOW<���ʽ> û�н���, ����޻���.
*/
void GrammaticalParser::__expression(int level, bool *is_char) {
	FLAG_ENTER("<���ʽ>", level);
	int cnt = 0;
	try {
		// match +/- if have
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
		}

		// RECUR_CHECK(__item, RECUR_DEFAULT);								// <��>
		bool temp = false;
		__item(level + 1, &temp);
		cnt++;
		while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
			__add_operator(level + 1);
			// RECUR_CHECK(__item, RECUR_DEFAULT);
			bool tempx = false;
			__item(level + 1, &temp);
			cnt++;
		}
		// ���ʽֻ��һ�������������char����ô���ʽ����char��
		if (cnt == 1 && temp && is_char != NULL)
			* is_char = true;
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}


/**
 * ���::= �����ӣ�{���˷�������������ӣ�}
 * FIRST(<��>) = FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}

 * FIRST(<�˷������>) = {MULT, DIV}
 * FOLLOW(<��>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST �� FOLLOW �������ʲ����ڻ���
*/
void GrammaticalParser::__item(int level, bool *is_char) {
	FLAG_ENTER("<��>", level);
	int cnt = 0;
	try {
		bool temp = false;
		__factor(level + 1, &temp);
		cnt++;
		// RECUR_CHECK(__factor, RECUR_DEFAULT);
		while (_peek()->equal(SYMBOL::MULT) || _peek()->equal(SYMBOL::DIV)) {
			__mult_operator(level + 1);
			bool tempx = false;
			// RECUR_CHECK(__factor, RECUR_DEFAULT);
			__factor(level + 1, &tempx);
			cnt++;
		}
		// ����ֻ��һ�����Ӳ�������Ϊchar����Ϊchar
		if (cnt == 1 && temp && is_char) {
			*is_char = true;
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * �����ӣ�::= ����ʶ���� �� ����ʶ����'['�����ʽ��']' | '('�����ʽ��')' �� �������� | ���ַ��������з���ֵ����������䣾
 * FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * !: ���ӵ�ǰ����ѡ������FIRST���ϲ��ཻ��ԭ�򣬸�дΪ<��ʶ��>[ '[' <���ʽ> ']' ]��
 *    ����FIRST([...]) = {LBRACK} �� FOLLOW{<����>} = {MULT, DIV} + FOLLOW(<��>)û�н���
*/
void GrammaticalParser::__factor(int level, bool *is_char) {
	FLAG_ENTER("<����>", level);
	try {
		// <��ʶ��> [ '[' <���ʽ> ']' ] 
		if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
			// SYMBOL_CHECK(SYMBOL::IDENFR);
			__idenfr(level + 1, false);
			// �ڷ��ű��в��ұ�������
			SymbolItem* p = find_const_var(symbol_table.get_present_block(), token->token, true);
			if (p != NULL) {
				if (p->var_type == BasicType::_char && is_char != NULL) {
					*is_char = true;
				}
			}
			if (_peek()->equal(SYMBOL::LBRACK)) {
				bool char_detector = false;
				SYMBOL_CHECK(SYMBOL::LBRACK);
				__expression(level + 1, &char_detector);
				// unit4-error-i
				if (char_detector) {
					_register_error(token->line, ErrorType::ArrayIndexWrong);
				}
				SYMBOL_CHECK(SYMBOL::RBRACK);
			}
		}
		// ���з���ֵ����������䣾  
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT)) {
			string name = _peek()->token;
			__function_call_return(level + 1);
			// �ڷ��ű��в��Һ�������
			SymbolItem* p = find_func(symbol_table.get_present_block(), name, true);
			if (p != NULL) {
				if (p->return_type == BasicType::_char && is_char != NULL) {
					*is_char = true;
				}
			}
		}
		// '(' <���ʽ> ')'
		else if (_peek()->equal(SYMBOL::LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			__expression(level + 1, NULL);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
		// <����>
		else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
			__integer(level + 1);
		}
		// <�ַ�>
		else if (_peek()->equal(SYMBOL::CHARCON)) {
			__char(level + 1);
			if (is_char) {
				*is_char = true;
			}
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
	
}

/**
 * ����䣾::= ��������䣾����ѭ����䣾| '{'������У�'}'| ���з���ֵ����������䣾;
			 |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|��������䣾;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / IDENFR / IDENFR / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(���з���ֵ����������䣾)��FISRT(<��ֵ���>) ���غϣ���Ҫ��_peek(2) = (
*/
void GrammaticalParser::__statement(int level, bool * has_return)
{
	FLAG_ENTER("<���>", level);
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
			// �з���ֵ�����������޷���ֵ����������ʽ��ȫһ�£���Ҫ��ǰʹ�÷��ű����С�
			if (found != NULL && found->return_type != BasicType::_void) { __function_call_return(level + 1); }
			else if (found != NULL && found->return_type == BasicType::_void) { __function_call_void(level + 1); }
			else { 
				_register_error(_peek()->line, ErrorType::Undefine);
				while (!_peek()->equal(SYMBOL::SEMICN))
					_next();
			}	// ������Ϊ�������ı�ʶ����û���ҵ���Ӧ�ġ�
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
 * ����ֵ��䣾::= ����ʶ�����������ʽ��|����ʶ����'['�����ʽ��']'=�����ʽ��
 * �����д <��ֵ���> ::= <��ʶ��> ['[' <���ʽ> ']'] = <���ʽ>
 * FIRST(<��ֵ���>) = {IDENFR}
*/
void GrammaticalParser::__assign_statment(int level) {
	FLAG_ENTER("<��ֵ���>", level);
	
	try {
		__idenfr(level + 1 ,false);
		SymbolItem* p = find_const_var(symbol_table.get_present_block(), token->token, true);
		if (p!=NULL) {
			if (p->type == SymbolItemType::_const) {
				_register_error(token->line, ErrorType::ConstModify);
			}
		}
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			bool char_detector = false;
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			__expression(level + 1, &char_detector);
			// unit4-error-i
			if (char_detector) {
				_register_error(token->line, ErrorType::ArrayIndexWrong);
			}
			SYMBOL_CHECK(SYMBOL::RBRACK);
		}
		SYMBOL_CHECK(SYMBOL::ASSIGN);					// ASSIGN: = ,  EQL: ==
		__expression(level + 1, NULL);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ��������䣾  ::= if '('��������')'����䣾��else����䣾��
 * FISRT(<�������>) = {IFTK}
 *
 * FOLLOW(<�������>) = {RBRACE} // <���> + <�����> ��follow
 * FIRST(else <���>) = {else}
 * ����if+һ��else��������ķ��㼶�����������壬��ʱ��Ҫ��Ϊ�涨���ȼ����򣬽���̰��ƥ�䣬elseճճ�������if���ɡ�
*/
void GrammaticalParser::__condition_statement(int level, bool* has_return) {
	FLAG_ENTER("<�������>", level);

	try {
		SYMBOL_CHECK(SYMBOL::IFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		__condition(level + 1);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		// RECUR_CHECK(__statement, RECUR_DEFAULT);
		__statement(level + 1, has_return);

		if (_peek()->equal(SYMBOL::ELSETK)) {
			SYMBOL_CHECK(SYMBOL::ELSETK);
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
 * �������� ::=  �����ʽ������ϵ������������ʽ���������ʽ��
 * FIRST(<����>) = FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * �����д��<����> ::= <���ʽ>  [ <��ϵ�����> <���ʽ> ]

 * FOLLOW<����> = {SIMICN, RPARENT}, ��FIRST(<��ϵ�����> <���ʽ>)�޽���
 * FIRST(<��ϵ�����> <���ʽ>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
void GrammaticalParser::__condition(int level) {
	FLAG_ENTER("<����>", level);

	try {
		bool char_detector = false;
		__expression(level + 1, &char_detector);
		// RECUR_CHECK(__expression, RECUR_DEFAULT);

		if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
			_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
			__rel_operator(level + 1);									// <��ϵ�����>
			__expression(level + 1, &char_detector);					// <���ʽ>
		}
		// uni4-error-f
		if (char_detector) {
			_register_error(token->line, ErrorType::ConditionError);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
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
void GrammaticalParser::__loop_statement(int level, bool* has_return)
{
	FLAG_ENTER("<ѭ�����>", level);
	try {
		// while while '('��������')'����䣾
		if (_peek()->equal(SYMBOL::WHILETK)) {
			SYMBOL_CHECK(SYMBOL::WHILETK);					// while
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			__condition(level + 1);							// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// ��
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(level +1 , has_return);
		}
		// do while do����䣾while '('��������')'
		else if (_peek()->equal(SYMBOL::DOTK)) {
			SYMBOL_CHECK(SYMBOL::DOTK);					// do
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(level + 1, has_return);
			try {
				SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>
			}
			catch (ParseException& e) {
				_register_error(token->line, ErrorType::MissWhile);
			}

			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			__condition(level + 1);									// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
		}
		// for: for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
		else if (_peek()->equal(SYMBOL::FORTK)) {
			SYMBOL_CHECK(SYMBOL::FORTK);					// for
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			__idenfr(level + 1, false);											// <��ʶ��>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			__expression(level + 1, NULL);										// <���ʽ>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			__condition(level + 1);												// <����>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			__idenfr(level + 1, false);											// <��ʶ��>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			__idenfr(level + 1, false);											// <��ʶ��>
			if (_peek()->equal(SYMBOL::PLUS)) {					// (+ | -)
				SYMBOL_CHECK(SYMBOL::PLUS);
			}
			else {
				SYMBOL_CHECK(SYMBOL::MINU);
			}
			__step_length(level + 1);											// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(level + 1, has_return);
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
 * ��������::= ���޷���������
*/
void GrammaticalParser::__step_length(int level) {
	FLAG_ENTER("<����>", level);
	try {
		__unsigned_integer(level + 1);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	
}

/**
 * ���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���з���ֵ����������䣾) = {<��ʶ��>}
*/
void GrammaticalParser::__function_call_return(int level) {
	FLAG_ENTER("<�з���ֵ�����������>", level);

	try {
		__idenfr(level + 1, false);
		SymbolItem* p = find_func(symbol_table.get_present_block(),token->token, true);
		// ���û�еĻ���ֱ�Ӻ�����仰
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {
			SYMBOL_CHECK(SYMBOL::LPARENT);

			__value_parameter_list(level+1 , &(p->paramsList));
			// RECUR_CHECK(__value_parameter_list, RECUR_DEFAULT);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ���޷���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���޷���ֵ����������䣾) = {<��ʶ��>}
*/
void GrammaticalParser::__function_call_void(int level) {
	FLAG_ENTER("<�޷���ֵ�����������>", level);

	try {
		__idenfr(level + 1, false);
		SymbolItem* p = find_func(symbol_table.get_present_block(), token->token, true);
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {

			SYMBOL_CHECK(SYMBOL::LPARENT);
			__value_parameter_list(level + 1, &(p->paramsList));
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ��ֵ������::= �����ʽ��{,�����ʽ��}�����գ�
 * FISRT() = FIRST(<���ʽ>)
 * FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * FOLLOW(<ֵ������>) = {RPARENT}
 *
 * !: Ҳ���Բ�����FOLLOW�жϿյ����������ö��FIRST��©
*/
void GrammaticalParser::__value_parameter_list(int level, vector<SymbolItem*>* params)
{
	FLAG_ENTER("<ֵ������>", level);

	try {
		SYMBOL first_list[] = { SYMBOL::IDENFR, SYMBOL::LPARENT,
					 SYMBOL::PLUS, SYMBOL::MINU, SYMBOL::INTCON,
					 SYMBOL::CHARCON };

		// <���ʽ>
		int cnt = 0;
		bool char_detector = false;
		vector<string> temp;
		if (_peek()->equal(first_list, 6)) {
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			__expression(level + 1, &char_detector);
			temp.push_back((char_detector ? "char" : "int"));
			cnt += 1;
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				// RECUR_CHECK(__expression, RECUR_DEFAULT);
				char_detector = false;
				__expression(level + 1, &char_detector);
				temp.push_back((char_detector ? "char" : "int"));
				cnt += 1;
			}
		}
		else {
			// <��> : ignore
		}
		// unit4-error-d
		if (cnt != params->size()) {
			_register_error(token->line, ErrorType::FuncParamNumUnmatch);
		}
		// unit4-error-e
		auto it1 = temp.begin();
		auto it2 = params->begin();
		for ( ; it1 != temp.end() && it2 != params->end(); it1++, it2++) {
			string std = (*it2)->var_type == BasicType::_char ? "char" : "int";
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
 * ������У�::=������䣾��
 * FIRST(<���>) =
 * {IFTK} + {WHILETK} + {DOTK} + {FORTK} + {LBRACE} + {IDENFR*3} + {IDENFR} + {SCANFTK} + {PRINTFTK}
 * + {SEMICN} + {RETURNTK}
 * FOLLOW(<�����>) = {RBRACE}, ���<��>ѡ����<���>ѡ�񲻴��ڻ������⡣
 *
 * !: Ҳ���Բ����ж�follow���ϵķ�ʽ������ö��FISRT��©.
*/
void GrammaticalParser::__statement_list(int level, bool * has_return) {
	FLAG_ENTER("<�����>", level);

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
 * ������䣾::=  scanf '('����ʶ����{,����ʶ����}')'
 * FIRST(<�����>) = {SCANFTK}
*/
void GrammaticalParser::__read_statement(int level) {
	FLAG_ENTER("<�����>", level);

	try {
		SYMBOL_CHECK(SYMBOL::SCANFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		__idenfr(level + 1, false);
		while (_peek()->equal(SYMBOL::COMMA)) {
			SYMBOL_CHECK(SYMBOL::COMMA);
			__idenfr(level + 1, false);
		}
		SYMBOL_CHECK(SYMBOL::RPARENT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	
}

/**
 * ��д��䣾 ::= printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'| printf '('�����ʽ��')'
 * FISRT(<�����>) = {PRINTFTK}
 * FISRT(<�ַ���>) = {STRCON},
 * FISRT(<���ʽ>) = {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 *
 * !: ���ж��ַ�������ʽ����ʱ��Ҳ���Բ���if-else�ͣ����ַ��������ʽ��
*/
void GrammaticalParser::__write_statement(int level) {
	FLAG_ENTER("<д���>", level);

	try {
		SYMBOL exp_first[] =
		{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
		  SYMBOL::INTCON, SYMBOL::CHARCON };

		SYMBOL_CHECK(SYMBOL::PRINTFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);

		// printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'
		if (_peek()->equal(SYMBOL::STRCON)) {
			__string(level + 1);
			if (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				__expression(level + 1, NULL);
			}
		}
		// printf '('�����ʽ��')'
		else if (_peek()->equal(exp_first, 6)) {
			__expression(level + 1, NULL);
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
	
}

/**
 * ��������䣾::=  return['('�����ʽ��')']
 * FISRT('('�����ʽ��')') = {LPARENT};
 * FOLLOW{<�������>} = {SEMICN}���޽��漯��
*/
void GrammaticalParser::__return_statement(int level, bool * has_return) {
	FLAG_ENTER("<�������>", level);

	try {
		SYMBOL_CHECK(SYMBOL::RETURNTK);
		Block* block = symbol_table.get_present_block();
		*has_return = true;
		if (_peek()->equal(LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			bool char_detector = false;
			__expression(level + 1, &char_detector);
			// unit4-error-h
			if (block->func_head->return_type == BasicType::_int && char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			else if (block->func_head->return_type == BasicType::_char && !char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			else if (block->func_head->return_type == BasicType::_void) {
				_register_error(token->line, ErrorType::VoidWithReturn);
			}
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
		else {
			if (block->func_head->return_type == BasicType::_int || block->func_head->return_type == BasicType::_char)
			{
				_register_error(token->line, ErrorType::ReturnError);
			}
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
}