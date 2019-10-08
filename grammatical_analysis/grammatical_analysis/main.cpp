// token_analysis.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "FileReader.h"
#include "utils.h"
#include "LexParser.h"
#include <cstdio>
#include <list>

// Global Variable and Pointer
FileReader* reader;
FILE* f;

int main()
{
	// initial output
	f = fopen("output.txt", "w");
	// initial reader class
	FileReader t("testfile.txt");
	reader = &t;

	LexParser lexParser = LexParser(reader);
	list<Token>* token_list_ptr = lexParser.parse();

	list<Token>::iterator itr = token_list_ptr->begin();
	while (itr!= token_list_ptr->end()) {
		print_token(f,itr->symbol, itr->token);
		itr++;
	}
	fclose(f);
	return 0;
}