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
	__program(PARSE_HEAD{1});
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
PARSE_RETURN GrammaticalParser::__add_operator(PARSE_HEAD head) {
	FLAG_ENTER("<�ӷ������>",head.level);
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
 * ���˷��������  ::= *��/
 * FIRST(*) = {MULT}, FIRST(/) = {DIV}
 */
PARSE_RETURN GrammaticalParser::__mult_operator(PARSE_HEAD head) {
	FLAG_ENTER("<�˷������>", head.level);
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
 * ����ϵ�������  ::=  <��<=��>��>=��!=��==
 * FIRST��...
*/
PARSE_RETURN GrammaticalParser::__rel_operator(PARSE_HEAD head) {
	FLAG_ENTER("<��ϵ�����>", head.level);
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
 * ����ĸ��::= �ߣ�a����������z��A����������Z
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__letter(PARSE_HEAD head) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<��ĸ>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Char>"));
}

/**
 * �����֣�::= �������������֣�
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__number(PARSE_HEAD head) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<����>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Number>"));
}

/**
 * ���������֣�::= ��������������
 * FIRST: ...
 */
PARSE_RETURN GrammaticalParser::__non_zero_number(PARSE_HEAD head) {
	// �ʷ��������㣬�÷��ս��������
	FLAG_ENTER("<��������>", head.level);
	FLAG_FAIL;
	throw ParseException(ParseExceptionType::Unexpected, string("<Non-Zero Number>"));
}

/**
 * ���ַ���::=  '���ӷ��������'��'���˷��������'��'����ĸ��'��'�����֣�'
 * FIRST: '
 * !: �ʷ�����ʱ�����Ѿ������˵�����'���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�
*/
PARSE_RETURN GrammaticalParser::__char(PARSE_HEAD head) {
	FLAG_ENTER("<�ַ�>", head.level);
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
 * ���ַ�����::=  "��ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ���"
 * FIRST: "
 * !: �ʷ�����ʱ�����Ѿ�������˫����"���ж����ַ�����˸ù���ƥ�䲻��ȫ�ϸ�.
*/
PARSE_RETURN GrammaticalParser::__string(PARSE_HEAD head) {
	FLAG_ENTER("<�ַ���>", head.level);
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
PARSE_RETURN GrammaticalParser::__program(PARSE_HEAD head)
{
	FLAG_ENTER("<����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		// <����˵��>
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			RECUR_CHECK(__const_description, RECUR_DEFAULT);
		}

		// <����˵��>: �÷����жϣ�<����˵��>��<�з���ֵ�ĺ�������>�Ӻ����������ַ�����ֲ���
		if ((_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			RECUR_CHECK(__var_description, RECUR_DEFAULT);
		}

		// {���з���ֵ�������壾 | ���޷���ֵ�������壾}
		// ���޷���ֵ�������壾��Ҫ�ų�void main
		while (
			(_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) ||
			(_peek()->equal(SYMBOL::VOIDTK) && !_peek(2)->equal(SYMBOL::MAINTK))
			)
		{
			if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
				RECUR_CHECK(__function_return, RECUR_DEFAULT);
			}
			else {
				RECUR_CHECK(__function_void, RECUR_DEFAULT);
			}
		}
		// <������>
		RECUR_CHECK(__main_function, RECUR_DEFAULT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ������˵���� ::=  const���������壾;{ const���������壾;}
 * FISRT(<����˵��>) = {CONSTTK}
 * FOLLOW(<����˵��>) = {...}
 * ������FISRT(< const���������壾;>)��FOLLOW(<����˵��>)���Ͻ��������
*/
PARSE_RETURN GrammaticalParser::__const_description(PARSE_HEAD head)
{
	FLAG_ENTER("<����˵��>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL_CHECK(SYMBOL::CONSTTK);
		RECUR_CHECK(__const_def, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		while (_peek()->equal(SYMBOL::CONSTTK)) {
			SYMBOL_CHECK(SYMBOL::CONSTTK);
			RECUR_CHECK(__const_def, RECUR_DEFAULT);
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
 * ���������壾::=int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{,����ʶ���������ַ���}
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
 * FOLLOW(<��������>) = {SEMICN}
 * ������FOLLOW(<��������>)��FIRST(,����ʶ��������������| <�ַ�>) �ཻ�����
*/
PARSE_RETURN GrammaticalParser::__const_def(PARSE_HEAD head)
{
	FLAG_ENTER("<��������>", head.level);
	PARSE_HEAD RECUR_def;RECUR_def.level = head.level + 1;RECUR_def.is_def = true;
	PARSE_HEAD RECUR_DEFAULT = { head.level + 1 };

	try {
		if (_peek()->equal(SYMBOL::INTTK)) {
			SYMBOL_CHECK(SYMBOL::INTTK);
			RECUR_CHECK(__idenfr, RECUR_def);								// <��ʶ��>
			string name = token->token;
			SYMBOL_CHECK(SYMBOL::ASSIGN);									// ��ֵ����
			// unit4-error-o
			try{
				RECUR_CHECK(__integer, RECUR_DEFAULT);							// <����>
				// ���������Ķ���
				if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
					while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
						_next();
					_register_error(token->line, ErrorType::ConstDefWrong);
				}
			}
			catch (ParseException& e) {
				_register_error(token->line, ErrorType::ConstDefWrong);
			}
			// ��ȷ��������������
			insert_one_record(symbol_table.get_present_block(),
				ParamRecord{ name, "int", true, false });
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__idenfr, RECUR_def);
				string name = token->token;
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				// unit4-error-o
				try {
					RECUR_CHECK(__integer, RECUR_DEFAULT);
					// ���������Ķ���
					if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
						while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
							_next();
						_register_error(token->line, ErrorType::ConstDefWrong);
					}
				}
				catch (ParseException& e) {
					_register_error(token->line, ErrorType::ConstDefWrong);
				}
				// ��ȷ��������������
				insert_one_record(symbol_table.get_present_block(),
					ParamRecord{ name, "int", true, false });
			}
		}
		else if (_peek()->equal(SYMBOL::CHARTK)) {
			SYMBOL_CHECK(SYMBOL::CHARTK);
			RECUR_CHECK(__idenfr, RECUR_def);				// <��ʶ��>
			string name = token->token;
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// ��ֵ����
			// unit4-error-o
			try {
				RECUR_CHECK(__char, RECUR_DEFAULT);				// <������>
				// ���������Ķ���
				if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
					while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
						_next();
					_register_error(token->line, ErrorType::ConstDefWrong);
				}
			}
			catch (ParseException& e) {
				_register_error(token->line, ErrorType::ConstDefWrong);
			}
			// ��ȷ��������������
			insert_one_record(symbol_table.get_present_block(),
				ParamRecord{ name, "char", true, false });
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__idenfr, RECUR_def);
				string name = token->token;
				SYMBOL_CHECK(SYMBOL::ASSIGN);
				// unit4-error-o
				try {
					RECUR_CHECK(__char, RECUR_DEFAULT);
					// ���������Ķ���
					if (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN)))) {
						while (!(_peek()->equal(SYMBOL::COMMA) || (_peek()->equal(SYMBOL::SEMICN))))
							_next();
						_register_error(token->line, ErrorType::ConstDefWrong);
					}
				}
				catch (ParseException& e) {
					_register_error(token->line, ErrorType::ConstDefWrong);
				}
				// ��ȷ��������������
				insert_one_record(symbol_table.get_present_block(),
					ParamRecord{ name, "char", true, false });
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
 * ���޷���������  ::= ���������֣��������֣���| 0
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__unsigned_integer(PARSE_HEAD head) {
	FLAG_ENTER("<�޷�������>", head.level);
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
 * ��������::= �ۣ������ݣ��޷���������
 * FIRST(1) = {PLUS}, FIRST(2) = {MINU}, FIRST(3) = {INTCON}
*/
PARSE_RETURN GrammaticalParser::__integer(PARSE_HEAD head)
{
	FLAG_ENTER("<����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		if (_peek()->equal(SYMBOL::PLUS)) {
			SYMBOL_CHECK(SYMBOL::PLUS);
		}
		else if (_peek()->equal(SYMBOL::MINU)) {
			SYMBOL_CHECK(SYMBOL::MINU);
		}
		RECUR_CHECK(__unsigned_integer, RECUR_DEFAULT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ����ʶ����::=  ����ĸ��������ĸ���������֣���
 * FIRST: ...
*/
PARSE_RETURN GrammaticalParser::__idenfr(PARSE_HEAD head) {
	FLAG_ENTER("<��ʶ��>", head.level);
	try {
		SYMBOL_CHECK(SYMBOL::IDENFR);
		// unit4error - b 
		// unit4error - c
		void* p = find_indefr(symbol_table.get_present_block(), token->token);
		if (head.is_def && p != NULL) {
			_register_error(token->line, ErrorType::Redefine);
		}
		if (!head.is_def && p == NULL) {
			_register_error(token->line, ErrorType::Undefine);
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
 * ������ͷ����::=  int����ʶ���� | char����ʶ����
 * FIRST(1) = {INTTK}, FIRST(2) = {CHARTK}
*/
PARSE_RETURN GrammaticalParser::__declar_head(PARSE_HEAD head) {
	FLAG_ENTER("<����ͷ��>", head.level);
	PARSE_HEAD RECUR_def; RECUR_def.level = head.level + 1; RECUR_def.is_def = true;
	try {
		if (_peek()->equal(SYMBOL::INTTK))
		{
			SYMBOL_CHECK(SYMBOL::INTTK);
			RECUR_CHECK(__idenfr, RECUR_def);
		}
		else if (_peek()->equal(SYMBOL::CHARTK)) {
			SYMBOL_CHECK(SYMBOL::CHARTK);
			RECUR_CHECK(__idenfr, RECUR_def);
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
 * ������˵����::= ���������壾;{���������壾;}
 * FIRST(<����˵��>) = FISRT(<��������>) = {INTTK, CHARTK}
 * FOLLOW(<����˵��>) = FISRT(<�з���ֵ��������>) + FISRT(<�޷���ֵ��������>) + FIRST(<������>) + FIRST(<�����>)
 * {INTTK, CHARTK} // _peek(3) = ( ?
 * {VOIDTK}		   // _peek() = VOIDTK
 * {VOIDTK}        // _peek() = VOIDTK
 * {IFTK, WHILETK, DOTK, FORTK, LBRACE, INTTK, CHARTK,    // _peek(3) = (?
 *  VOIDTK, IDENFR, SCANFTK, PRINTFTK, SEMICN, RETURNTK}
 */
PARSE_RETURN GrammaticalParser::__var_description(PARSE_HEAD head)
{
	FLAG_ENTER("<����˵��>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL fisrt_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		RECUR_CHECK(__var_def, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::SEMICN);

		// �÷����������ο�FIRST���ϵĲ���
		while (_peek()->equal(fisrt_list, 2) && !_peek(3)->equal(SYMBOL::LPARENT)) {
			RECUR_CHECK(__var_def, RECUR_DEFAULT);
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
 * ���������壾::= �����ͱ�ʶ����(����ʶ����|����ʶ����'['���޷���������']'){,(����ʶ����|����ʶ����'['���޷���������']' )}
 * FOLLOW(<��������>) = {SEMICN}, FISRT(, <��ʶ��>...) �� FIRST(<'[' <�޷��ź���> ']'>)û�н���.
 * FOLLOW(<��������>) = {SEMICN} �� FIRST(<, <��ʶ��>>) û�н���
 * * �������������������������[]ȡ��ȡ����ʾ
*/
PARSE_RETURN GrammaticalParser::__var_def(PARSE_HEAD head)
{
	FLAG_ENTER("<��������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	PARSE_HEAD RECUR_def; RECUR_def.level = head.level + 1; RECUR_def.is_def = true;
	try {
		RECUR_CHECK(__type_idenfr, RECUR_DEFAULT);
		string type = (token->symbol == SYMBOL::CHARCON) ? "char" : "int";
		
		RECUR_CHECK(__idenfr, RECUR_def);
		string idenfr_name = token->token;
		
		// '[' <�޷�������> ']'
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			RECUR_CHECK(__unsigned_integer, RECUR_DEFAULT);
			SYMBOL_CHECK(SYMBOL::RBRACK);
			insert_one_record(symbol_table.get_present_block(),
				ParamRecord{ idenfr_name, type, false, true });
		}
		else {
			insert_one_record(symbol_table.get_present_block(),
				ParamRecord{ idenfr_name, type, false, false });
		}
		
		// {} 
		while (_peek()->equal(SYMBOL::COMMA)) {
			SYMBOL_CHECK(SYMBOL::COMMA);
			RECUR_CHECK(__idenfr, RECUR_def);
			idenfr_name = token->token;
			// '[' <�޷�������> ']'
			if (_peek()->equal(SYMBOL::LBRACK)) {
				SYMBOL_CHECK(SYMBOL::LBRACK);
				RECUR_CHECK(__unsigned_integer, RECUR_DEFAULT);
				SYMBOL_CHECK(SYMBOL::RBRACK);
				insert_one_record(symbol_table.get_present_block(),
					ParamRecord{ idenfr_name, type, false, true });
			}
			else {
				insert_one_record(symbol_table.get_present_block(),
					ParamRecord{ idenfr_name, type, false, false });
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
 * �����ͱ�ʶ����::=  int | char
*/
PARSE_RETURN GrammaticalParser::__type_idenfr(PARSE_HEAD head) {
	FLAG_ENTER("<���ͱ�ʶ��>", head.level);
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
 * ���з���ֵ�������壾  ::=  ������ͷ����'('��������')' '{'��������䣾'}'
*/
PARSE_RETURN GrammaticalParser::__function_return(PARSE_HEAD head) {
	FLAG_ENTER("<�з���ֵ��������>", head.level);
	FuncHead func_head;

	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	PARSE_HEAD RECUR_def; RECUR_def.level = head.level + 1; RECUR_def.is_def = true;
	PARSE_HEAD RECUR_params = RECUR_DEFAULT; RECUR_params.is_def = true; RECUR_params.func_head = &func_head;

	symbol_table.add_one_block();												// ���뵱ǰ��

	try {
		string func_type = (_peek()->symbol == SYMBOL::CHARCON) ? "char" : "int";
		RECUR_CHECK(__declar_head, RECUR_def);
		string func_name = token->token;
		func_head.name = func_name;
		func_head.returnType = func_type;

		Token save = *token;

		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__parameter_list, RECUR_params);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		update_function_head(symbol_table.get_present_block(), func_head);
		
		SYMBOL_CHECK(SYMBOL::LBRACE);
		bool has_return = false;
		__compound_statement(RECUR_DEFAULT, &has_return);
		// unit4-error h
		if (!has_return) {
			_register_error(token->line, ErrorType::ReturnError);
		}
		// RECUR_CHECK(__compound_statement, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::RBRACE);

		func_call_return_idenfr.push_back(save.token);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;

	symbol_table.exit_present_block();											// �뿪��ǰ��

	return PARSE_RETURN{ 0 };
}

/**
 * ���޷���ֵ�������壾  ::= void����ʶ����'('��������')''{'��������䣾'}'
*/
PARSE_RETURN GrammaticalParser::__function_void(PARSE_HEAD head)
{
	FLAG_ENTER("<�޷���ֵ��������>", head.level);
	FuncHead func_head;

	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	PARSE_HEAD RECUR_def; RECUR_def.level = head.level + 1; RECUR_def.is_def = true;
	PARSE_HEAD RECUR_params = RECUR_DEFAULT; RECUR_params.is_def = true; RECUR_params.func_head = &func_head;

	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);
		string type = "void";
		RECUR_CHECK(__idenfr, RECUR_def);
		string func_name = token->token;
		func_head.name = func_name;
		func_head.returnType = type;

		Token save = *token;

		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__parameter_list, RECUR_params);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		update_function_head(symbol_table.get_present_block(), func_head);

		SYMBOL_CHECK(SYMBOL::LBRACE);

		bool has_return = false;
		__compound_statement(RECUR_DEFAULT, &has_return);
		if (has_return) {
			_register_error(token->line, ErrorType::VoidWithReturn);
		}

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
 * ��������䣾::=�ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
 * FISRT(<����˵��>) = {CONSTK}�� FIRST(<����˵��>) = {FIRST(<��������>)} = {INTTK, CHARTK}
 * FOLLOW(...<[����˵��]>) = FISRT(<�����>) �� FISRT(<����˵��>) û�н���
*/
PARSE_RETURN GrammaticalParser::__compound_statement(PARSE_HEAD head, bool *has_return) {
	FLAG_ENTER("<�������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL first_list[] = { SYMBOL::INTTK, SYMBOL::CHARTK };
		// [<const des>]
		if (_peek()->equal(SYMBOL::CONSTTK)) {
			RECUR_CHECK(__const_description, RECUR_DEFAULT);
		}
		// [<var des>]
		if (_peek()->equal(first_list, 2)) {
			RECUR_CHECK(__var_description, RECUR_DEFAULT);
		}
		__statement_list(RECUR_DEFAULT, has_return);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ��������::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����} | ���գ�
 * �������ݵ��ķ�������ʽ��1.����ѡ���FIRST���ϲ����н����� 2.����һ���ա� 3.�����յ�ѡ�񣬷ǿյ����˳��������в��ܺ���FOLLOW(<������>)
 * FIRST(1) = {INTTK, CHARTK}, FOLLOW(������) =  {RPARENT}
 * FOLLOW(<��ʶ��>) = {RPARENT} �� FIRST(<,<���ͱ�ʶ��><��ʶ��>>) �޽���
*/
PARSE_RETURN GrammaticalParser::__parameter_list(PARSE_HEAD head)
{
	FLAG_ENTER("<������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	PARSE_HEAD RECUR_def = PARSE_HEAD{head.level+1, true};

	FuncHead* func_head = head.func_head;

	try {
		// ~<��>
		if (_peek()->equal(SYMBOL::INTTK) || _peek()->equal(SYMBOL::CHARTK)) {
			RECUR_CHECK(__type_idenfr, RECUR_DEFAULT);
			// �������ͱ�ʾ
			string type = (token->symbol == SYMBOL::CHARCON) ? "char" : "int";
			func_head->paramsList.push_back(type);
			RECUR_CHECK(__idenfr, RECUR_def);
			// ������ű���
			string name = token->token;
			ParamRecord record{ name, type, false, false };
			insert_one_record(symbol_table.get_present_block(), record);
			while (_peek()->equal(SYMBOL::COMMA))
			{
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__type_idenfr, RECUR_DEFAULT);
				// �������ͱ�ʾ
				string type = (token->symbol == SYMBOL::CHARCON) ? "char" : "int";
				func_head->paramsList.push_back(type);
				RECUR_CHECK(__idenfr, RECUR_def);
				string name = token->token;
				// ������ű���
				ParamRecord record{ name, type, false, false };
				insert_one_record(symbol_table.get_present_block(), record);
			}
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
	return PARSE_RETURN{ 0 };
}

/**
 * ����������::= void main��(����)�� ��{����������䣾��}��
 * FIRST(x): {VOIDTK}
*/
PARSE_RETURN GrammaticalParser::__main_function(PARSE_HEAD head)
{
	FLAG_ENTER("<������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL_CHECK(SYMBOL::VOIDTK);				// void
		SYMBOL_CHECK(SYMBOL::MAINTK);				// main
		SYMBOL_CHECK(SYMBOL::LPARENT);				// (
		SYMBOL_CHECK(SYMBOL::RPARENT);				// )
		SYMBOL_CHECK(SYMBOL::LBRACE);				// {
		bool has_return = false;
		// RECUR_CHECK(__compound_statement, RECUR_DEFAULT);			// <�������>
		__compound_statement(RECUR_DEFAULT, &has_return);
		if (has_return) {
			_register_error(token->line, ErrorType::VoidWithReturn);
		}
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
 * �����ʽ��::= �ۣ������ݣ��{���ӷ�����������}   //[+|-]ֻ�����ڵ�һ��<��>
 * FIRST(<���ʽ>): {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * ���ʽ������ ++5 - 1 ���������
 *
 * FIRST(<�ӷ������>) = {PLUS, MINU}
 * FOLLOW(<���ʽ>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA} + {SEMICN}(<��ֵ�������>)
 * FIRST<�ӷ������> �� FOLLOW<���ʽ> û�н���, ����޻���.
*/
PARSE_RETURN GrammaticalParser::__expression(PARSE_HEAD head, bool *is_char=NULL) {
	FLAG_ENTER("<���ʽ>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
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
		__item(RECUR_DEFAULT, &temp);
		cnt++;
		while (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU)) {
			RECUR_CHECK(__add_operator, RECUR_DEFAULT);
			// RECUR_CHECK(__item, RECUR_DEFAULT);
			bool tempx = false;
			__item(RECUR_DEFAULT, &tempx);
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
	return PARSE_RETURN{ 0 };
}


/**
 * ���::= �����ӣ�{���˷�������������ӣ�}
 * FIRST(<��>) = FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}

 * FIRST(<�˷������>) = {MULT, DIV}
 * FOLLOW(<��>) = {RBRACK, RPARENT, GRE,GEQ,LSS,LEQ,NEQ,EQL, COMMA, SEMICN} + {PLUS, MINU}
 * FIRST �� FOLLOW �������ʲ����ڻ���
*/
PARSE_RETURN GrammaticalParser::__item(PARSE_HEAD head, bool *is_char) {
	FLAG_ENTER("<��>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	int cnt = 0;
	try {
		bool temp = false;
		__factor(RECUR_DEFAULT, &temp);
		cnt++;
		// RECUR_CHECK(__factor, RECUR_DEFAULT);
		while (_peek()->equal(SYMBOL::MULT) || _peek()->equal(SYMBOL::DIV)) {
			RECUR_CHECK(__mult_operator, RECUR_DEFAULT);
			bool tempx = false;
			// RECUR_CHECK(__factor, RECUR_DEFAULT);
			__factor(RECUR_DEFAULT, &tempx);
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
	return PARSE_RETURN{ 0 };
}

/**
 * �����ӣ�::= ����ʶ���� �� ����ʶ����'['�����ʽ��']' | '('�����ʽ��')' �� �������� | ���ַ��������з���ֵ����������䣾
 * FIRST(<����>) = {IDENFR} + {IDENFR} + {RPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * !: ���ӵ�ǰ����ѡ������FIRST���ϲ��ཻ��ԭ�򣬸�дΪ<��ʶ��>[ '[' <���ʽ> ']' ]��
 *    ����FIRST([...]) = {LBRACK} �� FOLLOW{<����>} = {MULT, DIV} + FOLLOW(<��>)û�н���
*/
PARSE_RETURN GrammaticalParser::__factor(PARSE_HEAD head, bool *is_char) {
	FLAG_ENTER("<����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		// <��ʶ��> [ '[' <���ʽ> ']' ] 
		if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT)) {
			// SYMBOL_CHECK(SYMBOL::IDENFR);
			RECUR_CHECK(__idenfr, RECUR_DEFAULT);
			// �ڷ��ű��в��ұ�������
			void* p = find_indefr(symbol_table.get_present_block(), token->token);
			if (p != NULL) {
				ParamRecord* param_p = (ParamRecord*)p;
				if (param_p->type == "char" && is_char) {
					*is_char = true;
				}
			}
			if (_peek()->equal(SYMBOL::LBRACK)) {
				bool char_detector = false;
				SYMBOL_CHECK(SYMBOL::LBRACK);
				__expression(RECUR_DEFAULT, &char_detector);
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
			RECUR_CHECK(__function_call_return, RECUR_DEFAULT);
			// �ڷ��ű��в��Һ�������
			void* p = find_indefr(symbol_table.get_present_block(), name);
			if (p != NULL) {
				FuncHead* func_p = (FuncHead*)p;
				if (func_p->returnType == "char" && is_char) {
					*is_char = true;
				}
			}
		}
		// '(' <���ʽ> ')'
		else if (_peek()->equal(SYMBOL::LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			RECUR_CHECK(__expression, RECUR_DEFAULT);
			SYMBOL_CHECK(SYMBOL::RPARENT);
		}
		// <����>
		else if (_peek()->equal(SYMBOL::PLUS) || _peek()->equal(SYMBOL::MINU) || _peek()->equal(SYMBOL::INTCON)) {
			RECUR_CHECK(__integer, RECUR_DEFAULT);
		}
		// <�ַ�>
		else if (_peek()->equal(SYMBOL::CHARCON)) {
			RECUR_CHECK(__char, RECUR_DEFAULT);
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
	return PARSE_RETURN{ 0 };
}

/**
 * ����䣾::= ��������䣾����ѭ����䣾| '{'������У�'}'| ���з���ֵ����������䣾;
			 |���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|��������䣾;
 * FIRST = IFTK / WHILETK, DOTK, FORTK / LBRACE / IDENFR / IDENFR / IDENFR / SCANFTK / PRINTFTK / SEMICN / RETURNTK
 * !: FISRT(���з���ֵ����������䣾)��FISRT(<��ֵ���>) ���غϣ���Ҫ��_peek(2) = (
*/
PARSE_RETURN GrammaticalParser::__statement(PARSE_HEAD head, bool * has_return)
{
	FLAG_ENTER("<���>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL loop_first[] = { SYMBOL::WHILETK, SYMBOL::FORTK, SYMBOL::DOTK };

		if (_peek()->equal(SYMBOL::IFTK)) { __condition_statement(RECUR_DEFAULT, has_return); }
		else if (_peek()->equal(loop_first, 3)) { __loop_statement(RECUR_DEFAULT, has_return); }
		else if (_peek()->equal(SYMBOL::LBRACE)) {
			SYMBOL_CHECK(SYMBOL::LBRACE);
			__statement_list(RECUR_DEFAULT, has_return);
			SYMBOL_CHECK(SYMBOL::RBRACE);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && _peek(2)->equal(SYMBOL::LPARENT))
		{
			// �з���ֵ�����������޷���ֵ����������ʽ��ȫһ�£���Ҫ��ǰʹ�÷��ű����С�
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
			if (x == 1) { RECUR_CHECK(__function_call_return, RECUR_DEFAULT) }
			else if (x == -1) { RECUR_CHECK(__function_call_void, RECUR_DEFAULT); }
			else { FLAG_FAIL; }	// ������Ϊ�������ı�ʶ����û���ҵ���Ӧ�ġ�
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::IDENFR) && !_peek(2)->equal(SYMBOL::LPARENT))
		{
			RECUR_CHECK(__assign_statment, RECUR_DEFAULT);
			SYMBOL_CHECK(SYMBOL::SEMICN);
		}
		else if (_peek()->equal(SYMBOL::SCANFTK)) { RECUR_CHECK(__read_statement, RECUR_DEFAULT); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::PRINTFTK)) { RECUR_CHECK(__write_statement, RECUR_DEFAULT); SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::SEMICN)) { SYMBOL_CHECK(SYMBOL::SEMICN); }
		else if (_peek()->equal(SYMBOL::RETURNTK)) { 
			// RECUR_CHECK(__return_statement, RECUR_DEFAULT);
			__return_statement(RECUR_DEFAULT, has_return);
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
	return PARSE_RETURN{ 0 };
}


/**
 * ����ֵ��䣾::= ����ʶ�����������ʽ��|����ʶ����'['�����ʽ��']'=�����ʽ��
 * �����д <��ֵ���> ::= <��ʶ��> ['[' <���ʽ> ']'] = <���ʽ>
 * FIRST(<��ֵ���>) = {IDENFR}
*/
PARSE_RETURN GrammaticalParser::__assign_statment(PARSE_HEAD head) {
	FLAG_ENTER("<��ֵ���>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	
	try {
		RECUR_CHECK(__idenfr, RECUR_DEFAULT);
		// SYMBOL_CHECK(SYMBOL::IDENFR);
		void* p = find_indefr(symbol_table.get_present_block(), token->token);
		if (p!=NULL) {
			ParamRecord* param_p = (ParamRecord*)p;
			if (param_p->isConst) {
				_register_error(token->line, ErrorType::ConstModify);
			}
		}
		if (_peek()->equal(SYMBOL::LBRACK)) {
			SYMBOL_CHECK(SYMBOL::LBRACK);
			bool char_detector = false;
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			__expression(RECUR_DEFAULT, &char_detector);
			// unit4-error-i
			if (char_detector) {
				_register_error(token->line, ErrorType::ArrayIndexWrong);
			}
			SYMBOL_CHECK(SYMBOL::RBRACK);
		}
		SYMBOL_CHECK(SYMBOL::ASSIGN);					// ASSIGN: = ,  EQL: ==
		RECUR_CHECK(__expression, RECUR_DEFAULT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}

	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ��������䣾  ::= if '('��������')'����䣾��else����䣾��
 * FISRT(<�������>) = {IFTK}
 *
 * FOLLOW(<�������>) = {RBRACE} // <���> + <�����> ��follow
 * FIRST(else <���>) = {else}
 * ����if+һ��else��������ķ��㼶�����������壬��ʱ��Ҫ��Ϊ�涨���ȼ����򣬽���̰��ƥ�䣬elseճճ�������if���ɡ�
*/
PARSE_RETURN GrammaticalParser::__condition_statement(PARSE_HEAD head, bool* has_return) {
	FLAG_ENTER("<�������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		SYMBOL_CHECK(SYMBOL::IFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__condition, RECUR_DEFAULT);
		SYMBOL_CHECK(SYMBOL::RPARENT);

		// RECUR_CHECK(__statement, RECUR_DEFAULT);
		__statement(RECUR_DEFAULT, has_return);

		if (_peek()->equal(SYMBOL::ELSETK)) {
			SYMBOL_CHECK(SYMBOL::ELSETK);
			// RECUR_CHECK(__statement, RECUR_DEFAULT);
			__statement(RECUR_DEFAULT, has_return);
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
 * �������� ::=  �����ʽ������ϵ������������ʽ���������ʽ��
 * FIRST(<����>) = FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * �����д��<����> ::= <���ʽ>  [ <��ϵ�����> <���ʽ> ]

 * FOLLOW<����> = {SIMICN, RPARENT}, ��FIRST(<��ϵ�����> <���ʽ>)�޽���
 * FIRST(<��ϵ�����> <���ʽ>) = {GRE, GEQ, LSS, LEQ, NEQ, EQL}
*/
PARSE_RETURN GrammaticalParser::__condition(PARSE_HEAD head) {
	FLAG_ENTER("<����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		bool char_detector = false;
		__expression(RECUR_DEFAULT, &char_detector);
		// RECUR_CHECK(__expression, RECUR_DEFAULT);

		if (_peek()->equal(SYMBOL::GRE) || _peek()->equal(SYMBOL::GEQ) || _peek()->equal(SYMBOL::LSS) ||
			_peek()->equal(SYMBOL::LEQ) || _peek()->equal(SYMBOL::NEQ) || _peek()->equal(SYMBOL::EQL)) {
			RECUR_CHECK(__rel_operator, RECUR_DEFAULT);				// <��ϵ�����>
			// RECUR_CHECK(__expression, RECUR_DEFAULT);				// <���ʽ>
			__expression(RECUR_DEFAULT, &char_detector);
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
	return PARSE_RETURN{ 0 };
}

/**
 * ��ѭ����䣾::= while '('��������')'����䣾 |
				  do����䣾while '('��������')' |
				  for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
 * FISRT(<ѭ�����>) = {WHILETK, DOTK, FORTK}
 * FIRST(1) != FIRST(2) != FIRST(3)
*/
PARSE_RETURN GrammaticalParser::__loop_statement(PARSE_HEAD head, bool* has_return)
{
	FLAG_ENTER("<ѭ�����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		// while while '('��������')'����䣾
		if (_peek()->equal(SYMBOL::WHILETK)) {
			SYMBOL_CHECK(SYMBOL::WHILETK);					// while
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__condition, RECUR_DEFAULT);							// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// ��
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(RECUR_DEFAULT, has_return);
		}
		// do while do����䣾while '('��������')'
		else if (_peek()->equal(SYMBOL::DOTK)) {
			SYMBOL_CHECK(SYMBOL::DOTK);					// do
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(RECUR_DEFAULT, has_return);
			try {
				SYMBOL_CHECK(SYMBOL::WHILETK);					// <while>
			}
			catch (ParseException& e) {
				_register_error(token->line, ErrorType::MissWhile);
			}

			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__condition, RECUR_DEFAULT);					// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
		}
		// for: for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
		else if (_peek()->equal(SYMBOL::FORTK)) {
			SYMBOL_CHECK(SYMBOL::FORTK);					// for
			SYMBOL_CHECK(SYMBOL::LPARENT);					// (
			RECUR_CHECK(__idenfr, RECUR_DEFAULT);								// <��ʶ��>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			RECUR_CHECK(__expression, RECUR_DEFAULT);							// <���ʽ>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			RECUR_CHECK(__condition, RECUR_DEFAULT);							// <����>
			SYMBOL_CHECK(SYMBOL::SEMICN);					// ;
			RECUR_CHECK(__idenfr, RECUR_DEFAULT);								// <��ʶ��>
			SYMBOL_CHECK(SYMBOL::ASSIGN);					// =
			RECUR_CHECK(__idenfr, RECUR_DEFAULT);								// <��ʶ��>
			if (_peek()->equal(SYMBOL::PLUS)) {					// (+ | -)
				SYMBOL_CHECK(SYMBOL::PLUS);
			}
			else {
				SYMBOL_CHECK(SYMBOL::MINU);
			}
			RECUR_CHECK(__step_length, RECUR_DEFAULT);							// <����>
			SYMBOL_CHECK(SYMBOL::RPARENT);					// )
			// RECUR_CHECK(__statement, RECUR_DEFAULT);							// <���>
			__statement(RECUR_DEFAULT, has_return);
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
 * ��������::= ���޷���������
*/
PARSE_RETURN GrammaticalParser::__step_length(PARSE_HEAD head) {
	FLAG_ENTER("<����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		RECUR_CHECK(__unsigned_integer, RECUR_DEFAULT);
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}

/**
 * ���з���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���з���ֵ����������䣾) = {<��ʶ��>}
*/
PARSE_RETURN GrammaticalParser::__function_call_return(PARSE_HEAD head) {
	FLAG_ENTER("<�з���ֵ�����������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		RECUR_CHECK(__idenfr, RECUR_DEFAULT);
		void* p = find_indefr(symbol_table.get_present_block(),token->token);
		// ���û�еĻ���ֱ�Ӻ�����仰
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {
			FuncHead* func_p = (FuncHead*)p;
			SYMBOL_CHECK(SYMBOL::LPARENT);
			__value_parameter_list(RECUR_DEFAULT, &(func_p->paramsList));
			// RECUR_CHECK(__value_parameter_list, RECUR_DEFAULT);
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

/**
 * ���޷���ֵ����������䣾 ::= ����ʶ����'('��ֵ������')'
 * FISRT(���޷���ֵ����������䣾) = {<��ʶ��>}
*/
PARSE_RETURN GrammaticalParser::__function_call_void(PARSE_HEAD head) {
	FLAG_ENTER("<�޷���ֵ�����������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		RECUR_CHECK(__idenfr, RECUR_DEFAULT);
		void* p = find_indefr(symbol_table.get_present_block(), token->token);
		if (p == NULL) {
			while (!_peek()->equal(SYMBOL::SEMICN))
				_next();
		}
		else {
			FuncHead* func_p = (FuncHead*)p;

			SYMBOL_CHECK(SYMBOL::LPARENT);
			__value_parameter_list(RECUR_DEFAULT, &(func_p->paramsList));
			// RECUR_CHECK(__value_parameter_list, RECUR_DEFAULT);
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

/**
 * ��ֵ������::= �����ʽ��{,�����ʽ��}�����գ�
 * FISRT() = FIRST(<���ʽ>)
 * FISRT(<���ʽ>) = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 * FOLLOW(<ֵ������>) = {RPARENT}
 *
 * !: Ҳ���Բ�����FOLLOW�жϿյ����������ö��FIRST��©
*/
PARSE_RETURN GrammaticalParser::__value_parameter_list(PARSE_HEAD head, vector<string>* params)
{
	FLAG_ENTER("<ֵ������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

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
			__expression(RECUR_DEFAULT, &char_detector);
			temp.push_back((char_detector ? "char" : "int"));
			cnt += 1;
			while (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				// RECUR_CHECK(__expression, RECUR_DEFAULT);
				char_detector = false;
				__expression(RECUR_DEFAULT, &char_detector);
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
		if (temp != (*params)) {
			_register_error(token->line, ErrorType::FuncParamTypeUnmatch);
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
 * ������У�::=������䣾��
 * FIRST(<���>) =
 * {IFTK} + {WHILETK} + {DOTK} + {FORTK} + {LBRACE} + {IDENFR*3} + {IDENFR} + {SCANFTK} + {PRINTFTK}
 * + {SEMICN} + {RETURNTK}
 * FOLLOW(<�����>) = {RBRACE}, ���<��>ѡ����<���>ѡ�񲻴��ڻ������⡣
 *
 * !: Ҳ���Բ����ж�follow���ϵķ�ʽ������ö��FISRT��©.
*/
PARSE_RETURN GrammaticalParser::__statement_list(PARSE_HEAD head, bool * has_return) {
	FLAG_ENTER("<�����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

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
			__statement(RECUR_DEFAULT, has_return);
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
 * ������䣾::=  scanf '('����ʶ����{,����ʶ����}')'
 * FIRST(<�����>) = {SCANFTK}
*/
PARSE_RETURN GrammaticalParser::__read_statement(PARSE_HEAD head) {
	FLAG_ENTER("<�����>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		SYMBOL_CHECK(SYMBOL::SCANFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);
		RECUR_CHECK(__idenfr, RECUR_DEFAULT);
		while (_peek()->equal(SYMBOL::COMMA)) {
			SYMBOL_CHECK(SYMBOL::COMMA);
			RECUR_CHECK(__idenfr, RECUR_DEFAULT);
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
 * ��д��䣾 ::= printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'| printf '('�����ʽ��')'
 * FISRT(<�����>) = {PRINTFTK}
 * FISRT(<�ַ���>) = {STRCON},
 * FISRT(<���ʽ>) = {PLUS, MINU, FIRST(��)} = {IDENFR} + {IDENFR} + {LPARENT} + {PLUS, MINU, INTCON} + {CHARCON}
 *
 * !: ���ж��ַ�������ʽ����ʱ��Ҳ���Բ���if-else�ͣ����ַ��������ʽ��
*/
PARSE_RETURN GrammaticalParser::__write_statement(PARSE_HEAD head) {
	FLAG_ENTER("<д���>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };

	try {
		SYMBOL exp_first[] =
		{ SYMBOL::IDENFR, SYMBOL::LPARENT, SYMBOL::PLUS, SYMBOL::MINU,
		  SYMBOL::INTCON, SYMBOL::CHARCON };

		SYMBOL_CHECK(SYMBOL::PRINTFTK);
		SYMBOL_CHECK(SYMBOL::LPARENT);

		// printf '(' ���ַ�����,�����ʽ�� ')'| printf '('���ַ����� ')'
		if (_peek()->equal(SYMBOL::STRCON)) {
			RECUR_CHECK(__string, RECUR_DEFAULT);
			if (_peek()->equal(SYMBOL::COMMA)) {
				SYMBOL_CHECK(SYMBOL::COMMA);
				RECUR_CHECK(__expression, RECUR_DEFAULT);
			}
		}
		// printf '('�����ʽ��')'
		else if (_peek()->equal(exp_first, 6)) {
			RECUR_CHECK(__expression, RECUR_DEFAULT);
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
 * ��������䣾::=  return['('�����ʽ��')']
 * FISRT('('�����ʽ��')') = {LPARENT};
 * FOLLOW{<�������>} = {SEMICN}���޽��漯��
*/
PARSE_RETURN GrammaticalParser::__return_statement(PARSE_HEAD head, bool * has_return) {
	FLAG_ENTER("<�������>", head.level);
	PARSE_HEAD RECUR_DEFAULT = PARSE_HEAD{ head.level + 1 };
	try {
		SYMBOL_CHECK(SYMBOL::RETURNTK);
		if (_peek()->equal(LPARENT)) {
			SYMBOL_CHECK(SYMBOL::LPARENT);
			// RECUR_CHECK(__expression, RECUR_DEFAULT);
			bool char_detector = false;
			__expression(RECUR_DEFAULT, &char_detector);
			// unit4-error-h
			Block* block = symbol_table.get_present_block();
			if (block->func_head.returnType == "int" && char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			else if (block->func_head.returnType == "char" && !char_detector) {
				_register_error(token->line, ErrorType::ReturnError);
			}
			SYMBOL_CHECK(SYMBOL::RPARENT);
			*has_return = true;
		}
	}
	catch (ParseException& e) {
		FLAG_FAIL;
		throw e;
	}
	FLAG_PASS;
	return PARSE_RETURN{ 0 };
}