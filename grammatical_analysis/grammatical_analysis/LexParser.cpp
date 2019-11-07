#include "LexParser.h"
#include "utils.h"

using namespace std;

LexParser::LexParser(FileReader& reader, vector<tuple<int,string>> error_list) 
	:reader(reader), error_output_list(error_list), current_line(1) {}

const vector<Token>& LexParser::parse() {
	while (_getsym() >= 0) {
		if(symbol != SYMBOL::UNKNOWN)
			token_list.push_back(Token(symbol, reader.getToken(), current_line));
		/* Ĭ�ϣ���ǰ��������Ϊ��һ�������趨��ָ��  */
		reader.getchar();
	}
	return token_list;
}

/* �ʷ��������� */
int LexParser::_getsym() {
	reader.clearToken();
	/* ����޹ؿհ׷� */
	while (reader.isSpace() || reader.isNewlineN() || reader.isNewlineR() || reader.isTab()) {
		if (reader.isNewlineN()) {
			current_line += 1;
		}
		reader.getchar();
	}

	/* ö�����ȼ��������� - ��ʶ�� - ������ - �ֽ�� - �ַ� - �ַ��� */
	/* ���������ʶ�� */
	if (reader.isLetter()) {
		while (reader.isLetter() || reader.isDigit()) {
			reader.catToken();
			reader.getchar();
		}
		reader.retract();
		SYMBOL resultvalue = reserver(reader.getToken());
		if (resultvalue == 0)
			symbol = SYMBOL::IDENFR;
		else
			symbol = resultvalue;
	}
	/* ������ */
	// todo : �Ƿ�����ǰ���㣿
	else if (reader.isDigit()) {
		// non-zero : must start with a non-zero number.
		if (reader.isNZeroDigit()) {
			while (reader.isDigit()) {
				reader.catToken();
				reader.getchar();
			}
			reader.retract();
			symbol = SYMBOL::INTCON;
		}
		// zero : must only 1 '0'.
		else {
			reader.catToken();
			reader.getchar();
			if (!reader.isDigit()) {
				reader.retract();
				symbol = SYMBOL::INTCON;
			}
			else
				_error("INTCON");
		}
	}
	/* ˫�ֽ�� */
	else if (reader.isLss()) { // < | <=
		reader.catToken();
		reader.getchar();
		if (reader.isEqu()) {
			reader.catToken();
			symbol = SYMBOL::LEQ;
		}
		else {
			reader.retract();
			symbol = SYMBOL::LSS;
		}
	}
	else if (reader.isGre()) { // > | >=
		reader.catToken();
		reader.getchar();
		if (reader.isEqu())
		{
			reader.catToken();
			symbol = SYMBOL::GEQ;
		}
		else {
			reader.retract();
			symbol = SYMBOL::GRE;
		}
	}
	else if (reader.isEqu()) { // = | ==
		reader.catToken();
		reader.getchar();
		if (reader.isEqu())
		{
			reader.catToken();
			symbol = SYMBOL::EQL;
		}
		else
		{
			reader.retract();
			symbol = SYMBOL::ASSIGN;
		}
	}
	else if (reader.isExclam()) { // !=
		reader.catToken();
		reader.getchar();
		if (reader.isEqu())
		{
			reader.catToken();
			symbol =SYMBOL::NEQ;
		}
		else {
			// todo
			_error("!=");
		}
	}
	/* ���ֽ���� */
	else if (reader.isPlus()) reader.catToken(), symbol = SYMBOL::PLUS;
	else if (reader.isMinu()) reader.catToken(), symbol = SYMBOL::MINU;
	else if (reader.isMult()) reader.catToken(), symbol = SYMBOL::MULT;
	else if (reader.isDiv())  reader.catToken(), symbol = SYMBOL::DIV;
	else if (reader.isSemicn()) reader.catToken(), symbol = SYMBOL::SEMICN;
	else if (reader.isComma()) reader.catToken(), symbol = SYMBOL::COMMA;
	else if (reader.isLparent()) reader.catToken(), symbol = SYMBOL::LPARENT;
	else if (reader.isRparent()) reader.catToken(), symbol = SYMBOL::RPARENT;
	else if (reader.isLbrack()) reader.catToken(), symbol = SYMBOL::LBRACK;
	else if (reader.isRbrack()) reader.catToken(), symbol = SYMBOL::RBRACK;
	else if (reader.isLbrace()) reader.catToken(), symbol = SYMBOL::LBRACE;
	else if (reader.isRbrace()) reader.catToken(), symbol = SYMBOL::RBRACE;
	/* �ַ� */
	else if (reader.isSinQuotation()) {
		reader.getchar();
		// todo : �ɼ��ַ����ɻ�����Ҫ�����ķ��������?
		if (reader.isDigit() || reader.isLetter() || reader.isPlus() || reader.isMinu() || reader.isMult() || reader.isDiv()) {
			reader.catToken();
			reader.getchar();
			if (reader.isSinQuotation())
				symbol = SYMBOL::CHARCON;
			else
				_error("CHARCON");
		}
	}
	/* �ַ��� */
	else if (reader.isDouQuotation()) {
		// ת�÷� ��ʱ������
		reader.getchar();
		while (reader.isChar() && !reader.isDouQuotation()) {
			reader.catToken();
			reader.getchar();
		}
		if (reader.isDouQuotation()) {
			symbol = SYMBOL::STRCON;
		}
		else {
			_error("STRCON");
		}
	}
	else if (reader.isEOT()) {
		return -1;
	}
	else {
		symbol = SYMBOL::UNKNOWN;
		_error("UNKNOWN");
	}
	return 0;
}

void LexParser::_error(const char * s) {
	printf("We Face an Error When Parsing %s.\n", s);
}