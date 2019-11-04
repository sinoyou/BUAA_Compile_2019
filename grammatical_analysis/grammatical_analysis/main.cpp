
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include "GrammaticalParser.h"
#include <cstdio>
#include <vector>
#include <fstream>
#include "error.h"

// Global Variable and Pointer
FileReader* reader;
FILE* f;

void run() {
	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser(reader);
	vector<Token> token_list = lexParser.parse();

	/* Grammatical Parse */
	vector<string> output_list;
	GrammaticalParser gram_parser(token_list, output_list);

	/* Lexical : Course Evaluation Output */
	//f = fopen("output.txt", "w");
	//vector<Token>::iterator itr = token_list.begin();
	//while (itr!= token_list.end()) {
	//	print_token(f,itr->symbol, itr->token);
	//	itr++;
	//}
	//fclose(f);

	/* Grammatical Parse :  */
	gram_parser.parse();
	ofstream gram;
	gram.open("output.txt", ios::out | ios::trunc);
	vector<string>::iterator itr1 = output_list.begin();
	while (itr1 != output_list.end()) {
		gram << itr1->c_str() << endl;
		itr1++;
	}
	gram.close();
}

int main()
{
	try {
		throw ParseException(ErrorType::Unknown, "HAHAHA");
	}
	catch (ParseException& e) {
		cout << e.what() << endl;
		cout << e.errorType;
	}
	return 0;
}