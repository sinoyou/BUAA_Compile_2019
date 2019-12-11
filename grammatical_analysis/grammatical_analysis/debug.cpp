#include "debug.h"

void DEBUG_REGISTER(string func_name, string info) {
	register_file << "[DEBUG] @" + func_name + ": " + info << endl;
}

void ERROR_REGISTER(string func_name, string info) {
	// register_file << "[ERROR] @" + func_name + ": " + info;
	cout << "[ERROR] @" + func_name + ": " + info << endl;
}

void DEBUG_BASICBLOCK(string info) {
	basic_block_file << "[DEBUG]  "<< info << endl;
}