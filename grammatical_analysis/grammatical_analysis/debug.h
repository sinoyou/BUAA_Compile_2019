#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cstdio>
#include <fstream>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

extern ofstream log_file;				// 通用日志
extern ofstream register_file;			// 寄存器分配日志
extern ofstream basic_block_file;	// 基本块日志


// 显示代码文件名称与行号的debug输出
#define DEBUG_PRINT(msg) fprintf(stdout, "line: %d, file : %s %s \n", __LINE__, __FILE__, msg); 

/* 通用日志 */
// 适用于GrammaticalParser语法解析的输出
#define RECUR_PRINT(head, level, rule)	{						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += "   ";				\
	log_file << space << head << " " << rule << endl;				\
}

// 适用于GrammaticalParser语法解析的输出
#define PARSE_PRINT(level, token, symbol){						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += "   ";				\
	log_file << space << " @ " << token << " " << symbol << endl;				\
}

/* 寄存器分配日志 */
void DEBUG_REGISTER(string func_name, string info);
void ERROR_REGISTER(string func_name, string info);

/* 基本块分配日志 */
void DEBUG_BASICBLOCK(string info);

#endif // !__DEBUG_H__
	
