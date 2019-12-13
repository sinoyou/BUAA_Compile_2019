
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include "GrammaticalParser.h"
#include <cstdio>
#include <vector>
#include <tuple>
#include <fstream>
#include <string>
#include "ParseException.h"
#include "Quaternary.h"
#include "mips_generator.h"
#include "register_mips_generator.h"

// optimize
#include "InlineOptimizer.h"
#include "SeekOptimizer.h"
#include "basic_block_debug.h"
#include "BasicBlock.h"

#define OPTIMIZE 1

// Global Variable and Pointer
FileReader reader("testfile.txt");
ofstream log_file("log.txt", ios::out | ios::trunc);					// 通用日志
ofstream register_file("log_register.txt", ios::out | ios::trunc);		// 寄存器分配日志
ofstream basic_block_file("log_block.txt", ios::out | ios::trunc);		// 基本块日志

ofstream gram("output.txt", ios::out | ios::trunc);
ofstream error_out("error.txt", ios::out | ios::trunc);
ofstream quater_out("middle.txt", ios::out | ios::trunc);
ofstream mips_out("mips.txt", ios::out | ios::trunc);

void run() {
	vector<tuple<int, string>> error_output_list;

	/* Lexcial Parse */
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
	vector<string>::iterator itr1 = output_list.begin();
	while (itr1 != output_list.end()) {
		gram << itr1->c_str() << endl;
		itr1++;
	}
	gram.close();

	// error output
	vector<tuple<int, string>>::iterator error_it = error_output_list.begin();
	while (error_it != error_output_list.end()) {
		tuple<int, string> t = *error_it;
		error_out << get<0>(t) << " " << get<1>(t) << endl;
		error_it += 1;
	}
	error_out.close();

	cout << QuaterList.size() << endl;
	
	
	// quaternary output
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
	}

	

	// Basic Blocks and Data Flow
	map<Quaternary*, BasicBlock*> quater_block_map;
	auto block_list = generate_basic_blocks(QuaterList, &quater_block_map);
	dump_basic_blocks(block_list, "basic_block.txt");
	
	if (OPTIMIZE == 1) {
		SeekOptimizer* seek_opt = new SeekOptimizer(QuaterList, quater_block_map);
		QuaterList = seek_opt->get_optimized_quaters();
		seek_opt->dumps();
	}

	// object out
	if (OPTIMIZE == 1) {
		RegMipsGenerator object_gen(QuaterList, quater_block_map);
		vector<string> mips = object_gen.simple_dump();
		for (auto it = mips.begin(); it != mips.end(); it++) {
			mips_out << *it << endl;
		}
		mips_out.close();
	}
	else {
		MipsGenerator object_gen(QuaterList);
		vector<string> mips = object_gen.simple_dump();
		for (auto it = mips.begin(); it != mips.end(); it++) {
			mips_out << *it << endl;
		}
		mips_out.close();
	}
	
}

int main()
{
	run();
	return 0;
}