
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include "GrammaticalParser.h"
#include <cstdio>
#include <vector>
#include <tuple>
#include <fstream>
#include "ParseException.h"
#include "Quaternary.h"

// Global Variable and Pointer
FileReader* reader;
FILE* f;

void run() {
	vector<tuple<int, string>> error_output_list;

	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser(reader, error_output_list);
	vector<Token> token_list = lexParser.parse();
	for (vector<Token>::iterator it = token_list.begin(); it != token_list.end(); it++) {
		cout << it->line << " " << it->token << " " << symbol_dict[it->symbol] << endl;
	}

	/* Lexical : Course Evaluation Output */
	//f = fopen("output.txt", "w");
	//vector<Token>::iterator itr = token_list.begin();
	//while (itr!= token_list.end()) {
	//	print_token(f,itr->symbol, itr->token);
	//	itr++;
	//}
	//fclose(f);

	/* Grammatical Parse :  Recursive Output + Error Output */
	// recursive output
	
	vector<string> output_list;
	GrammaticalParser gram_parser(token_list, output_list, error_output_list);
	gram_parser.parse();

	// grammtical parse output
	ofstream gram;
	gram.open("output.txt", ios::out | ios::trunc);
	vector<string>::iterator itr1 = output_list.begin();
	while (itr1 != output_list.end()) {
		gram << itr1->c_str() << endl;
		itr1++;
	}
	gram.close();

	// error output
	ofstream error_out;
	error_out.open("error.txt", ios::out | ios::trunc);
	vector<tuple<int, string>>::iterator error_it = error_output_list.begin();
	while (error_it != error_output_list.end()) {
		tuple<int, string> t = *error_it;
		error_out << get<0>(t) << " " << get<1>(t) << endl;
		error_it += 1;
	}
	error_out.close();

	cout << QuaterList.size() << endl;
}

int main()
{
	run();
	return 0;
}