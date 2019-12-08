
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
#include "mips_generator.h"
#include "InlineOptimizer.h"

#define OPTIMIZE 1

// Global Variable and Pointer
FileReader* reader;
FILE* f;
ofstream log_file("log.txt", ios::out | ios::trunc);

void run() {
	vector<tuple<int, string>> error_output_list;

	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser(reader, error_output_list);
	vector<Token> token_list = lexParser.parse();
	for (vector<Token>::iterator it = token_list.begin(); it != token_list.end(); it++) {
		log_file << it->line << " " << it->token << " " << symbol_dict[it->symbol] << endl;
	}

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
	
	// quaternary output
	ofstream quater_out;
	quater_out.open("middle.txt", ios::out | ios::trunc);
	vector<string> quater_out_list = PrintQuater(&QuaterList);
	for (auto it = quater_out_list.begin(); it != quater_out_list.end(); it++) {
		quater_out << *it << endl;
	}
	quater_out.close();
	
	// Inline Optimize
	if (OPTIMIZE == 1) {
		InlineOptimizer* inline_opt = new InlineOptimizer(QuaterList);
		QuaterList = inline_opt->get_optimized_quaters();
		inline_opt->dumps();
		QuaterList = inline_opt->get_optimized_quaters();
	}
	
	// object out
	ofstream mips_out;
	mips_out.open("mips.txt", ios::out | ios::trunc);
	MipsGenerator object_gen(QuaterList);
	vector<string> mips = object_gen.simple_dump();
	for (auto it = mips.begin(); it != mips.end(); it++) {
		mips_out << *it << endl;
	}
	mips_out.close();
	/**/
}

int main()
{
	run();
	/*
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	auto begin = v.begin() + 1;
	auto end = v.begin() + 3;
	auto it = v.insert(v.begin() + 4, begin, end);
	printf("%d", *it);
	*/
	return 0;
}