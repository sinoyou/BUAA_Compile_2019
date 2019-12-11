#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cstdio>
#include <fstream>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

extern ofstream log_file;				// ͨ����־
extern ofstream register_file;			// �Ĵ���������־
extern ofstream basic_block_file;	// ��������־


// ��ʾ�����ļ��������кŵ�debug���
#define DEBUG_PRINT(msg) fprintf(stdout, "line: %d, file : %s %s \n", __LINE__, __FILE__, msg); 

/* ͨ����־ */
// ������GrammaticalParser�﷨���������
#define RECUR_PRINT(head, level, rule)	{						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += "   ";				\
	log_file << space << head << " " << rule << endl;				\
}

// ������GrammaticalParser�﷨���������
#define PARSE_PRINT(level, token, symbol){						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += "   ";				\
	log_file << space << " @ " << token << " " << symbol << endl;				\
}

/* �Ĵ���������־ */
void DEBUG_REGISTER(string func_name, string info);
void ERROR_REGISTER(string func_name, string info);

/* �����������־ */
void DEBUG_BASICBLOCK(string info);

#endif // !__DEBUG_H__
	
