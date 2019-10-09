
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include "GrammaticalParser.h"
#include <cstdio>
#include <vector>

// Global Variable and Pointer
FileReader* reader;
FILE* f;

int main()
{
	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser(reader);
	vector<Token> token_list = lexParser.parse();

	/* Grammatical Parse */
	vector<string> output_list;
	GrammaticalParser gram_parser(token_list,output_list);

	/* Course Evaluation Output */
	f = fopen("output.txt", "w");
	vector<Token>::iterator itr = token_list.begin();
	while (itr!= token_list.end()) {
		print_token(f,itr->symbol, itr->token);
		itr++;
	}
	fclose(f);
	return 0;
}