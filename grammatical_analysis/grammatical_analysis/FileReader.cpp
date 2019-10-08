#include "FileReader.h"
#include <fstream>

FileReader::FileReader(const char* path) {
	// initial members
	token.clear();
	text.clear();
	ptr = 0;
	ifstream source(path);
	if (!source.is_open()) {
		printf("[Error] Cat not open source code file %s.\n", path);
	}
	else {
		char c;
		while ((c = source.get()) != EOF) {
			text += c;
		}
	}
	source.close();
}

void FileReader::catToken() {
	token = token + text[ptr];
}

void FileReader::clearToken() {
	token.clear();
}

void FileReader::getchar() {
	ptr += 1;
}

void FileReader::retract() {
	ptr -= 1;
}
