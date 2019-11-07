
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include "GrammaticalParser.h"
#include <cstdio>
#include <vector>
#include <tuple>
#include <fstream>
#include "ParseException.h"

// Global Variable and Pointer
FileReader* reader;
FILE* f;

void run() {
	vector<tuple<int, string>> error_output_list;

	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser(reader, error_output_list);
	vector<Token> token_list = lexParser.parse();

	/* Grammatical Parse + error process */
	vector<string> output_list;
	GrammaticalParser gram_parser(token_list, output_list, error_output_list);

	/* Lexical : Course Evaluation Output */
	//f = fopen("output.txt", "w");
	//vector<Token>::iterator itr = token_list.begin();
	//while (itr!= token_list.end()) {
	//	print_token(f,itr->symbol, itr->token);
	//	itr++;
	//}
	//fclose(f);

	/* Grammatical Parse :  Recursive Output + Error Output */
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
	run();
	return 0;
}