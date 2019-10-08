
#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include <cstdio>
#include <vector>

// Global Variable and Pointer
FileReader* reader;
FILE* f;

int main()
{
	/* Lexcial Parse */
	FileReader reader("testfile.txt");
	LexParser lexParser = LexParser(reader);
	vector<Token> token_list_ptr = lexParser.parse();

	/* Grammatical Parse */

	/* Course Evaluation Output */
	f = fopen("output.txt", "w");
	vector<Token>::iterator itr = token_list_ptr.begin();
	while (itr!= token_list_ptr.end()) {
		print_token(f,itr->symbol, itr->token);
		itr++;
	}
	fclose(f);
	return 0;
}